#include "stdafx.h"
#include "State_Link_Throw.h"
#include "GameInstance.h"
#include "Link.h"
#include "HousePot.h"

CState_Link_Throw::CState_Link_Throw(CFsm* pFsm, CPlayer* pPlayer)
	:CState{ pFsm }
	, m_pPlayer{ pPlayer }
{
}

HRESULT CState_Link_Throw::Initialize(_uint iStateNum)
{
	m_iCurrentAnimIndex = m_pPlayer->Get_Model()->Get_AnimationIndex("throw");
	m_iStateNum = iStateNum;

	return S_OK;
}

HRESULT CState_Link_Throw::Start_State()
{
	m_pPlayer->Get_Model()->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.02f);
	m_pPlayer->Get_Model()->Set_AnimationSpeed(m_iCurrentAnimIndex, 50.f);
	return S_OK;
}

void CState_Link_Throw::Update(_float fTimeDelta)
{
	m_fTimer += fTimeDelta;

	if (m_fTimer > 0.28f)
	{
		static_cast<CHousePot*>(static_cast<CLink*>(m_pPlayer)->Get_CarryItem())->Set_Break(true);
	}

	if (m_pPlayer->Get_IsEnd_CurrentAnimation())
	{
		m_pPlayer->Change_State(CLink::IDLE);
	}
}

void CState_Link_Throw::End_State()
{
	static_cast<CLink*>(m_pPlayer)->Set_ActiveSheild(true);
	static_cast<CLink*>(m_pPlayer)->Set_ActiveSword(true);
	static_cast<CLink*>(m_pPlayer)->Set_CarryItem(nullptr);
	m_fTimer = 0.f;
}

CState_Link_Throw* CState_Link_Throw::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum)
{
	CState_Link_Throw* pInstance = new CState_Link_Throw(pFsm, pPlayer);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Link_Throw"));
		Safe_Release(pInstance);
	}

	return pInstance;
}
void CState_Link_Throw::Free()
{
	__super::Free();
}