#include "stdafx.h"
#include "State_Link_Damage_Front.h"
#include "GameInstance.h"
#include "Link.h"

CState_Link_Damage_Front::CState_Link_Damage_Front(CFsm* pFsm, CPlayer* pPlayer)
	:CState{ pFsm }
	, m_pPlayer{ pPlayer }
{
}

HRESULT CState_Link_Damage_Front::Initialize(_uint iStateNum)
{
	m_iCurrentAnimIndex = m_pPlayer->Get_Model()->Get_AnimationIndex("dmg_f");
	m_iStateNum = iStateNum;

	return S_OK;
}

HRESULT CState_Link_Damage_Front::Start_State()
{
	m_pPlayer->Get_Model()->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.02f);
	m_pPlayer->Get_Model()->Set_AnimationSpeed(m_iCurrentAnimIndex, 80.f);
	m_pPlayer->Get_PlayerSound()->Play_Sound(TEXT("1_Link_Sword_Damage_Front.wav"), 1.f);
	m_pPlayer->Set_bBodyRed(true);
	m_pPlayer->Get_Collider()->Set_IsActive(false);
	return S_OK;
}

void CState_Link_Damage_Front::Update(_float fTimeDelta)
{
	if (m_pPlayer->Get_IsEnd_CurrentAnimation())
	{
		m_pPlayer->Change_State(CLink::IDLE);
	}
}

void CState_Link_Damage_Front::End_State()
{
	m_pPlayer->Set_bBodyRed(false);
	m_pPlayer->Set_bBlink(true);
}

CState_Link_Damage_Front* CState_Link_Damage_Front::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum)
{
	CState_Link_Damage_Front* pInstance = new CState_Link_Damage_Front(pFsm, pPlayer);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Link_Damage_Front"));
		Safe_Release(pInstance);
	}

	return pInstance;
}
void CState_Link_Damage_Front::Free()
{
	__super::Free();
}