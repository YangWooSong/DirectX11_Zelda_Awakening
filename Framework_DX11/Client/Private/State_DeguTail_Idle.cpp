#include "stdafx.h"
#include "State_DeguTail_Idle.h"
#include "GameInstance.h"
#include "Model.h"
#include "DeguTail_00.h"

CState_DeguTail_Idle::CState_DeguTail_Idle(CFsm* pFsm, CMonster* pOwner)
    :CState{ pFsm }
    , m_pOwner{ pOwner }
{
}
HRESULT CState_DeguTail_Idle::Initialize(_uint iStateNum)
{
    m_iCurrentAnimIndex = m_pOwner->Get_Model()->Get_AnimationIndex("appear01");
    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_DeguTail_Idle::Start_State()
{
    m_pOwner->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.1f, true);
  //  m_pOwner->Set_AnimationSpeed(m_iCurrentAnimIndex, 40.f);

    return S_OK;
}

void CState_DeguTail_Idle::Update(_float fTimeDelta)
{
}

void CState_DeguTail_Idle::End_State()
{
}

CState_DeguTail_Idle* CState_DeguTail_Idle::Create(CFsm* pFsm, CMonster* pOwner, _uint iStateNum)
{
    CState_DeguTail_Idle* pInstance = new CState_DeguTail_Idle(pFsm, pOwner);

    if (FAILED(pInstance->Initialize(iStateNum)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_DeguTail_Idle"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_DeguTail_Idle::Free()
{
    __super::Free();
}
