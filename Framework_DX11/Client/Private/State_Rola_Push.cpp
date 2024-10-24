#include "stdafx.h"
#include "State_Rola_Push.h"
#include "GameInstance.h"
#include "Model.h"
#include "Rola.h"
#include "RollingSpike.h"

CState_Rola_Push::CState_Rola_Push(CFsm* pFsm, CMonster* pOwner)
    :CState{ pFsm }
    , m_pOwner{ pOwner }
{
}

HRESULT CState_Rola_Push::Initialize(_uint iStateNum)
{
    m_iCurrentAnimIndex = m_pOwner->Get_Model()->Get_AnimationIndex("push");
    m_iStateNum = iStateNum;
    m_pAddDir = static_cast<CRola*>(m_pOwner)->Get_AddDir();
    m_pARrived = static_cast<CRola*>(m_pOwner)->Get_Arrived();
    m_pTargetIndex = static_cast<CRola*>(m_pOwner)->Get_TargetPosIndex();
    m_pRollingSpike = static_cast<CRollingSpike*>(m_pGameInstance->Find_Object(LEVEL_DUNGEON, TEXT("Layer_RollingSpike"), 0));
    return S_OK;
}

HRESULT CState_Rola_Push::Start_State()
{
    m_pOwner->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.f);
    m_pOwner->Set_AnimationSpeed(m_iCurrentAnimIndex, 40.f);
    return S_OK;
}

void CState_Rola_Push::Update(_float fTimeDelta)
{
    //È¸Àü
    if(*m_pTargetIndex == 0)
    {
        m_pOwner->Get_Transform()->Turn_Lerp(_vector{-1.f, 0.f,0.f}, 5.f, fTimeDelta);
    }
    else if (*m_pTargetIndex == 4)
    {
        m_pOwner->Get_Transform()->Turn_Lerp(_vector{ 1.f, 0.f,0.f }, 5.f, fTimeDelta);
    }
        
    if (m_pOwner->Get_Model()->Get_CurrentTrackPosition() > 35.f)
    {
        if (*m_pARrived)
        {
            *m_pARrived = false;
            *m_pAddDir *= -1;
        }
        if (*m_pAddDir == -1)
            m_pRollingSpike->Start_Move_Right();
        else
            m_pRollingSpike->Start_Move_Left();
        static_cast<CRola*>(m_pOwner)->Set_Active_HandEffect(true);
    }

    if (m_pOwner->Get_Model()->Get_CurrentTrackPosition() > 45.f)
    {
        static_cast<CRola*>(m_pOwner)->Set_Active_HandEffect(false);
    }
    if(strcmp(m_pOwner->Get_Model()->Get_CurrentAnimationName(), "push") == 0 && m_pOwner->Get_IsEnd_CurrentAnimation())
    { 
        m_pOwner->Change_State(CRola::JUMP);
    }
}

void CState_Rola_Push::End_State()
{
    
}

CState_Rola_Push* CState_Rola_Push::Create(CFsm* pFsm, CMonster* pOwner, _uint iStateNum)
{
    CState_Rola_Push* pInstance = new CState_Rola_Push(pFsm, pOwner);

    if (FAILED(pInstance->Initialize(iStateNum)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Rola_Push"));
        Safe_Release(pInstance);
    }

    return pInstance;
}
void CState_Rola_Push::Free()
{
    __super::Free();
}

