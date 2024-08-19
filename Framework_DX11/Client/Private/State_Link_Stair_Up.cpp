#include "stdafx.h"
#include "State_Link_Stair_Up.h"
#include "GameInstance.h"
#include "Model.h"
#include "Link.h"

CState_Link_Stair_Up::CState_Link_Stair_Up(CFsm* pFsm, CPlayer* pPlayer)
    :CState{ pFsm }
    , m_pPlayer{ pPlayer }
{
}

HRESULT CState_Link_Stair_Up::Initialize(_uint iStateNum)
{
	m_iCurrentAnimIndex = m_pPlayer->Get_Model()->Get_AnimationIndex("ev_stairs_up");
    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_Link_Stair_Up::Start_State()
{
    m_pPlayer->Get_Model()->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.1f);
	m_pPlayer->Get_Model()->Set_AnimationSpeed(m_iCurrentAnimIndex, 60.f);
    return S_OK;
}

void CState_Link_Stair_Up::Update(_float fTimeDelta)
{
	if (m_pPlayer->Get_IsEnd_CurrentAnimation())
		m_pPlayer->Change_State(CLink::IDLE);
	else
	{
		m_pPlayer->Go_Stair_Up(fTimeDelta, 0.01f);
	}
}

void CState_Link_Stair_Up::End_State()
{
}

CState_Link_Stair_Up* CState_Link_Stair_Up::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum)
{
	CState_Link_Stair_Up* pInstance = new CState_Link_Stair_Up(pFsm, pPlayer);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Link_Stair_Up"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CState_Link_Stair_Up::Free()
{
	__super::Free();
}
