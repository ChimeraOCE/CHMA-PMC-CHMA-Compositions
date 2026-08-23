//------------------------------------------------------------------------------------------------
//! CHMA Logistics - Logistics point
//!
//! Marks a location as a place where crates can be requested and repacked.
//! Put this on a prefab you place at MOB and at each FOB. Make the prefab an
//! editable entity so a GM can drop new ones when a FOB is established mid-op.
//!
//! Points register themselves in a static list, so the range check is a short
//! loop rather than a world query on every action-manager tick.
//------------------------------------------------------------------------------------------------

[EntityEditorProps(category: "CHMA/Logistics", description: "Marks a MOB/FOB as a supply crate point.", color: "0 200 0 255")]
class CHMA_LogiPointComponentClass : ScriptComponentClass
{
}

class CHMA_LogiPointComponent : ScriptComponent
{
	[Attribute("30", desc: "Radius within which crates can be requested and repacked (m)", params: "5 200")]
	protected float m_fRadius;

	[Attribute("MOB", desc: "Name shown in logs / debug")]
	protected string m_sPointName;

	protected static ref array<CHMA_LogiPointComponent> s_aPoints = {};

	//------------------------------------------------------------------------------------------------
	//! True if the given world position falls inside any active logistics point.
	static bool IsInRange(vector pos)
	{
		foreach (CHMA_LogiPointComponent point : s_aPoints)
		{
			if (!point)
				continue;

			IEntity owner = point.GetOwner();
			if (!owner)
				continue;

			if (vector.DistanceSq(pos, owner.GetOrigin()) <= point.m_fRadius * point.m_fRadius)
				return true;
		}

		return false;
	}

	//------------------------------------------------------------------------------------------------
	static int GetPointCount()
	{
		return s_aPoints.Count();
	}

	//------------------------------------------------------------------------------------------------
	float GetRadius()
	{
		return m_fRadius;
	}

	//------------------------------------------------------------------------------------------------
	string GetPointName()
	{
		return m_sPointName;
	}

	//------------------------------------------------------------------------------------------------
	override void OnPostInit(IEntity owner)
	{
		if (!GetGame().InPlayMode())
			return;

		s_aPoints.Insert(this);
	}

	//------------------------------------------------------------------------------------------------
	override void OnDelete(IEntity owner)
	{
		s_aPoints.RemoveItem(this);
	}
}
