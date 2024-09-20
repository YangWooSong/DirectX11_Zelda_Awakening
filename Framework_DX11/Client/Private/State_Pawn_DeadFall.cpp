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
    m_fTimer += fTimeDelta;

    if (m_fTimer > 0.7f)
    {
        if(m_bPlaySound == false)
        {
            m_pOwner->Get_Sound()->Play_Sound(TEXT("3_Pawn_Dead.wav"), 0.8f);
            m_bPlaySound = true;
        }
        m_pOwner->Get_Transform()->Go_World_Down(fTimeDelta, 2.f);
        m_fFallTimer += fTimeDelta;
    }

    if (m_fFallTimer > 1.f)
    {
        m_pOwner->SetActive(false);
        m_pOwner->Set_Dead(true);
    }
}


void CState_Pawn_DeadFall::End_State()
{
    m_fTimer = 0.f;
    m_fFallTimer = 0.f;
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

