//------------------------------------------------------------------------------------------------
//! CHMA Logistics - Crate manifest
//!
//! Defines what a supply crate is packed with, fills it on spawn, and can top it
//! back up to the manifest when repacked at a logistics point.
//!
//! Requires on the same prefab:
//!   - SCR_UniversalInventoryStorageComponent (the actual storage)
//!   - InventoryStorageManagerComponent       (lets the crate manage its own storage)
//------------------------------------------------------------------------------------------------

//! One line of a crate's packing list.
//! BaseContainerProps is required - without it Workbench cannot instantiate
//! entries when you press + on the Contents array.
[BaseContainerProps(), SCR_BaseContainerCustomTitleResourceName("m_sPrefab")]
class CHMA_CratePreloadEntry
{
	[Attribute(defvalue: "", uiwidget: UIWidgets.ResourceNamePicker, desc: "Item prefab to pack into the crate", params: "et")]
	ResourceName m_sPrefab;

	[Attribute(defvalue: "1", desc: "How many of this item the crate should hold when full", params: "1 500 1")]
	int m_iCount;
}

[EntityEditorProps(category: "CHMA/Logistics", description: "Packs a crate with a preset item manifest.", color: "0 200 0 255")]
class CHMA_CratePreloadComponentClass : ScriptComponentClass
{
}

class CHMA_CratePreloadComponent : ScriptComponent
{
	[Attribute(desc: "Contents of this crate when full")]
	protected ref array<ref CHMA_CratePreloadEntry> m_aContents;

	[Attribute("1", desc: "Fill the crate automatically when it spawns")]
	protected bool m_bFillOnSpawn;

	//------------------------------------------------------------------------------------------------
	//! True only on the machine with authority over this entity.
	protected bool IsAuthority()
	{
		RplComponent rpl = RplComponent.Cast(GetOwner().FindComponent(RplComponent));
		return !rpl || !rpl.IsProxy();
	}

	//------------------------------------------------------------------------------------------------
	protected InventoryStorageManagerComponent GetStorageManager()
	{
		InventoryStorageManagerComponent mgr = InventoryStorageManagerComponent.Cast(
			GetOwner().FindComponent(InventoryStorageManagerComponent));

		if (!mgr)
		{
			Print("[CHMA] Crate " + GetOwner() + " has no InventoryStorageManagerComponent - it cannot pack itself. Add one to the prefab.",
				LogLevel.ERROR);
		}
		else
		{
			Print("[CHMA] Resolved storage manager: " + mgr, LogLevel.NORMAL);
		}

		return mgr;
	}

	//------------------------------------------------------------------------------------------------
	//! Counts how many of a given prefab are currently sitting in the crate.
	protected int CountInCrate(notnull array<IEntity> contents, ResourceName prefab)
	{
		int found = 0;

		foreach (IEntity item : contents)
		{
			if (!item)
				continue;

			EntityPrefabData data = item.GetPrefabData();
			if (data && data.GetPrefabName() == prefab)
				found++;
		}

		return found;
	}

