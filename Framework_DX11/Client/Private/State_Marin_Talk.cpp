#include "stdafx.h"
#include "State_Marin_Talk.h"
#include "GameInstance.h"
#include "Marin.h"
#include "UIObject.h"
#include "Link.h"
#include "DialogueUI.h"
#include "Store_Item.h"
#include "ChoiceBtn.h"

CState_Marin_Talk::CState_Marin_Talk(CFsm* pFsm, CNPC* pOwner)
	:CState{ pFsm }
	, m_pOwner{ pOwner }
{
}

HRESULT CState_Marin_Talk::Initialize(_uint iStateNum)
{
	m_iCurrentAnimIndex = m_pOwner->Get_Model()->Get_AnimationIndex("talk");
	m_iStateNum = iStateNum;

	m_pPlayer = static_cast<CLink*>(m_pGameInstance->Find_Player(LEVEL_STORE));

	return S_OK;
}


HRESULT CState_Marin_Talk::Start_State()
{
	m_pOwner->Get_Model()->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.1f, true);
	m_pOwner->Get_Model()->Set_AnimationSpeed(m_iCurrentAnimIndex, 30.f);
	m_pOwner->Get_UIObject(CNPC::DIALOGUE_UI)->SetActive(true);

	static_cast<CLink*>(m_pGameInstance->Find_Player(LEVEL_STORE))->Set_Talk(true);

	m_pCarryObj = m_pPlayer->Get_CarryItem();
	m_pDialogueUI = static_cast<CDialogueUI*>(m_pOwner->Get_UIObject(CNPC::DIALOGUE_UI));
	Set_UI_LineNum();

	return S_OK;
}

void CState_Marin_Talk::Update(_float fTimeDelta)
{
	//플레이어가 NPC 쳐다보게 함
	_vector newLook = XMVector3Normalize(m_pOwner->Get_Transform()->Get_State(CTransform::STATE_POSITION) - m_pPlayer->Get_Position());
	m_pPlayer->Get_Transform()->Turn_Lerp(newLook, 1.0f, fTimeDelta);

	//상품이 없으면 알아서 창 꺼짐
	if (m_pCarryObj == nullptr)
		m_fTimer += fTimeDelta;

	if (m_fTimer > 2.f)
		m_pOwner->Change_State(CMarin::IDLE);

	//선택지 변경
	if (KEY_TAP(KEY::W))
		m_iSelectChoiceBtn++;
	else if (KEY_TAP(KEY::S))
		m_iSelectChoiceBtn--;

	Set_SelectedChoiceBtn();

	//선택지 선택, 대화 종료
	if (KEY_TAP(KEY::E))
	{
		Buy_Item();
		m_pOwner->Change_State(CMarin::IDLE);
	}


}

void CState_Marin_Talk::End_State()
{
	m_iSelectChoiceBtn = 0;
	m_fTimer = 0.f;
	m_pOwner->Set_Talk(false);
	m_pOwner->Get_UIObject(CNPC::DIALOGUE_UI)->SetActive(false);
	if (m_pCarryObj == nullptr)
		static_cast<CLink*>(m_pPlayer)->Set_Talk(false);
}

void CState_Marin_Talk::Set_SelectedChoiceBtn()
{
	if (m_iSelectChoiceBtn % 2 == 0)
	{
		m_pDialogueUI->Get_ChildUI(CChoiceBtn::BUY)->Set_Selected(true);
		m_pDialogueUI->Get_ChildUI(CChoiceBtn::NOTBUY)->Set_Selected(false);
	}
	else
	{
		m_pDialogueUI->Get_ChildUI(CChoiceBtn::BUY)->Set_Selected(false);
		m_pDialogueUI->Get_ChildUI(CChoiceBtn::NOTBUY)->Set_Selected(true);
	}
}

void CState_Marin_Talk::Buy_Item()
{
	if (m_iSelectChoiceBtn % 2 == 0)	//산다
	{
		if (static_cast<CStore_Item*>(m_pCarryObj)->Get_ItemType() == CStore_Item::BOMB)
		{
			if (static_cast<CLink*>(m_pPlayer)->Get_Lupee() >= 5)
			{
				m_pCarryObj->SetActive(false);
				static_cast<CStore_Item*>(m_pCarryObj)->Set_Picked(false);
				static_cast<CLink*>(m_pPlayer)->Set_Bomb(true);
				static_cast<CLink*>(m_pPlayer)->Add_Lupee(-5);
			}
			else
				static_cast<CStore_Item*>(m_pCarryObj)->Go_Back_OriginPos();
		}
		else if (static_cast<CStore_Item*>(m_pCarryObj)->Get_ItemType() == CStore_Item::HEART)
		{
			if (static_cast<CLink*>(m_pPlayer)->Get_Lupee() >= 10)
			{
				m_pCarryObj->SetActive(false);
				static_cast<CStore_Item*>(m_pCarryObj)->Set_Picked(false);
				static_cast<CLink*>(m_pPlayer)->Add_MaxHp();
				static_cast<CLink*>(m_pPlayer)->Add_Lupee(-10);
			}
			else
				static_cast<CStore_Item*>(m_pCarryObj)->Go_Back_OriginPos();
		}
	}
	else //안산다
	{
		static_cast<CStore_Item*>(m_pCarryObj)->Go_Back_OriginPos();
	}
}

void CState_Marin_Talk::Set_UI_LineNum()
{
	if (m_pCarryObj == nullptr)
		m_pDialogueUI->Set_LineNum(0);
	else if (static_cast<CStore_Item*>(m_pCarryObj)->Get_ItemType() == CStore_Item::BOMB)
		m_pDialogueUI->Set_LineNum(1);
	else if (static_cast<CStore_Item*>(m_pCarryObj)->Get_ItemType() == CStore_Item::HEART)
		m_pDialogueUI->Set_LineNum(2);
}

CState_Marin_Talk* CState_Marin_Talk::Create(CFsm* pFsm, CNPC* pOwner, _uint iStateNum)
{
	CState_Marin_Talk* pInstance = new CState_Marin_Talk(pFsm, pOwner);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Marin_Talk"));
		Safe_Release(pInstance);
	}

	return pInstance;
}
void CState_Marin_Talk::Free()
{
	__super::Free();
}