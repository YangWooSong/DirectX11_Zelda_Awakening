#include "stdafx.h"
#include "State_Togezo_Damage.h"
#include "GameInstance.h"
#include "Model.h"
#include "Togezo.h"

CState_Togezo_Damage::CState_Togezo_Damage(CFsm* pFsm, CMonster* pOwner)
    :CState{ pFsm }
    , m_pOwner{ pOwner }
{
}

HRESULT CState_Togezo_Damage::Initialize(_uint iStateNum)
{
    m_iCurrentAnimIndex = m_pOwner->Get_Model()->Get_AnimationIndex("damage");
    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_Togezo_Damage::Start_State()
{
    m_pOwner->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.1f);
  //  m_pOwner->Set_AnimationSpeed(m_iCurrentAnimIndex, 40.f);
    return S_OK;
}

void CState_Togezo_Damage::Update(_float fTimeDelta)
{
    if (m_pOwner->Get_IsEnd_CurrentAnimation())
    {
        m_pOwner->Change_State(CTogezo::REBOUND);
    }

}

void CState_Togezo_Damage::End_State()
{
}

CState_Togezo_Damage* CState_Togezo_Damage::Create(CFsm* pFsm, CMonster* pOwner, _uint iStateNum)
{
    CState_Togezo_Damage* pInstance = new CState_Togezo_Damage(pFsm, pOwner);

    if (FAILED(pInstance->Initialize(iStateNum)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Togezo_Damage"));
        Safe_Release(pInstance);
    }

    return pInstance;
}
void CState_Togezo_Damage::Free()
{
    __super::Free();
}

