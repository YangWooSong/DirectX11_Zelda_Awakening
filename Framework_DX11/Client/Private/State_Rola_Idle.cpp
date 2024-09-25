#include "stdafx.h"
#include "State_Rola_Idle.h"
#include "GameInstance.h"
#include "Model.h"
#include "Rola.h"

CState_Rola_Idle::CState_Rola_Idle(CFsm* pFsm, CMonster* pOwner)
    :CState{ pFsm }
    , m_pOwner{ pOwner }
{
}

HRESULT CState_Rola_Idle::Initialize(_uint iStateNum)
{
    m_iCurrentAnimIndex = m_pOwner->Get_Model()->Get_AnimationIndex("wait");
    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_Rola_Idle::Start_State()
{
    m_pOwner->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.1f, true);
    m_pOwner->Set_AnimationSpeed(m_iCurrentAnimIndex, 60.f);
    return S_OK;
}

void CState_Rola_Idle::Update(_float fTimeDelta)
{
    //if (KEY_AWAY(Z))
    //{
    //    m_pOwner->Change_State(CRola::DEMAGE);
    //}
}

void CState_Rola_Idle::End_State()
{
}

CState_Rola_Idle* CState_Rola_Idle::Create(CFsm* pFsm, CMonster* pOwner, _uint iStateNum)
{
    CState_Rola_Idle* pInstance = new CState_Rola_Idle(pFsm, pOwner);

    if (FAILED(pInstance->Initialize(iStateNum)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Rola_Idle"));
        Safe_Release(pInstance);
    }

    return pInstance;
}
void CState_Rola_Idle::Free()
{
    __super::Free();
}

