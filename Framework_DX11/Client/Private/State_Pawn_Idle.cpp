#include "stdafx.h"
#include "State_Pawn_Idle.h"
#include "GameInstance.h"
#include "Model.h"
#include "Pawn.h"

CState_Pawn_Idle::CState_Pawn_Idle(CFsm* pFsm, CMonster* pOwner)
    :CState{ pFsm }
    , m_pOwner{ pOwner }
{
}


HRESULT CState_Pawn_Idle::Initialize(_uint iStateNum)
{
    m_iCurrentAnimIndex = m_pOwner->Get_Model()->Get_AnimationIndex("wait");
    m_iStateNum = iStateNum;

    return S_OK;
}


HRESULT CState_Pawn_Idle::Start_State()
{
    m_pPlayer = m_pGameInstance->Find_Player(LEVEL_DUNGEON);
    m_pOwner->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.1f, true);
   //m_pOwner->Set_AnimationSpeed(m_iCurrentAnimIndex, 60.f);
    return S_OK;
}

void CState_Pawn_Idle::Update(_float fTimeDelta)
{
    m_vPlayerPos = m_pPlayer->Get_Transform()->Get_State(CTransform::STATE_POSITION);
    _float fDistance = m_pOwner->Get_Distance(m_pOwner->Get_Pos_vector(), m_vPlayerPos);

    if (fDistance < 5.f)
    {
        m_pOwner->Get_Transform()->LookAt(m_vPlayerPos);
        m_pOwner->Change_State(CPawn::WALK);
    }
}


void CState_Pawn_Idle::End_State()
{
}

CState_Pawn_Idle* CState_Pawn_Idle::Create(CFsm* pFsm, CMonster* pOwner, _uint iStateNum)
{
    CState_Pawn_Idle* pInstance = new CState_Pawn_Idle(pFsm, pOwner);

    if (FAILED(pInstance->Initialize(iStateNum)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Pawn_Idle"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_Pawn_Idle::Free()
{
    __super::Free();
}

