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
	m_pOwner->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.1f);
	m_pOwner->Set_AnimationSpeed(m_iCurrentAnimIndex, 45.f);
	m_pOwner->Set_NewRandom_Dir();
	m_iOwnerDir = m_pOwner->Get_Monster_Dir();
	return S_OK;
}

void CState_Vegas_Idle::Update(_float fTimeDelta)
{
	

	if (m_pOwner->Get_IsEnd_CurrentAnimation())
	{
		switch (m_iOwnerDir)
		{
		case CMonster::FRONT:
			m_pOwner->Change_State(CVegas::WALK_F);
			break;
		case CMonster::BACK:
			m_pOwner->Change_State(CVegas::WALK_B);
			break;
		case CMonster::RIGHT:
			m_pOwner->Change_State(CVegas::WALK_R);
			break;
		case CMonster::LEFT:
			m_pOwner->Change_State(CVegas::WALK_L);
			break;
		default:
			break;
		}
	}
	else
	{
		//앉았다 일어나는 애니 맞춰서 y축 이동하는 코드
		if (m_fTimer > 0.38f)
		{
			m_fTimer = 0;
			m_fSpeed *= -1.f;
		}
		else
			m_fTimer += fTimeDelta;

		_float3 pos = m_pOwner->Get_Pos();
		pos.y += m_fSpeed;
		m_pOwner->Get_Transform()->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&pos));
	}


}

void CState_Vegas_Idle::End_State()
{
	m_fSpeed = -0.008f;
	m_fTimer = 0;
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
