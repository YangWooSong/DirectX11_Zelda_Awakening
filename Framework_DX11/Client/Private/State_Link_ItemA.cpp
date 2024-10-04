#include "stdafx.h"
#include "State_Link_ItemA.h"
#include "GameInstance.h"
#include "Link.h"

CState_Link_ItemA::CState_Link_ItemA(CFsm* pFsm, CPlayer* pPlayer)
    :CState{ pFsm }
    , m_pPlayer{ pPlayer }
{
}

HRESULT CState_Link_ItemA::Initialize(_uint iStateNum)
{
   // m_iCurrentAnimIndex = m_pPlayer->Get_Model()->Get_AnimationIndex("idle_bare");
    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_Link_ItemA::Start_State()
{
   // m_pPlayer->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.2f);
	m_pPlayer_ItemA = static_cast<CLink*>(m_pPlayer)->Get_ItemA();
    return S_OK;
}

void CState_Link_ItemA::Update(_float fTimeDelta)
{
	if(m_pPlayer_ItemA == nullptr)
	{
		m_pPlayer->Change_State(CLink::JUMP);
	}

}

void CState_Link_ItemA::End_State()
{
}

CState_Link_ItemA* CState_Link_ItemA::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum)
{
	CState_Link_ItemA* pInstance = new CState_Link_ItemA(pFsm, pPlayer);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Link_ItemA"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Link_ItemA::Free()
{
	__super::Free();
}
