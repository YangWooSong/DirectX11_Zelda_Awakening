#include "stdafx.h"
#include "State_Link_Carry_Walk.h"
#include "GameInstance.h"
#include "Model.h"
#include "Link.h"

CState_Link_Carry_Walk::CState_Link_Carry_Walk(CFsm* pFsm, CPlayer* pPlayer)
    :CState{ pFsm }
    , m_pPlayer{ pPlayer }
{
}

HRESULT CState_Link_Carry_Walk::Initialize(_uint iStateNum)
{
    m_iCurrentAnimIndex = m_pPlayer->Get_Model()->Get_AnimationIndex("walk_carry");
    m_iStateNum = iStateNum;
    m_fPlayerSpeed = m_pPlayer->Get_MoveSpeed() * 0.3f;
    m_pPlayer_Navigation = m_pPlayer->Get_Navigation();
    return S_OK;
}

HRESULT CState_Link_Carry_Walk::Start_State()
{
    m_pPlayer->Get_Model()->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.1f, true, 0);
    m_pPlayer->Get_Model()->Set_AnimationSpeed(m_iCurrentAnimIndex, 60.f);
    m_iPlayerDir = m_pPlayer->Get_Player_Dir();
    return S_OK;
}

void CState_Link_Carry_Walk::Update(_float fTimeDelta)
{
    if (m_pPlayer->Get_LevelIndex() == LEVEL_FIELD)
        m_pPlayer->Get_PlayerSound()->Play_SoundRepeat(TEXT("1_Field_FootStep_VerySlow.wav"), 0.2f);
    else
        m_pPlayer->Get_PlayerSound()->Play_SoundRepeat(TEXT("1_Inside_FootStep_VerySlow.wav"), 0.4f);

    m_iPlayerDir = m_pPlayer->Get_Player_Dir();
    switch (m_iPlayerDir)
    {
    case CPlayer::FRONT:
        m_pPlayer->Get_Transform()->RotationThreeAxis(_float3(0.f, 0.f, 0.f));
        break;
    case CPlayer::LEFT:
        m_pPlayer->Get_Transform()->RotationThreeAxis(_float3(0.f, 270.f, 0.f));
        break;
    case CPlayer::LEFTUFRONT:
        m_pPlayer->Get_Transform()->RotationThreeAxis(_float3(0.f, 315.f, 0.f));
        break;
    case CPlayer::LEFTBACK:
        m_pPlayer->Get_Transform()->RotationThreeAxis(_float3(0.f, 225.f, 0.f));
        break;
    case CPlayer::BACK:
        m_pPlayer->Get_Transform()->RotationThreeAxis(_float3(0.f, 180.f, 0.f));
        break;
    case CPlayer::RIGHT:
        m_pPlayer->Get_Transform()->RotationThreeAxis(_float3(0.f, 90.f, 0.f));
        break;
    case CPlayer::RIGHTFRONT:
        m_pPlayer->Get_Transform()->RotationThreeAxis(_float3(0.f, 45.f, 0.f));
        break;
    case CPlayer::RIGHTBACK:
        m_pPlayer->Get_Transform()->RotationThreeAxis(_float3(0.f, 135.f, 0.f));
        break;
    default:
        break;
    }

    //방향키가 눌린게 없어야 Idle로 전환 (W,A,S,D 순서)
    if (!(GetAsyncKeyState(0x57) & 0x8000) && !(GetAsyncKeyState(0x41) & 0x8000) && !(GetAsyncKeyState(0x53) & 0x8000) && !(GetAsyncKeyState(0x44) & 0x8000))
    {
        m_pPlayer->Change_State(CLink::CARRY);
        return;
    }

    //모델 회전
    if (KEY_HOLD(KEY::W))
    {
        m_pPlayer->Set_Player_Dir(CPlayer::FRONT);
    }
    if (KEY_HOLD(KEY::S))
    {
        m_pPlayer->Set_Player_Dir(CPlayer::BACK);
    }
    if (KEY_HOLD(KEY::D))
    {
        m_pPlayer->Set_Player_Dir(CPlayer::RIGHT);
    }
    if (KEY_HOLD(KEY::A))
    {
        m_pPlayer->Set_Player_Dir(CPlayer::LEFT);
    }

    m_pPlayer->Go_Straight(fTimeDelta, m_fPlayerSpeed, m_pPlayer_Navigation);

    //방향 전환
    if (KEY_HOLD(KEY::W) && KEY_HOLD(KEY::A))
    {
        m_pPlayer->Set_Player_Dir(CPlayer::LEFTUFRONT);
    }
    if (KEY_HOLD(KEY::W) && KEY_HOLD(KEY::D))
    {
        m_pPlayer->Set_Player_Dir(CPlayer::RIGHTFRONT);
    }
    if (KEY_HOLD(KEY::S) && KEY_HOLD(KEY::A))
    {
        m_pPlayer->Set_Player_Dir(CPlayer::LEFTBACK);
    }
    if (KEY_HOLD(KEY::S) && KEY_HOLD(KEY::D))
    {
        m_pPlayer->Set_Player_Dir(CPlayer::RIGHTBACK);
    }

}

void CState_Link_Carry_Walk::End_State()
{
    m_fPressTime = 0.f;
    m_pPlayer->Get_PlayerSound()->Stop();
}

CState_Link_Carry_Walk* CState_Link_Carry_Walk::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum)
{
    CState_Link_Carry_Walk* pInstance = new CState_Link_Carry_Walk(pFsm, pPlayer);

    if (FAILED(pInstance->Initialize(iStateNum)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Link_Carry_Walk"));
        Safe_Release(pInstance);
    }

    return pInstance;
}


void CState_Link_Carry_Walk::Free()
{
    __super::Free();
}
