#include "stdafx.h"
#include "State_Pawn_Jump.h"
#include "GameInstance.h"
#include "Model.h"
#include "Pawn.h"

CState_Pawn_Jump::CState_Pawn_Jump(CFsm* pFsm, CMonster* pOwner)
	:CState{ pFsm }
	, m_pOwner{ pOwner }
{
}

HRESULT CState_Pawn_Jump::Initialize(_uint iStateNum)
{
	m_iCurrentAnimIndex = m_pOwner->Get_Model()->Get_AnimationIndex("walk");
	m_iStateNum = iStateNum;
	m_fMoveSpeed = 3.f;
	return S_OK;
}


HRESULT CState_Pawn_Jump::Start_State()
{
	m_pOwner->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.1f, true);
	// m_pOwner->Set_AnimationSpeed(m_iCurrentAnimIndex, 60.f);

	m_bJump = true;
	m_bStart = true;

	return S_OK;
}

void CState_Pawn_Jump::Update(_float fTimeDelta)
{
	if (m_bJump)
		Jump();
	else
		m_pOwner->Change_State(CPawn::IDLE);

	m_pOwner->Get_Transform()->Go_Backward(fTimeDelta, m_fMoveSpeed);
}

void CState_Pawn_Jump::End_State()
{
}

void CState_Pawn_Jump::Jump()
{
	_float3 fCurrentPos = {};
	XMStoreFloat3(&fCurrentPos, m_pOwner->Get_Transform()->Get_State(CTransform::STATE_POSITION));

	//점프 처음 시작할 때 위치 저장
	if (m_bStart == true)
	{
		m_fJumpStartY = fCurrentPos.y;
		m_bStart = false;
	}

	if (m_fJumpStartY + m_fJumpMax < fCurrentPos.y) //점프 최대치 도달시 방향 전환
		m_fJumpAmount = m_fJumpAmount * (-1.f);

	if (m_fJumpStartY > fCurrentPos.y)
	{
		fCurrentPos.y = m_fJumpStartY;
		m_pOwner->Get_Transform()->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&fCurrentPos));
		m_fJumpAmount = m_fJumpAmount * (-1.f);
		m_bJump = false;
		return;
	}

	fCurrentPos.y += m_fJumpAmount;
	m_pOwner->Get_Transform()->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&fCurrentPos));
}

CState_Pawn_Jump* CState_Pawn_Jump::Create(CFsm* pFsm, CMonster* pOwner, _uint iStateNum)
{
	CState_Pawn_Jump* pInstance = new CState_Pawn_Jump(pFsm, pOwner);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Pawn_Jump"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CState_Pawn_Jump::Free()
{
	__super::Free();
}


