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

	m_pPlayer = static_cast<CLink*>(m_pGameInstance->Find_Player(LEVEL_MARINHOUSE));

	return S_OK;
}


HRESULT CState_Marin_Talk::Start_State()
{
	m_pOwner->Get_Model()->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.1f, true);
	m_pOwner->Get_Model()->Set_AnimationSpeed(m_iCurrentAnimIndex, 30.f);
	m_pOwner->Get_UIObject(CNPC::DIALOGUE_UI)->SetActive(true);

	static_cast<CLink*>(m_pPlayer)->Set_Talk(true);

	m_pDialogueUI = static_cast<CDialogueUI*>(m_pOwner->Get_UIObject(CNPC::DIALOGUE_UI));
	m_pDialogueUI->Set_OwnerType(CDialogueUI::MARIN);
	m_pDialogueUI->Set_LineNum(m_iLineNum);
	m_pOwner->Get_Sound()->Play_Sound(TEXT("6_Npc_Marin_Happy.wav"), 1.f);
	return S_OK;
}

void CState_Marin_Talk::Update(_float fTimeDelta)
{
	//플레이어가 NPC 쳐다보게 함
	_vector newLook = XMVector3Normalize(m_pOwner->Get_Transform()->Get_State(CTransform::STATE_POSITION) - m_pPlayer->Get_Position());
	m_pPlayer->Get_Transform()->Turn_Lerp(newLook, 1.0f, fTimeDelta);

	m_fTimer += fTimeDelta;
	if (m_fTimer > 2.f)
	{
		m_fTimer = 0.f;

		if (m_iLineNum == 3)
		{
			m_pOwner->Change_State(CMarin::IDLE);
			return;
		}
		m_pDialogueUI->Set_LineNum(++m_iLineNum);
		m_pOwner->Get_Sound()->Play_Sound(TEXT("5_UI_Sys_Talk_Next.wav"), 1.f);
	}
}

void CState_Marin_Talk::End_State()
{
	m_fTimer = 0.f;
	m_pOwner->Set_Talk(false);
	m_pOwner->Get_UIObject(CNPC::DIALOGUE_UI)->SetActive(false);
	static_cast<CLink*>(m_pPlayer)->Set_Talk(false);
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