#include "stdafx.h"
#include "State_GrandmaUlrira_Idle.h"
#include "GameInstance.h"
#include "GrandmaUlrira.h"
#include "PartObject.h"
#include "Link.h"

CState_GrandmaUlrira_Idle::CState_GrandmaUlrira_Idle(CFsm* pFsm, CNPC* pOwner)
	:CState{ pFsm }
	, m_pOwner{ pOwner }
{
}

HRESULT CState_GrandmaUlrira_Idle::Initialize(_uint iStateNum)
{
	m_iCurrentAnimIndex = m_pOwner->Get_Model()->Get_AnimationIndex("wait_broom");
	m_iStateNum = iStateNum;

	m_pPlayer = static_cast<CLink*>(m_pGameInstance->Find_Player(LEVEL_MARINHOUSE));
	return S_OK;
}


HRESULT CState_GrandmaUlrira_Idle::Start_State()
{
	m_pOwner->Get_Model()->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.1f, true);
	m_pOwner->Get_Model()->Set_AnimationSpeed(m_iCurrentAnimIndex, 40.f);

	return S_OK;
}

void CState_GrandmaUlrira_Idle::Update(_float fTimeDelta)
{
	LookAtPlayer(fTimeDelta);
}

void CState_GrandmaUlrira_Idle::End_State()
{
}

void CState_GrandmaUlrira_Idle::LookAtPlayer(_float fTimerDelta)
{
	//_vector newLook = XMVector3Normalize(m_pPlayer->Get_Position() - m_pOwner->Get_Transform()->Get_State(CTransform::STATE_POSITION));
	//m_pOwner->Get_Transform()->Turn_Lerp(newLook, 0.8f, fTimerDelta);
}

CState_GrandmaUlrira_Idle* CState_GrandmaUlrira_Idle::Create(CFsm* pFsm, CNPC* pOwner, _uint iStateNum)
{
	CState_GrandmaUlrira_Idle* pInstance = new CState_GrandmaUlrira_Idle(pFsm, pOwner);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_GrandmaUlrira_Idle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}
void CState_GrandmaUlrira_Idle::Free()
{
	__super::Free();
}