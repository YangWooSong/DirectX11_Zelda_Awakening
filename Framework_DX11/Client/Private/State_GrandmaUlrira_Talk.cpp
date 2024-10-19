#include "stdafx.h"
#include "State_GrandmaUlrira_Talk.h"
#include "GameInstance.h"
#include "GrandmaUlrira.h"
#include "UIObject.h"
#include "Link.h"
#include "DialogueUI.h"
#include "MainUI.h"

CState_GrandmaUlrira_Talk::CState_GrandmaUlrira_Talk(CFsm* pFsm, CNPC* pOwner)
	:CState{ pFsm }
	, m_pOwner{ pOwner }
{
}

HRESULT CState_GrandmaUlrira_Talk::Initialize(_uint iStateNum)
{
	m_iCurrentAnimIndex = m_pOwner->Get_Model()->Get_AnimationIndex("talk");
	m_iStateNum = iStateNum;

	m_pPlayer = static_cast<CLink*>(m_pGameInstance->Find_Player(LEVEL_FIELD));

	return S_OK;
}


HRESULT CState_GrandmaUlrira_Talk::Start_State()
{
	m_pOwner->Get_Model()->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.1f, true);
	m_pOwner->Get_Model()->Set_AnimationSpeed(m_iCurrentAnimIndex, 30.f);
	m_pOwner->Get_UIObject(CNPC::DIALOGUE_UI)->SetActive(true);

	static_cast<CLink*>(m_pPlayer)->Set_Talk(true);

	m_pDialogueUI = static_cast<CDialogueUI*>(m_pOwner->Get_UIObject(CNPC::DIALOGUE_UI));
	m_pDialogueUI->Set_OwnerType(CDialogueUI::GRANDMA);
	m_pDialogueUI->Set_LineNum(m_iLineNum);
	m_pOwner->Get_Sound()->Play_Sound(TEXT("6_Npc_Grandma_0.wav"), 1.f);
	return S_OK;
}

void CState_GrandmaUlrira_Talk::Update(_float fTimeDelta)
{
	//플레이어가 NPC 쳐다보게 함
	_vector newLook = XMVector3Normalize(m_pOwner->Get_Transform()->Get_State(CTransform::STATE_POSITION) - m_pPlayer->Get_Position());
	m_pPlayer->Get_Transform()->Turn_Lerp(newLook, 1.0f, fTimeDelta);

	if (m_pPlayer->Get_MonsterCount() >= 3 && m_iLineNum != 4)
	{
		m_iLineNum = 4;
		m_pDialogueUI->Set_LineNum(m_iLineNum);
	}

	if (KEY_TAP(KEY::ENTER))
	{
		m_pOwner->Get_Sound()->Play_Sound(TEXT("5_UI_Sys_Talk_Next.wav"), 1.f);

		if (m_iLineNum == 3)
		{
			m_pOwner->Change_State(CGrandmaUlrira::IDLE);
			return;
		}
		else if (m_iLineNum == 4)
		{
			m_pOwner->Change_State(CGrandmaUlrira::IDLE);
			
			m_pPlayer->Set_UI_Active(CLink::INTERACT_UI, false);
			m_pPlayer->Change_PlayerUI_TextureNum(CLink::ITEM_ICON_UI, 9);
			static_cast<CDialogueUI*>(static_cast<CMainUI*>(m_pGameInstance->Find_Object(LEVEL_FIELD, TEXT("Layer_MainUI"), 0))->Get_ChildUI(CMainUI::DIALOGUE))->Set_LineNum(CDialogueUI::DUNGEON_KEY);
			m_pPlayer->Change_State(CLink::GET_ITEM);
		}
		m_pDialogueUI->Set_LineNum(++m_iLineNum);
	}
}

void CState_GrandmaUlrira_Talk::End_State()
{
	m_pOwner->Set_Talk(false);
	m_pOwner->Get_UIObject(CNPC::DIALOGUE_UI)->SetActive(false);
	static_cast<CLink*>(m_pPlayer)->Set_Talk(false);
}


CState_GrandmaUlrira_Talk* CState_GrandmaUlrira_Talk::Create(CFsm* pFsm, CNPC* pOwner, _uint iStateNum)
{
	CState_GrandmaUlrira_Talk* pInstance = new CState_GrandmaUlrira_Talk(pFsm, pOwner);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_GrandmaUlrira_Talk"));
		Safe_Release(pInstance);
	}

	return pInstance;
}
void CState_GrandmaUlrira_Talk::Free()
{
	__super::Free();
}