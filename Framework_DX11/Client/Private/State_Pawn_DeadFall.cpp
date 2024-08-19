#include "stdafx.h"
#include "State_Pawn_DeadFall.h"
#include "GameInstance.h"
#include "Model.h"
#include "Pawn.h"

CState_Pawn_DeadFall::CState_Pawn_DeadFall(CFsm* pFsm, CMonster* pOwner)
    :CState{ pFsm }
    , m_pOwner{ pOwner }
{
}


HRESULT CState_Pawn_DeadFall::Initialize(_uint iStateNum)
{
    m_iCurrentAnimIndex = m_pOwner->Get_Model()->Get_AnimationIndex("deadfall");
    m_iStateNum = iStateNum;

    return S_OK;
}


HRESULT CState_Pawn_DeadFall::Start_State()
{
    m_pOwner->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.1f, true);
    m_pOwner->Set_AnimationSpeed(m_iCurrentAnimIndex, 40.f);
    return S_OK;
}

void CState_Pawn_DeadFall::Update(_float fTimeDelta)
{

}


void CState_Pawn_DeadFall::End_State()
{
}

CState_Pawn_DeadFall* CState_Pawn_DeadFall::Create(CFsm* pFsm, CMonster* pOwner, _uint iStateNum)
{
    CState_Pawn_DeadFall* pInstance = new CState_Pawn_DeadFall(pFsm, pOwner);

    if (FAILED(pInstance->Initialize(iStateNum)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Pawn_DeadFall"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_Pawn_DeadFall::Free()
{
    __super::Free();
}

