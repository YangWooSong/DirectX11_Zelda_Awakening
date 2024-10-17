#include "stdafx.h"
#include "Marin_State_Idle.h"
#include "GameInstance.h"
#include "Marin.h"
#include "PartObject.h"
#include "Link.h"

CMarin_State_Idle::CMarin_State_Idle(CFsm* pFsm, CNPC* pOwner)
	:CState{ pFsm }
	, m_pOwner{ pOwner }
{
}

HRESULT CMarin_State_Idle::Initialize(_uint iStateNum)
{
	m_iCurrentAnimIndex = m_pOwner->Get_Model()->Get_AnimationIndex("wait");
	m_iStateNum = iStateNum;

	m_pPlayer = static_cast<CLink*>(m_pGameInstance->Find_Player(LEVEL_MARINHOUSE));
	return S_OK;
}


HRESULT CMarin_State_Idle::Start_State()
{
	m_pOwner->Get_Model()->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.1f, true);
	m_pOwner->Get_Model()->Set_AnimationSpeed(m_iCurrentAnimIndex, 50.f);

	return S_OK;
}

void CMarin_State_Idle::Update(_float fTimeDelta)
{
	LookAtPlayer(fTimeDelta);
}

void CMarin_State_Idle::End_State()
{
}

void CMarin_State_Idle::LookAtPlayer(_float fTimerDelta)
{
	_vector newLook = XMVector3Normalize(m_pPlayer->Get_Position() - m_pOwner->Get_Transform()->Get_State(CTransform::STATE_POSITION));
	m_pOwner->Get_Transform()->Turn_Lerp(newLook, 0.6f, fTimerDelta);
}

CMarin_State_Idle* CMarin_State_Idle::Create(CFsm* pFsm, CNPC* pOwner, _uint iStateNum)
{
	CMarin_State_Idle* pInstance = new CMarin_State_Idle(pFsm, pOwner);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CMarin_State_Idle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}
void CMarin_State_Idle::Free()
{
	__super::Free();
}