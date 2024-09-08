#include "stdafx.h"
#include "State_Link_Idle.h"
#include "GameInstance.h"
#include "Model.h"
#include "Link.h"

CState_Link_Idle::CState_Link_Idle(CFsm* pFsm, CPlayer* pPlayer)
    :CState{ pFsm }
    , m_pPlayer{ pPlayer }
{
}

HRESULT CState_Link_Idle::Initialize(_uint iStateNum)
{
	m_iCurrentAnimIndex = m_pPlayer->Get_Model()->Get_AnimationIndex("idle_bare");
    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_Link_Idle::Start_State()
{
    m_pPlayer->Get_Model()->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.1f, true);
    return S_OK;
}

void CState_Link_Idle::Update(_float fTimeDelta)
{
	if ((GetAsyncKeyState(0x57) & 0x8000) || (GetAsyncKeyState(0x41) & 0x8000) || (GetAsyncKeyState(0x53) & 0x8000) || (GetAsyncKeyState(0x44) & 0x8000))
	{
		m_pPlayer->Change_State(CLink::WALK);
		return;
	}

	if (KEY_HOLD(KEY::O))
	{
		m_pPlayer->Change_State(CLink::ITEMA);
	}

	if (KEY_HOLD(KEY::Q))
	{
	//	m_pPlayer->Change_State(CLink::DAMAGE_FRONT);
	}

	if (KEY_AWAY(KEY::P))
	{
		m_pPlayer->Change_State(CLink::ITEMB);
	}
	else if (KEY_HOLD(KEY::P))
	{
		m_fPressTime += fTimeDelta;
		if(m_fPressTime > 0.2f)
			m_pPlayer->Change_State(CLink::SLASH_HOLD);
	}

	if (KEY_HOLD(KEY::LSHIFT))
	{
		m_pPlayer->Change_State(CLink::SHIELD);
	}

	

}

void CState_Link_Idle::End_State()
{
	m_fPressTime = 0.f;
}

CState_Link_Idle* CState_Link_Idle::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum)
{
	CState_Link_Idle* pInstance = new CState_Link_Idle(pFsm, pPlayer);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Link_Idle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CState_Link_Idle::Free()
{
	__super::Free();
}
