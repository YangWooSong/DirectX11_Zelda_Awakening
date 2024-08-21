#include "stdafx.h"
#include "State_Vegas_Dead.h"
#include "GameInstance.h"
#include "Model.h"
#include "Vegas.h"

CState_Vegas_Dead::CState_Vegas_Dead(CFsm* pFsm, CMonster* pOwner)
	:CState{ pFsm }
	, m_pOwner{ pOwner }
{
}

HRESULT CState_Vegas_Dead::Initialize(_uint iStateNum)
{
	m_iCurrentAnimIndex = m_pOwner->Get_Model()->Get_AnimationIndex("dead");
	m_iStateNum = iStateNum;

	return S_OK;
}

HRESULT CState_Vegas_Dead::Start_State()
{
	m_pOwner->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.1f, true);
	//m_pOwner->Set_AnimationSpeed(m_iCurrentAnimIndex, 60.f);
	return S_OK;
}

void CState_Vegas_Dead::Update(_float fTimeDelta)
{
}

void CState_Vegas_Dead::End_State()
{
}

CState_Vegas_Dead* CState_Vegas_Dead::Create(CFsm* pFsm, CMonster* pOwner, _uint iStateNum)
{
	CState_Vegas_Dead* pInstance = new CState_Vegas_Dead(pFsm, pOwner);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Vegas_Dead"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Vegas_Dead::Free()
{
	__super::Free();
}
