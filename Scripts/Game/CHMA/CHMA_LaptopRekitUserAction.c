class CHMA_LaptopRekitUserAction : ScriptedUserAction
{
	[Attribute("0", UIWidgets.EditBox, "Kit index into CHMA_RekitTerminalComponent kit list.", "0 999")]
	protected int m_iKitIndex;

	[Attribute("Rifleman", UIWidgets.EditBox, "Action label shown in the interaction menu.")]
	protected string m_sKitName;

	override bool GetActionNameScript(out string outName)
	{
		outName = string.Format("Grab Kit: %1", m_sKitName);
		return true;
	}

	override bool CanBePerformedScript(IEntity user)
	{
		IEntity owner = GetOwner();
		if (!owner || !user)
			return false;

		CHMA_RekitTerminalComponent term = CHMA_RekitTerminalComponent.Cast(owner.FindComponent(CHMA_RekitTerminalComponent));
		return term && term.HasKitIndex(m_iKitIndex);
	}

	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		CHMA_RekitTerminalComponent term = CHMA_RekitTerminalComponent.Cast(pOwnerEntity.FindComponent(CHMA_RekitTerminalComponent));
		if (!term)
			return;

		term.RequestRekit(pUserEntity, m_iKitIndex);
	}

	override bool CanBroadcastScript()
	{
		return true;
	}

	override bool HasLocalEffectOnlyScript()
	{
		return false;
	}
}
