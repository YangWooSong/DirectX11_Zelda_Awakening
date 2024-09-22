#include "stdafx.h"
#include "State_Link_Key.h"
#include "GameInstance.h"
#include "Link.h"
#include "PartObject.h"

CState_Link_Key::CState_Link_Key(CFsm* pFsm, CPlayer* pPlayer)
	:CState{ pFsm }
	, m_pPlayer{ pPlayer }
{
}

HRESULT CState_Link_Key::Initialize(_uint iStateNum)
{
	m_iCurrentAnimIndex = m_pPlayer->Get_Model()->Get_AnimationIndex("key_open");
	m_iStateNum = iStateNum;

	return S_OK;
}


HRESULT CState_Link_Key::Start_State()
{
	m_pPlayer->Get_Model()->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.2f);
	m_pPlayer->Get_Model()->Set_AnimationSpeed(m_iCurrentAnimIndex, 80.f);

	m_pPlayer->Get_PlayerSound()->Play_Sound(TEXT("4_Obj_Key_Sound.wav"), 1.f);
	static_cast<CLink*>(m_pPlayer)->Minus_SmallKeyCount();

	static_cast<CLink*>(m_pPlayer)->Set_ActiveSheild(false);
	static_cast<CLink*>(m_pPlayer)->Set_ActiveSword(false);

	return S_OK;
}

void CState_Link_Key::Update(_float fTimeDelta)
{

	if (m_pPlayer->Get_IsEnd_CurrentAnimation())
	{
		m_pPlayer->Change_State(CLink::IDLE);
	}
}

void CState_Link_Key::End_State()
{
	static_cast<CLink*>(m_pPlayer)->Set_ActiveSheild(true);
	static_cast<CLink*>(m_pPlayer)->Set_ActiveSword(true);
}

CState_Link_Key* CState_Link_Key::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum)
{
	CState_Link_Key* pInstance = new CState_Link_Key(pFsm, pPlayer);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Link_Key"));
		Safe_Release(pInstance);
	}

	return pInstance;
}
void CState_Link_Key::Free()
{
	__super::Free();
}