	//------------------------------------------------------------------------------------------------
	//! Tops the crate back up to its manifest. Anything already inside is counted,
	//! so a half-empty crate only receives the shortfall. Authority only.
	void RestockToManifest()
	{
		int entryCount = 0;
		if (m_aContents)
			entryCount = m_aContents.Count();

		Print("[CHMA] RestockToManifest called. Authority=" + IsAuthority() + " ContentsRows=" + entryCount, LogLevel.NORMAL);

		if (!IsAuthority())
		{
			Print("[CHMA] Bailed: not authority (this machine is a proxy).", LogLevel.NORMAL);
			return;
		}

		if (!m_aContents || entryCount == 0)
		{
			Print("[CHMA] Bailed: manifest is empty or did not deserialise.", LogLevel.WARNING);
			return;
		}

		InventoryStorageManagerComponent mgr = GetStorageManager();
		if (!mgr)
			return;

		Print("[CHMA] Storage manager found, packing " + entryCount + " manifest rows.", LogLevel.NORMAL);

		// ============================================================================
		// DIAGNOSTIC BUILD - isolates which engine call is killing the script.
		// The normal counting logic is disabled below; restore it once we know.
		// ============================================================================

		CHMA_CratePreloadEntry first = m_aContents[0];
		if (!first)
		{
			Print("[CHMA] Row 0 is null.", LogLevel.ERROR);
			return;
		}

		Print("[CHMA] Row 0 prefab is: " + first.m_sPrefab + " count " + first.m_iCount, LogLevel.NORMAL);

		// --- Test A: does TrySpawnPrefabToStorage survive at all? ---
		Print("[CHMA] TEST A: about to call TrySpawnPrefabToStorage.", LogLevel.NORMAL);

		bool ok = mgr.TrySpawnPrefabToStorage(first.m_sPrefab);

		Print("[CHMA] TEST A: returned " + ok, LogLevel.NORMAL);

		// --- Test B: does GetItems survive? ---
		Print("[CHMA] TEST B: about to call GetItems.", LogLevel.NORMAL);

		array<IEntity> contents = {};
		mgr.GetItems(contents);

		Print("[CHMA] TEST B: returned, crate now holds " + contents.Count() + " items.", LogLevel.NORMAL);

		Print("[CHMA] Both calls survived - the fault is elsewhere.", LogLevel.NORMAL);
	}

	//------------------------------------------------------------------------------------------------
	//! The real packing logic, parked while diagnosing. Call this instead of the
	//! diagnostic body above once we know which engine call is at fault.
	protected void RestockToManifest_Real(notnull InventoryStorageManagerComponent mgr)
	{
		array<IEntity> contents = {};
		mgr.GetItems(contents);

		foreach (CHMA_CratePreloadEntry entry : m_aContents)
		{
			if (!entry || entry.m_sPrefab.IsEmpty())
				continue;

			int have = CountInCrate(contents, entry.m_sPrefab);
			int packed = 0;

			for (int i = have; i < entry.m_iCount; i++)
			{
				// Storage full - stop trying rather than spamming failed inserts
				if (!mgr.TrySpawnPrefabToStorage(entry.m_sPrefab))
				{
					Print("[CHMA] Storage refused item after packing " + packed + " of " + entry.m_sPrefab
						+ " (had " + have + "). Likely out of volume.", LogLevel.WARNING);
					break;
				}

				packed++;
			}

			Print("[CHMA] " + entry.m_sPrefab + ": had " + have + ", added " + packed + ", target " + entry.m_iCount, LogLevel.NORMAL);
		}
	}

	//------------------------------------------------------------------------------------------------
	//! True if the crate is short of anything on its manifest. Used to grey out
	//! the repack action on a crate that is already full.
	bool NeedsRestock()
	{
		if (!m_aContents)
			return false;

		InventoryStorageManagerComponent mgr = InventoryStorageManagerComponent.Cast(
			GetOwner().FindComponent(InventoryStorageManagerComponent));

		if (!mgr)
			return false;

		array<IEntity> contents = {};
		mgr.GetItems(contents);

		foreach (CHMA_CratePreloadEntry entry : m_aContents)
		{
			if (!entry || entry.m_sPrefab.IsEmpty())
				continue;

			if (CountInCrate(contents, entry.m_sPrefab) < entry.m_iCount)
				return true;
		}

		return false;
	}

	//------------------------------------------------------------------------------------------------
	override void EOnInit(IEntity owner)
	{
		Print("[CHMA] EOnInit fired on " + owner, LogLevel.NORMAL);

		if (!GetGame().InPlayMode() || !m_bFillOnSpawn)
		{
			Print("[CHMA] Bailed in EOnInit. InPlayMode=" + GetGame().InPlayMode() + " FillOnSpawn=" + m_bFillOnSpawn, LogLevel.NORMAL);
			return;
		}

		RestockToManifest();
	}

	//------------------------------------------------------------------------------------------------
	override void OnPostInit(IEntity owner)
	{
		Print("[CHMA] OnPostInit fired, setting INIT event mask on " + owner, LogLevel.NORMAL);
		SetEventMask(owner, EntityEvent.INIT);
	}
}