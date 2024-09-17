#include "stdafx.h"
#include "State_Pawn_Damage.h"
#include "GameInstance.h"
#include "Model.h"
#include "Pawn.h"

CState_Pawn_Damage::CState_Pawn_Damage(CFsm* pFsm, CMonster* pOwner)
    :CState{ pFsm }
    , m_pOwner{ pOwner }
{
}


HRESULT CState_Pawn_Damage::Initialize(_uint iStateNum)
{
    m_iCurrentAnimIndex = m_pOwner->Get_Model()->Get_AnimationIndex("damage");
    m_iStateNum = iStateNum;

    return S_OK;
}


HRESULT CState_Pawn_Damage::Start_State()
{
    m_pOwner->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.1f);
    m_pOwner->Set_AnimationSpeed(m_iCurrentAnimIndex, 50.f);
    m_pOwner->Get_Sound()->Play_Sound(TEXT("3_Pawn_Damage.wav"),0.8f);
    return S_OK;
}

void CState_Pawn_Damage::Update(_float fTimeDelta)
{
    if (m_pOwner->Get_IsEnd_CurrentAnimation())
    {
        m_pOwner->Change_State(CPawn::IDLE);
    }
}


void CState_Pawn_Damage::End_State()
{
}

CState_Pawn_Damage* CState_Pawn_Damage::Create(CFsm* pFsm, CMonster* pOwner, _uint iStateNum)
{
    CState_Pawn_Damage* pInstance = new CState_Pawn_Damage(pFsm, pOwner);

    if (FAILED(pInstance->Initialize(iStateNum)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Pawn_Damage"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_Pawn_Damage::Free()
{
    __super::Free();
}

