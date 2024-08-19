#include "stdafx.h"
#include "State_Spark_Idle.h"
#include "GameInstance.h"
#include "Model.h"
#include "Spark.h"

CState_Spark_Idle::CState_Spark_Idle(CFsm* pFsm, CMonster* pOwner)
	:CState{ pFsm }
	, m_pOwner{ pOwner }
{
}
HRESULT CState_Spark_Idle::Initialize(_uint iStateNum)
{
	m_iCurrentAnimIndex = m_pOwner->Get_Model()->Get_AnimationIndex("wait_side");
	m_iStateNum = iStateNum;

	return S_OK;
}

HRESULT CState_Spark_Idle::Start_State()
{
	m_pOwner->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.1f, true);
	//m_pOwner->Set_AnimationSpeed(m_iCurrentAnimIndex, 60.f);
	return S_OK;
}

void CState_Spark_Idle::Update(_float fTimeDelta)
{
	if(fabs(m_pOwner->Get_Pos().x - m_vTargetPos[m_iTargetIndex].x) < 0.1 
		&& fabs(m_pOwner->Get_Pos().y - m_vTargetPos[m_iTargetIndex].y) < 0.1 
		&& fabs(m_pOwner->Get_Pos().z - m_vTargetPos[m_iTargetIndex].z) < 0.1)
	{
		if (m_iTargetIndex == 3)
			m_iTargetIndex = 0;
		else
			m_iTargetIndex++;
	}

	m_pOwner->Get_Transform()->Go_Lerp(m_pOwner->Get_Pos(), m_vTargetPos[m_iTargetIndex], 0.1f);
}

void CState_Spark_Idle::End_State()
{
}

CState_Spark_Idle* CState_Spark_Idle::Create(CFsm* pFsm, CMonster* pOwner, _uint iStateNum)
{
	CState_Spark_Idle* pInstance = new CState_Spark_Idle(pFsm, pOwner);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Spark_Idle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Spark_Idle::Free()
{
	__super::Free();
}
