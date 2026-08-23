//------------------------------------------------------------------------------------------------
//! CHMA Logistics - Request a crate
//!
//! Goes on the logistics point prefab. Add one instance per crate type
//! (ammo, medical) and point each at the matching crate prefab.
//!
//! The action is broadcast so the cooldown timestamp is set on every machine,
//! which means clients grey the action out correctly without needing an RplProp.
//! The actual spawn is still guarded to authority.
//------------------------------------------------------------------------------------------------

[BaseContainerProps(), SCR_BaseContainerCustomTitleField("m_sActionName")]
class CHMA_RequestCrateAction : ScriptedUserAction
{
	[Attribute(desc: "Crate prefab to spawn", params: "et")]
	protected ResourceName m_sCratePrefab;

	[Attribute("Request Ammo Crate", desc: "Text shown on the action")]
	protected string m_sActionName;

	[Attribute("120", desc: "Seconds before this action can be used again", params: "0 3600")]
	protected float m_fCooldown;

	[Attribute("0 0 1.5", desc: "Spawn offset in the logi point's local space (fixed, not random)")]
	protected vector m_vSpawnOffset;

	protected float m_fNextAllowedMs;

	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		// Runs on every machine (see CanBroadcastScript) so the cooldown stays in sync
		m_fNextAllowedMs = pOwnerEntity.GetWorld().GetWorldTime() + (m_fCooldown * 1000);

		RplComponent rpl = RplComponent.Cast(pOwnerEntity.FindComponent(RplComponent));
		if (rpl && rpl.IsProxy())
			return;

		if (m_sCratePrefab.IsEmpty())
		{
			Print("[CHMA] Request crate action has no crate prefab assigned.", LogLevel.ERROR);
			return;
		}

		Resource res = Resource.Load(m_sCratePrefab);
		if (!res.IsValid())
		{
			Print("[CHMA] Could not load crate prefab: " + m_sCratePrefab, LogLevel.ERROR);
			return;
		}

		// Fixed offset, snapped to terrain. Deliberately not a random point in a
		// radius - that is how vanilla drops crates onto rotors and roofs.
		vector spawnPos = pOwnerEntity.CoordToParent(m_vSpawnOffset);
		spawnPos[1] = pOwnerEntity.GetWorld().GetSurfaceY(spawnPos[0], spawnPos[2]);

		EntitySpawnParams params = new EntitySpawnParams();
		params.TransformMode = ETransformMode.WORLD;
		params.Transform[3] = spawnPos;

		IEntity crate = GetGame().SpawnEntityPrefab(res, pOwnerEntity.GetWorld(), params);

		if (!crate)
			Print("[CHMA] Crate spawn failed for " + m_sCratePrefab, LogLevel.ERROR);
	}

	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
	{
		return GetRemainingCooldown() <= 0;
	}

	//------------------------------------------------------------------------------------------------
	protected float GetRemainingCooldown()
	{
		IEntity owner = GetOwner();
		if (!owner)
			return 0;

		return (m_fNextAllowedMs - owner.GetWorld().GetWorldTime()) / 1000;
	}

	//------------------------------------------------------------------------------------------------
	override bool GetActionNameScript(out string outName)
	{
		float remaining = GetRemainingCooldown();

		if (remaining > 0)
			outName = m_sActionName + " (" + Math.Ceil(remaining).ToString() + "s)";
		else
			outName = m_sActionName;

		return true;
	}

	//------------------------------------------------------------------------------------------------
	//! Broadcast so every client runs PerformAction and keeps its own cooldown clock.
	override bool CanBroadcastScript()
	{
		return true;
	}

	//------------------------------------------------------------------------------------------------
	override bool HasLocalEffectOnlyScript()
	{
		return false;
	}
}
