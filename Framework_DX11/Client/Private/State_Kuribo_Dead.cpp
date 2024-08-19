#include "stdafx.h"
#include "State_Kuribo_Dead.h"
#include "GameInstance.h"
#include "Model.h"
#include "Kuribo.h"

CState_Kuribo_Dead::CState_Kuribo_Dead(CFsm* pFsm, CMonster* pOwner)
    :CState{ pFsm }
    , m_pOwner{ pOwner }
{
}

HRESULT CState_Kuribo_Dead::Initialize(_uint iStateNum)
{
    m_iCurrentAnimIndex = m_pOwner->Get_Model()->Get_AnimationIndex("dead");
    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_Kuribo_Dead::Start_State()
{
    m_pOwner->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.1f);
    m_pOwner->Set_AnimationSpeed(m_iCurrentAnimIndex, 50.f);
    return S_OK;
}

void CState_Kuribo_Dead::Update(_float fTimeDelta)
{
    if(m_pOwner->Get_IsEnd_CurrentAnimation())
        m_pOwner->Change_State(CKuribo::WALK);

}

void CState_Kuribo_Dead::End_State()
{
}

CState_Kuribo_Dead* CState_Kuribo_Dead::Create(CFsm* pFsm, CMonster* pOwner, _uint iStateNum)
{
    CState_Kuribo_Dead* pInstance = new CState_Kuribo_Dead(pFsm, pOwner);

    if (FAILED(pInstance->Initialize(iStateNum)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Kuribo_Dead"));
        Safe_Release(pInstance);
    }

    return pInstance;
}
void CState_Kuribo_Dead::Free()
{
    __super::Free();
}

