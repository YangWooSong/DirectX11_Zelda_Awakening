#include "stdafx.h"
#include "State_Rola_Push.h"
#include "GameInstance.h"
#include "Model.h"
#include "Rola.h"

CState_Rola_Push::CState_Rola_Push(CFsm* pFsm, CMonster* pOwner)
    :CState{ pFsm }
    , m_pOwner{ pOwner }
{
}

HRESULT CState_Rola_Push::Initialize(_uint iStateNum)
{
    m_iCurrentAnimIndex = m_pOwner->Get_Model()->Get_AnimationIndex("push");
    m_iStateNum = iStateNum;
    m_pTargetIndex = static_cast<CRola*>(m_pOwner)->Get_TargetPosIndex();
    return S_OK;
}

HRESULT CState_Rola_Push::Start_State()
{
    m_pOwner->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.1f);
    m_pOwner->Set_AnimationSpeed(m_iCurrentAnimIndex, 40.f);
    return S_OK;
}

void CState_Rola_Push::Update(_float fTimeDelta)
{
    //if(*m_pTargetIndex == 0)
    //{
    //    m_pOwner->Get_Transform()->Turn_Lerp_Angle(m_pOwner->Get_Rotation(), _float3(0.f, 90.f, 0.f), fTimeDelta);
    //}
    //else if (*m_pTargetIndex == 4)
    //{
    //    m_pOwner->Get_Transform()->Turn_Lerp_Angle(m_pOwner->Get_Rotation(), _float3(0.f, -90.f, 0.f), fTimeDelta);
    //}
    //    
    if(m_pOwner->Get_IsEnd_CurrentAnimation())
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

