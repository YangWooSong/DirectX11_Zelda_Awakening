#include "stdafx.h"
#include "State_DeguTail_Hurt.h"
#include "GameInstance.h"
#include "Model.h"
#include "DeguTail_00.h"

CState_DeguTail_Hurt::CState_DeguTail_Hurt(CFsm* pFsm, CMonster* pOwner)
    :CState{ pFsm }
    , m_pOwner{ pOwner }
{
}
HRESULT CState_DeguTail_Hurt::Initialize(_uint iStateNum)
{
    m_iHitAnimIndex = m_pOwner->Get_Model()->Get_AnimationIndex("damage01");
    m_iSpinAnimIndex = m_pOwner->Get_Model()->Get_AnimationIndex("damage_turn01");

    m_iCurrentAnimIndex = m_iHitAnimIndex;
    m_iStateNum = iStateNum;
    m_pOwnerTransform = m_pOwner->Get_Transform();
    m_pDeguOwner = static_cast<CDeguTail_00*>(m_pOwner);
    return S_OK;
}

HRESULT CState_DeguTail_Hurt::Start_State()
{
    m_iCurrentAnimIndex = m_iHitAnimIndex;

    m_pOwner->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.1f);
    m_pOwner->Set_AnimationSpeed(m_iCurrentAnimIndex, 50.f);
    m_pOwner->Minus_Hp();

    m_pOwner->Get_Sound()->Stop();
    m_pOwner->Get_Sound()->Play_Sound(TEXT("3_Monster_Hit.wav"), 1.f);
  
  
    return S_OK;
}

void CState_DeguTail_Hurt::Update(_float fTimeDelta)
{
    m_fBlinkTimer += fTimeDelta;

    if (m_fBlinkTimer > 0.2f)
    {
        m_fBlinkTimer = 0.f;
        m_pDeguOwner->Set_bBlink(!m_pDeguOwner->Get_bBlink());
    }
    if (m_iCurrentAnimIndex == m_iHitAnimIndex && m_pOwner->Get_IsEnd_CurrentAnimation())
    {
        static_cast<CDeguTail_00*>(m_pOwner)->Set_Active_AngryEffect(true);
        m_pOwner->Get_Sound()->Play_Sound(TEXT("2_DeguTail_Angry.wav"), 1.f);
        m_iCurrentAnimIndex = m_iSpinAnimIndex;
        m_pOwner->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.1f, true);
        m_pOwner->Set_AnimationSpeed(m_iCurrentAnimIndex, 70.f);
    }

    if (m_iCurrentAnimIndex == m_iSpinAnimIndex)
    {
        m_fTimer += fTimeDelta;

        if (m_fTimer < 1.f)
            m_pOwnerTransform->Turn_Lerp(m_pOwnerTransform->Get_State(CTransform::STATE_RIGHT), 7.f, fTimeDelta);
        else
        {
            static_cast<CDeguTail_00*>(m_pOwner)->Set_Angry(true);
            m_pOwner->Change_State(CDeguTail_00::WALK);
        }
    }
        
}

void CState_DeguTail_Hurt::End_State()
{
    m_pDeguOwner->Set_bBlink(false);

    if(m_pDeguOwner->Get_Hp() == 2)
         m_pDeguOwner->Set_bOutBodyRed(true);
    static_cast<CDeguTail_00*>(m_pOwner)->Set_Active_AngryEffect(false);
    m_fTimer = 0.f;
}

CState_DeguTail_Hurt* CState_DeguTail_Hurt::Create(CFsm* pFsm, CMonster* pOwner, _uint iStateNum)
{
    CState_DeguTail_Hurt* pInstance = new CState_DeguTail_Hurt(pFsm, pOwner);

    if (FAILED(pInstance->Initialize(iStateNum)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_DeguTail_Hurt"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_DeguTail_Hurt::Free()
{
    __super::Free();
}
