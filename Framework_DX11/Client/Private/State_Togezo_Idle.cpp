#include "stdafx.h"
#include "State_Togezo_Idle.h"
#include "GameInstance.h"
#include "Model.h"
#include "Togezo.h"

CState_Togezo_Idle::CState_Togezo_Idle(CFsm* pFsm, CMonster* pOwner)
    :CState{ pFsm }
    , m_pOwner{ pOwner }
{
}

HRESULT CState_Togezo_Idle::Initialize(_uint iStateNum)
{
    m_iCurrentAnimIndex = m_pOwner->Get_Model()->Get_AnimationIndex("wait");
    m_iStateNum = iStateNum;

    m_bDetectHori = static_cast<CTogezo*>(m_pOwner)->Get_bDetectHor();
    m_bDetectVer = static_cast<CTogezo*>(m_pOwner)->Get_bDetectVer();
    return S_OK;
}

HRESULT CState_Togezo_Idle::Start_State()
{
    m_pOwner->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.1f, true);
    m_pOwner->Set_AnimationSpeed(m_iCurrentAnimIndex, 50.f);
    return S_OK;
}

void CState_Togezo_Idle::Update(_float fTimeDelta)
{
    if (*m_bDetectHori || *m_bDetectVer)
    {
        m_pOwner->Change_State(CTogezo::DISCOVER);
    }
}

void CState_Togezo_Idle::End_State()
{
}

CState_Togezo_Idle* CState_Togezo_Idle::Create(CFsm* pFsm, CMonster* pOwner, _uint iStateNum)
{
    CState_Togezo_Idle* pInstance = new CState_Togezo_Idle(pFsm, pOwner);

    if (FAILED(pInstance->Initialize(iStateNum)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Togezo_Idle"));
        Safe_Release(pInstance);
    }

    return pInstance;
}
void CState_Togezo_Idle::Free()
{
    __super::Free();
}

