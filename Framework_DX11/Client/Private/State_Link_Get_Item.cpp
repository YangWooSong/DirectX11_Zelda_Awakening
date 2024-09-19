#include "stdafx.h"
#include "State_Link_Get_Item.h"
#include "GameInstance.h"
#include "Link.h"

CState_Link_Get_Item::CState_Link_Get_Item(CFsm* pFsm, CPlayer* pPlayer)
	:CState{ pFsm }
	, m_pPlayer{ pPlayer }
{
}

HRESULT CState_Link_Get_Item::Initialize(_uint iStateNum)
{
	m_iAnimStIndex = m_pPlayer->Get_Model()->Get_AnimationIndex("item_get_st");
	m_iAnimLpIndex = m_pPlayer->Get_Model()->Get_AnimationIndex("item_get_lp");
	m_iAnimEdIndex = m_pPlayer->Get_Model()->Get_AnimationIndex("item_get_ed");

	m_iStateNum = iStateNum;

	return S_OK;
}

HRESULT CState_Link_Get_Item::Start_State()
{
	m_iCurrentAnimIndex = m_iAnimStIndex;
	m_pPlayer->Get_Model()->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.02f);
	//m_pPlayer->Get_Model()->Set_AnimationSpeed(m_iCurrentAnimIndex, 80.f);

	static_cast<CLink*>(m_pPlayer)->Set_ActiveSheild(false);
	static_cast<CLink*>(m_pPlayer)->Set_ActiveSword(false);
	return S_OK;
}

void CState_Link_Get_Item::Update(_float fTimeDelta)
{

	if (m_iCurrentAnimIndex == m_iAnimStIndex && m_pPlayer->Get_IsEnd_CurrentAnimation())
	{
		m_iCurrentAnimIndex = m_iAnimLpIndex;
		m_pPlayer->Get_Model()->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.02f, true);
		//m_pPlayer->Get_Model()->Set_AnimationSpeed(m_iCurrentAnimIndex, 80.f);
	}
	else if(m_iCurrentAnimIndex == m_iAnimStIndex)
	{
		_vector vNewLook = { 0.f,0.f,-1.f };
		m_pPlayer->Get_Transform()->Turn_Lerp(vNewLook, 5.f, fTimeDelta);
	}

	if (m_iCurrentAnimIndex == m_iAnimLpIndex)
		m_fLoopTimer += fTimeDelta;

	if (m_fLoopTimer > 2.f && m_iCurrentAnimIndex != m_iAnimEdIndex)
	{
		m_iCurrentAnimIndex = m_iAnimEdIndex;
		m_pPlayer->Get_Model()->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.02f);
		m_pPlayer->Get_Model()->Set_AnimationSpeed(m_iCurrentAnimIndex, 70.f);
	}

	if (m_iCurrentAnimIndex == m_iAnimEdIndex && m_pPlayer->Get_IsEnd_CurrentAnimation())
	{
		m_pPlayer->Change_State(CLink::IDLE);
	}
}

void CState_Link_Get_Item::End_State()
{
	static_cast<CLink*>(m_pPlayer)->Set_ActiveSheild(true);
	static_cast<CLink*>(m_pPlayer)->Set_ActiveSword(true);
	m_fLoopTimer = 0.f;
}

CState_Link_Get_Item* CState_Link_Get_Item::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum)
{
	CState_Link_Get_Item* pInstance = new CState_Link_Get_Item(pFsm, pPlayer);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Link_Get_Item"));
		Safe_Release(pInstance);
	}

	return pInstance;
}
void CState_Link_Get_Item::Free()
{
	__super::Free();
}