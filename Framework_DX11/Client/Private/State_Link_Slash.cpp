#include "stdafx.h"
#include "State_Link_Slash.h"
#include "GameInstance.h"
#include "Link.h"

CState_Link_Slash::CState_Link_Slash(CFsm* pFsm, CPlayer* pPlayer)
	:CState{ pFsm }
	, m_pPlayer{ pPlayer }
{
}

HRESULT CState_Link_Slash::Initialize(_uint iStateNum)
{
	m_iCurrentAnimIndex = m_pPlayer->Get_Model()->Get_AnimationIndex("slash");
	m_iStateNum = iStateNum;

	return S_OK;
}

HRESULT CState_Link_Slash::Start_State()
{
	m_pPlayer->Get_Model()->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.02f);
	m_pPlayer->Get_Model()->Set_AnimationSpeed(m_iCurrentAnimIndex, 60.f);
	m_pPlayer->Get_PlayerSound()->Play_Sound(TEXT("1_Link_Sword_Slash.wav"), 1.f);
	m_pPlayer->Get_EffectSound()->Play_Sound(TEXT("1_Sword_Swing.wav"), 1.f);
	return S_OK;
}

void CState_Link_Slash::Update(_float fTimeDelta)
{

	if(m_pPlayer->Get_IsEnd_CurrentAnimation())
	{
		m_pPlayer->Change_State(CLink::IDLE);
	}

	if (KEY_AWAY(KEY::P))
	{
		m_pPlayer->Change_State(CLink::ITEMB);
	}
}

void CState_Link_Slash::End_State()
{
}

CState_Link_Slash* CState_Link_Slash::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum)
{
	CState_Link_Slash* pInstance = new CState_Link_Slash(pFsm, pPlayer);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Link_Slash"));
		Safe_Release(pInstance);
	}

	return pInstance;
}
void CState_Link_Slash::Free()
{
	__super::Free();
}