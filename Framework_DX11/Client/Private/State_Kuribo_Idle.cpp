#include "stdafx.h"
#include "State_Kuribo_Idle.h"
#include "GameInstance.h"
#include "Model.h"
#include "Kuribo.h"

CState_Kuribo_Idle::CState_Kuribo_Idle(CFsm* pFsm, CMonster* pOwner)
    :CState{ pFsm }
    , m_pOwner{ pOwner }
{
}

HRESULT CState_Kuribo_Idle::Initialize(_uint iStateNum)
{
    m_iCurrentAnimIndex = m_pOwner->Get_Model()->Get_AnimationIndex("wait");
    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_Kuribo_Idle::Start_State()
{
    m_pOwner->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.1f, true);
    m_pOwner->Set_AnimationSpeed(m_iCurrentAnimIndex, 50.f);
    return S_OK;
}

void CState_Kuribo_Idle::Update(_float fTimeDelta)
{
    if (KEY_AWAY(Z))
    {
        m_pOwner->Change_State(CKuribo::DEAD);
    }
}

void CState_Kuribo_Idle::End_State()
{
}

CState_Kuribo_Idle* CState_Kuribo_Idle::Create(CFsm* pFsm, CMonster* pOwner, _uint iStateNum)
{
    CState_Kuribo_Idle* pInstance = new CState_Kuribo_Idle(pFsm, pOwner);

    if (FAILED(pInstance->Initialize(iStateNum)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Kuribo_Idle"));
        Safe_Release(pInstance);
    }

    return pInstance;
}
void CState_Kuribo_Idle::Free()
{
    __super::Free();
}

