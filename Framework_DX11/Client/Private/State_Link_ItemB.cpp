#include "stdafx.h"
#include "State_Link_ItemB.h"
#include "GameInstance.h"
#include "Link.h"

CState_Link_ItemB::CState_Link_ItemB(CFsm* pFsm, CPlayer* pPlayer)
	:CState{ pFsm }
	, m_pPlayer{ pPlayer }
{
}

HRESULT CState_Link_ItemB::Initialize(_uint iStateNum)
{
	 m_iCurrentAnimIndex = m_pPlayer->Get_Model()->Get_AnimationIndex("idle_bare");
	 m_iStateNum = iStateNum;

	return S_OK;
}

HRESULT CState_Link_ItemB::Start_State()
{
	// m_pPlayer->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.2f);
	m_pPlayer_ItemB = static_cast<CLink*>(m_pPlayer)->Get_ItemB();
	return S_OK;
}

void CState_Link_ItemB::Update(_float fTimeDelta)
{
	if (KEY_AWAY(KEY::P))
	{
		m_pPlayer->Change_State(CLink::SLASH);
		return;
	}

	if (KEY_HOLD(KEY::P))
	{
		m_pPlayer->Change_State(CLink::SLASH_HOLD);
		return;
	}
	if (m_pPlayer_ItemB == nullptr)
		m_pPlayer->Change_State(CLink::SLASH);

}

void CState_Link_ItemB::End_State()
{
}

CState_Link_ItemB* CState_Link_ItemB::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum)
{
	CState_Link_ItemB* pInstance = new CState_Link_ItemB(pFsm, pPlayer);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Link_ItemB"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Link_ItemB::Free()
{
	__super::Free();
}
