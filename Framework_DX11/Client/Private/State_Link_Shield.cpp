#include "stdafx.h"
#include "State_Link_Shield.h"
#include "GameInstance.h"
#include "Link.h"

CState_Link_Shield::CState_Link_Shield(CFsm* pFsm, CPlayer* pPlayer)
    :CState{ pFsm }
    , m_pPlayer{ pPlayer }
{
}

HRESULT CState_Link_Shield::Initialize(_uint iStateNum)
{
    m_iStartAnimIndex = m_pPlayer->Get_Model()->Get_AnimationIndex("shield_st");
    m_iLoopAnimIndex = m_pPlayer->Get_Model()->Get_AnimationIndex("shield_lp");
    m_iEndAnimIndex = m_pPlayer->Get_Model()->Get_AnimationIndex("shield_ed");
    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_Link_Shield::Start_State()
{
    m_iCurrentAnimIndex = m_iStartAnimIndex;
    m_pPlayer->Get_Model()->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.1f);
   // if(m_pPlayer->Get_Fsm()->Get_PrevState() != CLink::SHIELD_WALK)
     //   m_pGameInstance->Play_Sound(TEXT("1_Link_Shield_On.wav"),SOUND_PLAYER_EFFECT, 1.f);
    return S_OK;
}

void CState_Link_Shield::Update(_float fTimeDelta)
{
    if (strcmp(m_pPlayer->Get_Model()->Get_CurrentAnimationName(), "shield_st") == 0 && m_pPlayer->Get_IsEnd_CurrentAnimation())
    {
        m_iCurrentAnimIndex = m_iLoopAnimIndex;
        m_pPlayer->Get_Model()->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.2f, true);
      //  m_pPlayer->Get_Model()->Set_AnimationSpeed(m_iCurrentAnimIndex, 60.f);
    }

    if (strcmp(m_pPlayer->Get_Model()->Get_CurrentAnimationName(), "shield_ed") == 0 && m_pPlayer->Get_IsEnd_CurrentAnimation())
    {
     //   m_pGameInstance->Play_Sound(TEXT("1_Link_Shield_Off.wav"), SOUND_PLAYER_EFFECT, 1.f);
        m_pPlayer->Change_State(CLink::IDLE);
    }

    if (KEY_AWAY(KEY::LSHIFT))
    {
        m_iCurrentAnimIndex = m_iEndAnimIndex;
        m_pPlayer->Get_Model()->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.01f);
        m_pPlayer->Get_Model()->Set_AnimationSpeed(m_iCurrentAnimIndex, 80.f);
    }

    if ((GetAsyncKeyState(0x57) & 0x8000) || (GetAsyncKeyState(0x41) & 0x8000) || (GetAsyncKeyState(0x53) & 0x8000) || (GetAsyncKeyState(0x44) & 0x8000))
    {
        m_pPlayer->Change_State(CLink::SHIELD_WALK);
        return;
    }

    if (KEY_TAP(KEY::O) )
    {
        m_pPlayer->Change_State(CLink::ITEMA);
    }

    if (KEY_TAP(KEY::P) )
    {
        m_pPlayer->Change_State(CLink::IDLE);
    }
}

void CState_Link_Shield::End_State()
{
}

CState_Link_Shield* CState_Link_Shield::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum)
{
    CState_Link_Shield* pInstance = new CState_Link_Shield(pFsm, pPlayer);

    if (FAILED(pInstance->Initialize(iStateNum)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Link_Shield"));
        Safe_Release(pInstance);
    }

    return pInstance;
}


void CState_Link_Shield::Free()
{
    __super::Free();
}

