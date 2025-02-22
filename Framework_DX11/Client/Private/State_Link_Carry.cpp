#include "stdafx.h"
#include "State_Link_Carry.h"
#include "GameInstance.h"
#include "Link.h"
#include "Store_Item.h"
CState_Link_Carry::CState_Link_Carry(CFsm* pFsm, CPlayer* pPlayer)
	:CState{ pFsm }
	, m_pPlayer{ pPlayer }
{
}

HRESULT CState_Link_Carry::Initialize(_uint iStateNum)
{
	m_iCarryStartAnimIndex = m_pPlayer->Get_Model()->Get_AnimationIndex("carry");
	m_iCarryIdleAnimIndex = m_pPlayer->Get_Model()->Get_AnimationIndex("idle_carry");
	m_iStateNum = iStateNum;

	return S_OK;
}

HRESULT CState_Link_Carry::Start_State()
{
	if(m_pPlayer->Get_Fsm()->Get_PrevState() == CLink::CARRY_WALK)
		m_iCurrentAnimIndex = m_iCarryIdleAnimIndex;
	else
		m_iCurrentAnimIndex = m_iCarryStartAnimIndex;

	m_pPlayer->Get_Model()->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.1f, 0);

	if(m_iCurrentAnimIndex == m_iCarryStartAnimIndex)
	{
		m_pPlayer->Get_PlayerSound()->Play_Sound(TEXT("1_Link_Carry.wav"), 1.f);
		m_pPlayer->Get_Model()->Set_AnimationSpeed(m_iCurrentAnimIndex, 50.f);
	}

	static_cast<CLink*>(m_pPlayer)->Set_ActiveSheild(false);
	static_cast<CLink*>(m_pPlayer)->Set_ActiveSword(false);


	return S_OK;
}

void CState_Link_Carry::Update(_float fTimeDelta)
{
	if (m_iCurrentAnimIndex == m_iCarryStartAnimIndex && m_pPlayer->Get_IsEnd_CurrentAnimation())
	{
		m_iCurrentAnimIndex = m_iCarryIdleAnimIndex;
		m_pPlayer->Get_Model()->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.02f, true);
		m_pPlayer->Get_Model()->Set_AnimationSpeed(m_iCurrentAnimIndex, 60.f);
	}

	//방향키가 눌린게 없어야 Idle로 전환 (W,A,S,D 순서)
	if ((GetAsyncKeyState(0x57) & 0x8000) || (GetAsyncKeyState(0x41) & 0x8000) || (GetAsyncKeyState(0x53) & 0x8000) || (GetAsyncKeyState(0x44) & 0x8000))
	{
		if(static_cast<CLink*>(m_pPlayer)->Get_isTalk() == false )
			m_pPlayer->Change_State(CLink::CARRY_WALK);
	}

	if (KEY_TAP(KEY::E))
	{
		if(static_cast<CLink*>(m_pPlayer)->Get_CarryItem()->Get_LayerTag() == TEXT("Layer_Store_Item") &&  static_cast<CLink*>(m_pPlayer)->Get_isTalk())
		{
			CGameObject* pObject = static_cast<CLink*>(m_pPlayer)->Get_CarryItem();
			//pObject->SetActive(false);
			//static_cast<CStore_Item*>(pObject)->Set_Picked(false);
			static_cast<CLink*>(m_pPlayer)->Set_CarryItem(nullptr);
			static_cast<CLink*>(m_pPlayer)->Set_Talk(false);
			static_cast<CLink*>(m_pPlayer)->Set_ActiveSheild(true);
			static_cast<CLink*>(m_pPlayer)->Set_ActiveSword(true);
			m_pPlayer->Change_State(CLink::IDLE);
			return;
		}

		if(static_cast<CLink*>(m_pPlayer)->Get_CarryItem()->Get_LayerTag() != TEXT("Layer_Store_Item"))
			m_pPlayer->Change_State(CLink::THROW);
	}

}

void CState_Link_Carry::End_State()
{
	
}

CState_Link_Carry* CState_Link_Carry::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum)
{
	CState_Link_Carry* pInstance = new CState_Link_Carry(pFsm, pPlayer);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Link_Carry"));
		Safe_Release(pInstance);
	}

	return pInstance;
}
void CState_Link_Carry::Free()
{
	__super::Free();
}