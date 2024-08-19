#include "stdafx.h"
#include "State_Togezo_Discover.h"
#include "GameInstance.h"
#include "Model.h"
#include "Togezo.h"

CState_Togezo_Discover::CState_Togezo_Discover(CFsm* pFsm, CMonster* pOwner)
    :CState{ pFsm }
    , m_pOwner{ pOwner }
{
}

HRESULT CState_Togezo_Discover::Initialize(_uint iStateNum)
{
    m_iCurrentAnimIndex = m_pOwner->Get_Model()->Get_AnimationIndex("discover");
    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_Togezo_Discover::Start_State()
{
    m_pOwner->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.1f);
    m_pOwner->Set_AnimationSpeed(m_iCurrentAnimIndex, 70.f);
    return S_OK;
}

void CState_Togezo_Discover::Update(_float fTimeDelta)
{
    if(m_pOwner->Get_IsEnd_CurrentAnimation())
        m_pOwner->Change_State(CTogezo::RUN);
}

void CState_Togezo_Discover::End_State()
{
}

CState_Togezo_Discover* CState_Togezo_Discover::Create(CFsm* pFsm, CMonster* pOwner, _uint iStateNum)
{
    CState_Togezo_Discover* pInstance = new CState_Togezo_Discover(pFsm, pOwner);

    if (FAILED(pInstance->Initialize(iStateNum)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Togezo_Discover"));
        Safe_Release(pInstance);
    }

    return pInstance;
}
void CState_Togezo_Discover::Free()
{
    __super::Free();
}

