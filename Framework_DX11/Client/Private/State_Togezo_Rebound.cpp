#include "stdafx.h"
#include "State_Togezo_Rebound.h"
#include "GameInstance.h"
#include "Model.h"
#include "Togezo.h"

CState_Togezo_Rebound::CState_Togezo_Rebound(CFsm* pFsm, CMonster* pOwner)
    :CState{ pFsm }
    , m_pOwner{ pOwner }
{
}

HRESULT CState_Togezo_Rebound::Initialize(_uint iStateNum)
{
    m_iReoundStIndex = m_pOwner->Get_Model()->Get_AnimationIndex("rebound_st");
    m_iReoundEdIndex = m_pOwner->Get_Model()->Get_AnimationIndex("rebound_ed");
    m_iReoundLpIndex = m_pOwner->Get_Model()->Get_AnimationIndex("rebound");
    m_iCurrentAnimIndex = m_iReoundStIndex;
    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_Togezo_Rebound::Start_State()
{
    if (m_iCurrentAnimIndex == m_iReoundLpIndex)
    {
        m_pOwner->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.1f, true);
        m_pOwner->Set_AnimationSpeed(m_iCurrentAnimIndex, 60.f);
    }
    else
    {
        m_pOwner->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.1f);
        m_pOwner->Set_AnimationSpeed(m_iCurrentAnimIndex, 50.f);
    }


    return S_OK;
}

void CState_Togezo_Rebound::Update(_float fTimeDelta)
{
    if (m_bPlaySound == false)
    {
        m_bPlaySound = true;
        m_pOwner->Get_Sound()->Play_Sound(TEXT("4_Obj_BladeTrap.wav"), 0.5f);
    }
    if (strcmp(m_pOwner->Get_Model()->Get_CurrentAnimationName(), "rebound_st") == 0 && m_pOwner->Get_IsEnd_CurrentAnimation())
    {
        m_iCurrentAnimIndex = m_iReoundEdIndex;
        m_pOwner->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.1f);
       m_pOwner->Set_AnimationSpeed(m_iCurrentAnimIndex, 60.f);
    }
    if (strcmp(m_pOwner->Get_Model()->Get_CurrentAnimationName(), "rebound_ed") == 0 && m_pOwner->Get_IsEnd_CurrentAnimation())
    {
        m_iCurrentAnimIndex = m_iReoundLpIndex;
        m_pOwner->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.1f, true);
        m_pOwner->Set_AnimationSpeed(m_iCurrentAnimIndex, 60.f);
    }
    if (KEY_AWAY(Z))
    {
        m_pOwner->Change_State(CTogezo::DAMAGE);
    }
}

void CState_Togezo_Rebound::End_State()
{
}

CState_Togezo_Rebound* CState_Togezo_Rebound::Create(CFsm* pFsm, CMonster* pOwner, _uint iStateNum)
{
    CState_Togezo_Rebound* pInstance = new CState_Togezo_Rebound(pFsm, pOwner);

    if (FAILED(pInstance->Initialize(iStateNum)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Togezo_Rebound"));
        Safe_Release(pInstance);
    }

    return pInstance;
}
void CState_Togezo_Rebound::Free()
{
    __super::Free();
}

