#include "stdafx.h"
#include "State_Vegas_Damage.h"
#include "GameInstance.h"
#include "Model.h"
#include "Vegas.h"

CState_Vegas_Damage::CState_Vegas_Damage(CFsm* pFsm, CMonster* pOwner)
	:CState{ pFsm }
	, m_pOwner{ pOwner }
{
}

HRESULT CState_Vegas_Damage::Initialize(_uint iStateNum)
{
	m_iCurrentAnimIndex = m_pOwner->Get_Model()->Get_AnimationIndex("damage");
	m_iStateNum = iStateNum;

	return S_OK;
}

HRESULT CState_Vegas_Damage::Start_State()
{
	m_pOwner->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.1f, true);
	m_pOwner->Set_AnimationSpeed(m_iCurrentAnimIndex, 30.f);
	return S_OK;
}

void CState_Vegas_Damage::Update(_float fTimeDelta)
{
}

void CState_Vegas_Damage::End_State()
{
}

CState_Vegas_Damage* CState_Vegas_Damage::Create(CFsm* pFsm, CMonster* pOwner, _uint iStateNum)
{
	CState_Vegas_Damage* pInstance = new CState_Vegas_Damage(pFsm, pOwner);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Vegas_Damage"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Vegas_Damage::Free()
{
	__super::Free();
}
