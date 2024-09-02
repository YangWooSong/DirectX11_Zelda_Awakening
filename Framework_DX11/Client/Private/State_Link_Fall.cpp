#include "stdafx.h"
#include "State_Link_Fall.h"
#include "GameInstance.h"
#include "Link.h"

CState_Link_Fall::CState_Link_Fall(CFsm* pFsm, CPlayer* pPlayer)
	:CState{ pFsm }
	, m_pPlayer{ pPlayer }
{
}

HRESULT CState_Link_Fall::Initialize(_uint iStateNum)
{
	m_iCurrentAnimIndex = m_pPlayer->Get_Model()->Get_AnimationIndex("d_fall");
	m_iStateNum = iStateNum;

	return S_OK;
}

HRESULT CState_Link_Fall::Start_State()
{
	m_pPlayer->Get_Model()->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.02f);
	//m_pPlayer->Get_Model()->Set_AnimationSpeed(m_iCurrentAnimIndex, 60.f);
	m_pPlayer->Set_Fall(true);
	return S_OK;
}

void CState_Link_Fall::Update(_float fTimeDelta)
{
	
	if (m_pPlayer->Get_IsEnd_CurrentAnimation())
	{
		m_fTimer += fTimeDelta;
		if(m_bPlaySound == false)
		{
			m_pGameInstance->Play_Sound(TEXT("1_Link_Fall.wav"), SOUND_PLAYER, 0.8f);
			m_bPlaySound = true;
		}
		m_pPlayer->Get_Transform()->Go_World_Down(fTimeDelta, 5.f);
	}

	if (m_fTimer > 1.5f)
	{
		m_pPlayer->Set_Fall(false);
		m_pPlayer->Get_Transform()->ChangePosToPreCellMiddle(m_pPlayer->Get_Navigation());
		m_pPlayer->Change_State(CLink::IDLE);
	}
}

void CState_Link_Fall::End_State()
{
	m_bPlaySound = false;
	m_fTimer = 0.f;
}

CState_Link_Fall* CState_Link_Fall::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum)
{
	CState_Link_Fall* pInstance = new CState_Link_Fall(pFsm, pPlayer);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Link_Fall"));
		Safe_Release(pInstance);
	}

	return pInstance;
}
void CState_Link_Fall::Free()
{
	__super::Free();
}