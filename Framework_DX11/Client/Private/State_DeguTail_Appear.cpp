#include "stdafx.h"
#include "State_DeguTail_Appear.h"
#include "GameInstance.h"
#include "Model.h"
#include "DeguTail_00.h"
#include"Level_Dungeon.h"
CState_DeguTail_Appear::CState_DeguTail_Appear(CFsm* pFsm, CMonster* pOwner)
    :CState{ pFsm }
    , m_pOwner{ pOwner }
{
}
HRESULT CState_DeguTail_Appear::Initialize(_uint iStateNum)
{
    m_iCurrentAnimIndex = m_pOwner->Get_Model()->Get_AnimationIndex("demo_pop");
    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_DeguTail_Appear::Start_State()
{
    m_pOwner->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.1f);
    m_pOwner->Set_AnimationSpeed(m_iCurrentAnimIndex, 60.f);
  //  m_pGameInstance->Play_Sound(TEXT("2_DeguTail_Appear.wav"), SOUND_MONSTER, 1.f);
    return S_OK;
}

void CState_DeguTail_Appear::Update(_float fTimeDelta)
{
    if (m_pOwner->Get_IsEnd_CurrentAnimation())
        m_pOwner->Change_State(CDeguTail_00::WALK);
}

void CState_DeguTail_Appear::End_State()
{
  //  m_pGameInstance->Stop_Sound(SOUND_BGM);
  //  m_pGameInstance->Play_SoundRepeat(TEXT("0_Dangeon_Boss.wav"), SOUND_BGM, 0.6f);
}

CState_DeguTail_Appear* CState_DeguTail_Appear::Create(CFsm* pFsm, CMonster* pOwner, _uint iStateNum)
{
    CState_DeguTail_Appear* pInstance = new CState_DeguTail_Appear(pFsm, pOwner);

    if (FAILED(pInstance->Initialize(iStateNum)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_DeguTail_Appear"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_DeguTail_Appear::Free()
{
    __super::Free();
}
