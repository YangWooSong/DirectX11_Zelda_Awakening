#include "stdafx.h"
#include "State_Link_Shield_Walk.h"
#include "GameInstance.h"
#include "Link.h"

CState_Link_Shield_Walk::CState_Link_Shield_Walk(CFsm* pFsm, CPlayer* pPlayer)
    :CState{ pFsm }
    , m_pPlayer{ pPlayer }
{
}

HRESULT CState_Link_Shield_Walk::Initialize(_uint iStateNum)
{
    m_iFrontAnimIndex = m_pPlayer->Get_Model()->Get_AnimationIndex("shield_hold_f");
    m_iBackAnimIndex = m_pPlayer->Get_Model()->Get_AnimationIndex("shield_hold_b");
    m_iLeftAnimIndex = m_pPlayer->Get_Model()->Get_AnimationIndex("shield_hold_l");
    m_iRightAnimIndex = m_pPlayer->Get_Model()->Get_AnimationIndex("shield_hold_r");

    m_iStateNum = iStateNum;

    m_fPlayerSpeed = m_pPlayer->Get_MoveSpeed() * 0.3f;
    m_pPlayer_Navigation = m_pPlayer->Get_Navigation();
    return S_OK;
}
HRESULT CState_Link_Shield_Walk::Start_State()
{
    m_iCurrentAnimIndex = m_iFrontAnimIndex;
    m_pPlayer->Get_Model()->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.1f, true);
    m_pPlayer->Get_Model()->Set_AnimationSpeed(m_iCurrentAnimIndex, 25.f);

    return S_OK;
}

void CState_Link_Shield_Walk::Update(_float fTimeDelta)
{
    if (m_pPlayer->Get_LevelIndex() == LEVEL_FIELD)
        m_pPlayer->Get_PlayerSound()->Play_SoundRepeat(TEXT("1_Field_FootStep_Slow.wav"), 0.3f);
    else
        m_pPlayer->Get_PlayerSound()->Play_SoundRepeat(TEXT("1_Inside_FootStep_Slow.wav"), 0.7f);

    if (!(GetAsyncKeyState(0x57) & 0x8000) && !(GetAsyncKeyState(0x41) & 0x8000) && !(GetAsyncKeyState(0x53) & 0x8000) && !(GetAsyncKeyState(0x44) & 0x8000)&& !(GetAsyncKeyState(0xA0) & 0x8000))
    {
        m_pPlayer->Change_State(CLink::IDLE);
        return;
    }

    if (!(GetAsyncKeyState(0x57) & 0x8000) && !(GetAsyncKeyState(0x41) & 0x8000) && !(GetAsyncKeyState(0x53) & 0x8000) && !(GetAsyncKeyState(0x44) & 0x8000))
    {
        m_pPlayer->Change_State(CLink::SHIELD);
        return;
    }
    
    if (KEY_AWAY(KEY::LSHIFT))
    {
        m_pPlayer->Change_State(CLink::WALK);
    }

    if (KEY_HOLD(KEY::W))
    {
        m_pPlayer->Set_Player_Dir(CPlayer::FRONT);
        m_pPlayer->Go_World_Straight(fTimeDelta, m_fPlayerSpeed, m_pPlayer_Navigation);
    }
    if (KEY_HOLD(KEY::S))
    {
        m_pPlayer->Set_Player_Dir(CPlayer::BACK);
        m_pPlayer->Go_World_Backward(fTimeDelta, m_fPlayerSpeed, m_pPlayer_Navigation);
    }
    if (KEY_HOLD(KEY::D))
    {
        m_pPlayer->Set_Player_Dir(CPlayer::RIGHT);
        m_pPlayer->Go_World_Right(fTimeDelta, m_fPlayerSpeed, m_pPlayer_Navigation);
    }
    if (KEY_HOLD(KEY::A))
    {
        m_pPlayer->Set_Player_Dir(CPlayer::LEFT);
        m_pPlayer->Go_World_Left(fTimeDelta, m_fPlayerSpeed, m_pPlayer_Navigation);
    }

    if (KEY_TAP(KEY::O))
    {
        m_pPlayer->Change_State(CLink::ITEMA);
    }

    if (KEY_AWAY(KEY::P))
    {
        m_pPlayer->Change_State(CLink::ITEMB);
    }
    else if (KEY_HOLD(KEY::P))
    {
        m_fPressTime += fTimeDelta;
        if (m_fPressTime > 0.2f)
            m_pPlayer->Change_State(CLink::SLASH_HOLD);
    }
}

void CState_Link_Shield_Walk::End_State()
{
    m_fPressTime = 0.f;
}

CState_Link_Shield_Walk* CState_Link_Shield_Walk::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum)
{
    CState_Link_Shield_Walk* pInstance = new CState_Link_Shield_Walk(pFsm, pPlayer);

    if (FAILED(pInstance->Initialize(iStateNum)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Link_Shield_Walk"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_Link_Shield_Walk::Free()
{
    __super::Free();
}

