#include "stdafx.h"
#include "../Public/State_Vegas_Walk_l.h"
#include "GameInstance.h"
#include "Model.h"
#include "Vegas.h"

CState_Vegas_Walk_l::CState_Vegas_Walk_l(CFsm* pFsm, CMonster* pOwner)
	:CState{ pFsm }
	, m_pOwner{ pOwner }
{
}

HRESULT CState_Vegas_Walk_l::Initialize(_uint iStateNum)
{
	m_iCurrentAnimIndex = m_pOwner->Get_Model()->Get_AnimationIndex("walk_r");
	m_iStateNum = iStateNum;

	return S_OK;
}

HRESULT CState_Vegas_Walk_l::Start_State()
{
	m_pOwner->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.1f, true);
	m_pOwner->Set_AnimationSpeed(m_iCurrentAnimIndex, 60.f);
	return S_OK;
}

void CState_Vegas_Walk_l::Update(_float fTimeDelta)
{
	m_fTimer += fTimeDelta;


	m_pOwner->Get_Transform()->Go_World_Left(fTimeDelta, m_fSpeed);

	if (m_fTimer > 1.3f)
	{
		m_fTimer = 0;
		m_pOwner->Change_State(CVegas::IDLE);
	}
}

void CState_Vegas_Walk_l::End_State()
{
}

CState_Vegas_Walk_l* CState_Vegas_Walk_l::Create(CFsm* pFsm, CMonster* pOwner, _uint iStateNum)
{
	CState_Vegas_Walk_l* pInstance = new CState_Vegas_Walk_l(pFsm, pOwner);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Vegas_Walk_l"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Vegas_Walk_l::Free()
{
	__super::Free();
}
