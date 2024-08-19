#include "stdafx.h"
#include "State_Kuribo_Walk.h"
#include "GameInstance.h"
#include "Model.h"
#include "Kuribo.h"

CState_Kuribo_Walk::CState_Kuribo_Walk(CFsm* pFsm, CMonster* pOwner)
    :CState{ pFsm }
    , m_pOwner{ pOwner }
{
}

HRESULT CState_Kuribo_Walk::Initialize(_uint iStateNum)
{
    m_iWalkAnimIndex = m_pOwner->Get_Model()->Get_AnimationIndex("walk");
    m_iCurrentAnimIndex = m_iWalkAnimIndex;
    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_Kuribo_Walk::Start_State()
{
    m_pOwner->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.1f, true);
    m_pOwner->Set_AnimationSpeed(m_iCurrentAnimIndex, 50.f);
    return S_OK;
}

void CState_Kuribo_Walk::Update(_float fTimeDelta)
{
    if (KEY_AWAY(Z))
    {
        m_pOwner->Change_State(CKuribo::WALK_SHAKE);
    }
}

void CState_Kuribo_Walk::End_State()
{
}

CState_Kuribo_Walk* CState_Kuribo_Walk::Create(CFsm* pFsm, CMonster* pOwner, _uint iStateNum)
{
    CState_Kuribo_Walk* pInstance = new CState_Kuribo_Walk(pFsm, pOwner);

    if (FAILED(pInstance->Initialize(iStateNum)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Kuribo_Walk"));
        Safe_Release(pInstance);
    }

    return pInstance;
}
void CState_Kuribo_Walk::Free()
{
    __super::Free();
}

