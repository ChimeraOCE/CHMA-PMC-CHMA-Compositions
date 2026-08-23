//------------------------------------------------------------------------------------------------
//! CHMA Logistics - Repack a crate
//!
//! Goes on the crate prefab. Tops the crate back up to its manifest in one
//! action instead of dragging magazines across one at a time.
//!
//! Only available while the crate is inside a CHMA_LogiPointComponent radius,
//! and only when the crate is actually short of something.
//------------------------------------------------------------------------------------------------

[BaseContainerProps()]
class CHMA_RepackCrateAction : ScriptedUserAction
{
	[Attribute("Repack Crate", desc: "Text shown on the action")]
	protected string m_sActionName;

	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		CHMA_CratePreloadComponent manifest = CHMA_CratePreloadComponent.Cast(
			pOwnerEntity.FindComponent(CHMA_CratePreloadComponent));

		if (!manifest)
		{
			Print("[CHMA] Repack action on an entity with no CHMA_CratePreloadComponent.", LogLevel.ERROR);
			return;
		}

		// RestockToManifest guards authority itself
		manifest.RestockToManifest();
	}

	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
	{
		IEntity owner = GetOwner();
		if (!owner)
			return false;

		if (!CHMA_LogiPointComponent.IsInRange(owner.GetOrigin()))
			return false;

		CHMA_CratePreloadComponent manifest = CHMA_CratePreloadComponent.Cast(
			owner.FindComponent(CHMA_CratePreloadComponent));

		return manifest && manifest.NeedsRestock();
	}

	//------------------------------------------------------------------------------------------------
	override bool GetActionNameScript(out string outName)
	{
		outName = m_sActionName;
		return true;
	}

	//------------------------------------------------------------------------------------------------
	override bool CanBroadcastScript()
	{
		return false;
	}
}
