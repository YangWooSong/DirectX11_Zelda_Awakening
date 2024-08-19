#include "stdafx.h"
#include "State_Link_Stair_Down.h"
#include "GameInstance.h"
#include "Model.h"
#include "Link.h"

CState_Link_Stair_Down::CState_Link_Stair_Down(CFsm* pFsm, CPlayer* pPlayer)
    :CState{ pFsm }
    , m_pPlayer{ pPlayer }
{
}

HRESULT CState_Link_Stair_Down::Initialize(_uint iStateNum)
{
	m_iCurrentAnimIndex = m_pPlayer->Get_Model()->Get_AnimationIndex("ev_stairs_down");
    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_Link_Stair_Down::Start_State()
{
    m_pPlayer->Get_Model()->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.1f);
	m_pPlayer->Get_Model()->Set_AnimationSpeed(m_iCurrentAnimIndex, 60.f);
    return S_OK;
}

void CState_Link_Stair_Down::Update(_float fTimeDelta)
{
	if (m_pPlayer->Get_IsEnd_CurrentAnimation())
		m_pPlayer->Change_State(CLink::IDLE);
	else
	{
		m_pPlayer->Go_Stair_Down(fTimeDelta, 0.01f);
	}
}

void CState_Link_Stair_Down::End_State()
{
}

CState_Link_Stair_Down* CState_Link_Stair_Down::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum)
{
	CState_Link_Stair_Down* pInstance = new CState_Link_Stair_Down(pFsm, pPlayer);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Link_Stair_Down"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CState_Link_Stair_Down::Free()
{
	__super::Free();
}
