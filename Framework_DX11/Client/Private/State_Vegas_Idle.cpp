#include "stdafx.h"
#include "State_Vegas_Idle.h"
#include "GameInstance.h"
#include "Model.h"
#include "Vegas.h"

CState_Vegas_Idle::CState_Vegas_Idle(CFsm* pFsm, CMonster* pOwner)
	:CState{ pFsm }
	, m_pOwner{ pOwner }
{
}

HRESULT CState_Vegas_Idle::Initialize(_uint iStateNum)
{
	m_iCurrentAnimIndex = m_pOwner->Get_Model()->Get_AnimationIndex("wait");
	m_iStateNum = iStateNum;

	return S_OK;
}

HRESULT CState_Vegas_Idle::Start_State()
{
	m_pOwner->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.1f, true);
	//m_pOwner->Set_AnimationSpeed(m_iCurrentAnimIndex, 60.f);
	return S_OK;
}

void CState_Vegas_Idle::Update(_float fTimeDelta)
{
}

void CState_Vegas_Idle::End_State()
{
}

CState_Vegas_Idle* CState_Vegas_Idle::Create(CFsm* pFsm, CMonster* pOwner, _uint iStateNum)
{
	CState_Vegas_Idle* pInstance = new CState_Vegas_Idle(pFsm, pOwner);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Vegas_Idle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Vegas_Idle::Free()
{
	__super::Free();
}
