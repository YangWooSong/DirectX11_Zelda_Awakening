#include "stdafx.h"
#include "State_ToolShopkeeper_Attack.h"
#include "GameInstance.h"
#include "ToolShopkeeper.h"
#include "PartObject.h"

CState_ToolShopkeeper_Attack::CState_ToolShopkeeper_Attack(CFsm* pFsm, CNPC* pOwner)
	:CState{ pFsm }
	, m_pOwner{ pOwner }
{
}

HRESULT CState_ToolShopkeeper_Attack::Initialize(_uint iStateNum)
{
	m_iBeamStAnimIndex = m_pOwner->Get_Model()->Get_AnimationIndex("beam_st");
	m_iBeamLpAnimIndex = m_pOwner->Get_Model()->Get_AnimationIndex("beam_lp");

	m_iStateNum = iStateNum;

	return S_OK;
}


HRESULT CState_ToolShopkeeper_Attack::Start_State()
{
	m_iCurrentAnimIndex = m_iBeamStAnimIndex;

	m_pOwner->Get_Model()->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.02f);
	m_pOwner->Get_Model()->Set_AnimationSpeed(m_iCurrentAnimIndex, 40.f);

	return S_OK;
}

void CState_ToolShopkeeper_Attack::Update(_float fTimeDelta)
{

}

void CState_ToolShopkeeper_Attack::End_State()
{
}

CState_ToolShopkeeper_Attack* CState_ToolShopkeeper_Attack::Create(CFsm* pFsm, CNPC* pOwner, _uint iStateNum)
{
	CState_ToolShopkeeper_Attack* pInstance = new CState_ToolShopkeeper_Attack(pFsm, pOwner);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_ToolShopkeeper_Attack"));
		Safe_Release(pInstance);
	}

	return pInstance;
}
void CState_ToolShopkeeper_Attack::Free()
{
	__super::Free();
}