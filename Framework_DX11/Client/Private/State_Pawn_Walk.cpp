#include "stdafx.h"
#include "State_Pawn_Walk.h"
#include "GameInstance.h"
#include "Model.h"
#include "Pawn.h"
#include "Link.h"

CState_Pawn_Walk::CState_Pawn_Walk(CFsm* pFsm, CMonster* pOwner)
    :CState{ pFsm }
    , m_pOwner{ pOwner }
{
}


HRESULT CState_Pawn_Walk::Initialize(_uint iStateNum)
{
    m_iCurrentAnimIndex = m_pOwner->Get_Model()->Get_AnimationIndex("walk");
    m_iStateNum = iStateNum;

    return S_OK;
}


HRESULT CState_Pawn_Walk::Start_State()
{
    m_pOwner->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.1f, true);
    // m_pOwner->Set_AnimationSpeed(m_iCurrentAnimIndex, 60.f);

    m_pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Player(LEVEL_DUNGEON));

    return S_OK;
}

void CState_Pawn_Walk::Update(_float fTimeDelta)
{
    m_pOwner->Get_Transform()->LookAt(m_pPlayer->Get_Position());

    Check_Jump(fTimeDelta);


    XMStoreFloat3(&m_vPlayerPos, m_pPlayer->Get_Transform()->Get_State(CTransform::STATE_POSITION));
    m_pOwner->Get_Transform()->Go_Lerp(m_pOwner->Get_Pos(), m_vPlayerPos, 0.02f, false, m_pOwner->Get_NavigationCom());
    m_pOwner->Get_Transform()->Go_Lerp(m_pOwner->Get_Pos(), m_vPlayerPos, 0.02f, false, m_pOwner->Get_NavigationCom());


    //멀어지면 상태 변환
   _vector m_vPlayerPos = m_pPlayer->Get_Transform()->Get_State(CTransform::STATE_POSITION);
    _float fDistance = m_pOwner->Get_Distance(m_pOwner->Get_Pos_vector(), m_vPlayerPos);

    if (fDistance > 5.f)
    {
        m_pOwner->Change_State(CPawn::IDLE);
    }
  
}


void CState_Pawn_Walk::End_State()
{
}


void CState_Pawn_Walk::Check_Jump(_float _fTimeDelta)
{
    if (m_pPlayer->Get_Player_State() == CLink::SLASH || m_pPlayer->Get_Player_State() == CLink::SLASH_HOLD)
    {
       if(m_pPlayer->Get_Model()->Get_CurrentTrackPosition() < 10.f)
            m_pOwner->Change_State(CPawn::JUMP);
    }

}

CState_Pawn_Walk* CState_Pawn_Walk::Create(CFsm* pFsm, CMonster* pOwner, _uint iStateNum)
{
    CState_Pawn_Walk* pInstance = new CState_Pawn_Walk(pFsm, pOwner);

    if (FAILED(pInstance->Initialize(iStateNum)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Pawn_Walk"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_Pawn_Walk::Free()
{
    __super::Free();
}

