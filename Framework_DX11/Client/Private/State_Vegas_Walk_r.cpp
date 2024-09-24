#include "stdafx.h"
#include "../Public/State_Vegas_Walk_r.h"
#include "GameInstance.h"
#include "Model.h"
#include "Vegas.h"

CState_Vegas_Walk_r::CState_Vegas_Walk_r(CFsm* pFsm, CMonster* pOwner)
	:CState{ pFsm }
	, m_pOwner{ pOwner }
{
}

HRESULT CState_Vegas_Walk_r::Initialize(_uint iStateNum)
{
	m_iCurrentAnimIndex = m_pOwner->Get_Model()->Get_AnimationIndex("walk_l");
	m_iStateNum = iStateNum;

	return S_OK;
}

HRESULT CState_Vegas_Walk_r::Start_State()
{
	m_pOwner->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.1f, true);
	m_pOwner->Set_AnimationSpeed(m_iCurrentAnimIndex, 60.f);
	return S_OK;
}

void CState_Vegas_Walk_r::Update(_float fTimeDelta)
{
	m_fTimer += fTimeDelta;


	m_pOwner->Get_Transform()->Go_World_Right(fTimeDelta, m_fSpeed, m_pOwner->Get_NavigationCom());

	if (m_fTimer > 1.5f)
	{
		m_fTimer = 0;
		m_pOwner->Change_State(CVegas::IDLE);
	}
}

void CState_Vegas_Walk_r::End_State()
{
}

CState_Vegas_Walk_r* CState_Vegas_Walk_r::Create(CFsm* pFsm, CMonster* pOwner, _uint iStateNum)
{
	CState_Vegas_Walk_r* pInstance = new CState_Vegas_Walk_r(pFsm, pOwner);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Vegas_Walk_r"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Vegas_Walk_r::Free()
{
	__super::Free();
}
