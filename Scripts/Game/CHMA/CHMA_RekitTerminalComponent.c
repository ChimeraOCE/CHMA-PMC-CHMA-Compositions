[ComponentEditorProps(category: "CHMA/Rekit", description: "Rekit terminal; switches player's main entity to a selected kit prefab.")]
class CHMA_RekitTerminalComponentClass : ScriptComponentClass
{
	[Attribute("", UIWidgets.ResourceNamePicker, "Kit character prefabs (.et). Index is used by CHMA_LaptopRekitUserAction.", "et")]
	private ref array<ResourceName> m_aKitCharacterPrefabs;

	[Attribute("1500", UIWidgets.EditBox, "Delay (ms) before deleting old body after switching main entity.", "0 5000")]
	private int m_iOldBodyDeleteDelayMs;

	ResourceName GetKitPrefab(int kitIndex)
	{
		if (!m_aKitCharacterPrefabs)
			return ResourceName.Empty;

		if (kitIndex < 0 || kitIndex >= m_aKitCharacterPrefabs.Count())
			return ResourceName.Empty;

		return m_aKitCharacterPrefabs[kitIndex];
	}

	int GetOldBodyDeleteDelayMs()
	{
		if (m_iOldBodyDeleteDelayMs < 0)
			return 0;

		return m_iOldBodyDeleteDelayMs;
	}
}

class CHMA_RekitTerminalComponent : ScriptComponent
{
	protected CHMA_RekitTerminalComponentClass GetData()
	{
		return CHMA_RekitTerminalComponentClass.Cast(GetComponentData(GetOwner()));
	}

	bool HasKitIndex(int kitIndex)
	{
		CHMA_RekitTerminalComponentClass data = GetData();
		if (!data)
			return false;

		ResourceName pf = data.GetKitPrefab(kitIndex);
		return pf != ResourceName.Empty && pf != string.Empty;
	}

	void RequestRekit(IEntity userEntity, int kitIndex)
	{
		if (!userEntity || !HasKitIndex(kitIndex))
			return;

		PlayerManager pm = GetGame().GetPlayerManager();
		if (!pm)
			return;

		int playerId = pm.GetPlayerIdFromControlledEntity(userEntity);
		if (playerId <= 0)
			return;

		if (Replication.IsServer())
			Server_RekitByPlayerId(playerId, kitIndex);
		else
			RpcAsk_Rekit(playerId, kitIndex);
	}

	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void RpcAsk_Rekit(int playerId, int kitIndex)
	{
		Server_RekitByPlayerId(playerId, kitIndex);
	}

	protected bool Server_RekitByPlayerId(int playerId, int kitIndex)
	{
		if (!Replication.IsServer() || !HasKitIndex(kitIndex))
			return false;

		PlayerManager pm = GetGame().GetPlayerManager();
		if (!pm)
			return false;

		SCR_PlayerController pc = SCR_PlayerController.Cast(pm.GetPlayerController(playerId));
		if (!pc)
			return false;

		IEntity oldEntity = pc.GetControlledEntity();
		if (!oldEntity)
			return false;

		ChimeraCharacter oldChar = ChimeraCharacter.Cast(oldEntity);
		if (oldChar)
		{
			CompartmentAccessComponent ca = oldChar.GetCompartmentAccessComponent();
			if (ca && ca.IsInCompartment())
				return false;
		}

		vector mat[4];
		oldEntity.GetTransform(mat);
		mat[3][1] = mat[3][1] + 0.05;

		EntitySpawnParams sp = new EntitySpawnParams();
		sp.TransformMode = ETransformMode.WORLD;
		sp.Transform = mat;

		CHMA_RekitTerminalComponentClass data = GetData();
		if (!data)
			return false;

		ResourceName prefab = data.GetKitPrefab(kitIndex);
		Resource res = Resource.Load(prefab);
		if (!res || !res.IsValid())
			return false;

		IEntity newEntity = GetGame().SpawnEntityPrefab(res, null, sp);
		if (!newEntity)
			return false;

		RplComponent newRpl = RplComponent.Cast(newEntity.FindComponent(RplComponent));
		if (!newRpl)
		{
			SCR_EntityHelper.DeleteEntityAndChildren(newEntity);
			return false;
		}

		Faction faction = SCR_FactionManager.SGetPlayerFaction(playerId);
		if (faction)
			SCR_FactionAffiliationComponent.SetFaction(newEntity, faction);

		pc.SetInitialMainEntity(newEntity);

		int delayMs = data.GetOldBodyDeleteDelayMs();
		GetGame().GetCallqueue().CallLater(DelayedDeleteOldEntity, delayMs, false, oldEntity);

		return true;
	}

	protected void DelayedDeleteOldEntity(IEntity ent)
	{
		if (!Replication.IsServer() || !ent || ent.IsDeleted())
			return;

		RplComponent rpl = RplComponent.Cast(ent.FindComponent(RplComponent));
		if (rpl)
		{
			RplComponent.DeleteRplEntity(ent, false);
			return;
		}

		SCR_EntityHelper.DeleteEntityAndChildren(ent);
	}
}
