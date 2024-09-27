#include "stdafx.h"
#include "State_Link_Push.h"
#include "GameInstance.h"
#include "Link.h"
#include "PartObject.h"

CState_Link_Push::CState_Link_Push(CFsm* pFsm, CPlayer* pPlayer)
	:CState{ pFsm }
	, m_pPlayer{ pPlayer }
{
}

HRESULT CState_Link_Push::Initialize(_uint iStateNum)
{
	m_iCurrentAnimIndex = m_pPlayer->Get_Model()->Get_AnimationIndex("push");
	m_iStateNum = iStateNum;

	return S_OK;
}


HRESULT CState_Link_Push::Start_State()
{
	m_pPlayer->Get_Model()->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.02f);
	m_pPlayer->Get_Model()->Set_AnimationSpeed(m_iCurrentAnimIndex, 60.f);

	m_pPlayer->Get_PlayerSound()->Play_Sound(TEXT("1_Link_Jump.wav"), 1.f);
	static_cast<CLink*>(m_pPlayer)->Set_ActiveSheild(false);
	static_cast<CLink*>(m_pPlayer)->Set_ActiveSword(false);

	return S_OK;
}

void CState_Link_Push::Update(_float fTimeDelta)
{

	if (m_pPlayer->Get_IsEnd_CurrentAnimation())
	{
		m_pPlayer->Change_State(CLink::IDLE);
	}

}

void CState_Link_Push::End_State()
{
	static_cast<CLink*>(m_pPlayer)->Set_ActiveSheild(true);
	static_cast<CLink*>(m_pPlayer)->Set_ActiveSword(true);
}

CState_Link_Push* CState_Link_Push::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum)
{
	CState_Link_Push* pInstance = new CState_Link_Push(pFsm, pPlayer);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Link_Push"));
		Safe_Release(pInstance);
	}

	return pInstance;
}
void CState_Link_Push::Free()
{
	__super::Free();
}