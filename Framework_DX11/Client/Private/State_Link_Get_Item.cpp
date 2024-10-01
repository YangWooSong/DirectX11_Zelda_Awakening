#include "stdafx.h"
#include "State_Link_Get_Item.h"
#include "GameInstance.h"
#include "Link.h"
#include "PlayerCamera.h"

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
	m_pCamera = static_cast<CPlayerCamera*>(m_pGameInstance->Find_Camera(m_pPlayer->Get_LevelIndex()));

	m_iCurrentAnimIndex = m_iAnimStIndex;
	m_pPlayer->Get_Model()->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.02f);
	//m_pPlayer->Get_Model()->Set_AnimationSpeed(m_iCurrentAnimIndex, 80.f);

	m_pCamera->Set_FollowPlayer(true);

	if(m_pPlayer->Get_LevelIndex() == LEVEL_DUNGEON)
		m_pCamera->Zoom_In(1.5f, 60.f);
	if (m_pPlayer->Get_LevelIndex() == LEVEL_FIELD)
		m_pCamera->Zoom_In(1.5f, 45.f);

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
	
		_float3 vPlayerLook;
		XMStoreFloat3(&vPlayerLook, m_pPlayer->Get_Transform()->Get_State(CTransform::STATE_LOOK));

		if (m_pPlayer->Get_Model()->Get_CurrentTrackPosition() > 21.f)
			m_pPlayer->Set_UI_Active(CLink::ITEM_ICON_UI, true);
	}

	if (m_iCurrentAnimIndex == m_iAnimLpIndex)
	{
		m_fLoopTimer += fTimeDelta;

	}

	if (m_fLoopTimer > 2.f && m_iCurrentAnimIndex != m_iAnimEdIndex)
	{
		m_iCurrentAnimIndex = m_iAnimEdIndex;
		m_pPlayer->Set_UI_Active(CLink::ITEM_ICON_UI, false);
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
	if(m_pPlayer->Get_LevelIndex() == LEVEL_DUNGEON)
	{
		if (m_pPlayer->Get_Navigation()->Get_CurrentCell_RoomNum() != 7 && m_pPlayer->Get_Navigation()->Get_CurrentCell_RoomNum() != 14 && m_pPlayer->Get_Navigation()->Get_CurrentCell_RoomNum() != 16)
			m_pCamera->Set_FollowPlayer(false);
		m_pCamera->Zoom_Out(1.5f, 60.f);
	}
	else
		m_pCamera->Zoom_Out(1.5f, 45.f);

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