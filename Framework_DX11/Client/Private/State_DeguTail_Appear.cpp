#include "stdafx.h"
#include "State_DeguTail_Appear.h"
#include "GameInstance.h"
#include "Model.h"
#include "DeguTail_00.h"
#include"Level_Dungeon.h"
#include"PlayerCamera.h"

CState_DeguTail_Appear::CState_DeguTail_Appear(CFsm* pFsm, CMonster* pOwner)
    :CState{ pFsm }
    , m_pOwner{ pOwner }
{
}
HRESULT CState_DeguTail_Appear::Initialize(_uint iStateNum)
{
    m_iCurrentAnimIndex = m_pOwner->Get_Model()->Get_AnimationIndex("demo_pop");
    m_iStateNum = iStateNum;

    m_pCamera = static_cast<CPlayerCamera*>(m_pGameInstance->Find_Camera(LEVEL_DUNGEON));
    return S_OK;
}

HRESULT CState_DeguTail_Appear::Start_State()
{
    m_pOwner->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.f);
    m_pOwner->Set_AnimationSpeed(m_iCurrentAnimIndex, 60.f);
  
    return S_OK;
}

void CState_DeguTail_Appear::Update(_float fTimeDelta)
{
    _float a = m_pOwner->Get_Model()->Get_CurrentTrackPosition();
    int b = 10;

    if (m_bPlaySound == false)
    {
        m_bPlaySound = true;
        m_pOwner->Get_Sound()->Play_Sound(TEXT("2_DeguTail_Appear.wav"), 1.f);
    }

    if (strcmp(m_pOwner->Get_Model()->Get_CurrentAnimationName(), "demo_pop") == 0 && m_pOwner->Get_Model()->Get_CurrentTrackPosition() > 60.f)
    {
        if (m_bCarmeraSetting == false)
        {
            m_bCarmeraSetting = true;
            _float3 v_Offset = _float3{ 0.0f, 20.0f, -12.f };
            m_pCamera->Set_TargetToOtherPos(m_pOwner->Get_Pos_vector() + XMLoadFloat3(&v_Offset));
            m_pCamera->Zoom_In(1.4f, 60.f);
        }
    }

    if (strcmp(m_pOwner->Get_Model()->Get_CurrentAnimationName(), "demo_pop") == 0 && m_pOwner->Get_Model()->Get_CurrentTrackPosition() > 440.f && m_bCameraShake == false)
    {
        static_cast<CDeguTail_00*>(m_pOwner)->Set_bRenderText(true);
        m_bCameraShake = true;
        m_pCamera->Start_Shake(0.5f, 0.1f, 6.f);
    }

    if (m_pOwner->Get_IsEnd_CurrentAnimation())
        m_pOwner->Change_State(CDeguTail_00::WALK);
}

void CState_DeguTail_Appear::End_State()
{
    m_pCamera->Stop_Shake();
    m_pCamera->Set_TargetToOtherPos(_vector{ 45.f, 20.f, 37.f, });
    m_pCamera->Zoom_Out(1.4f, 60.f);
    
    static_cast<CDeguTail_00*>(m_pOwner)->Set_bRenderText(false);

    m_pGameInstance->Stop_BGM();
    m_pGameInstance->Play_BGM(TEXT("0_Dangeon_Boss.wav"), 0.6f);
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
