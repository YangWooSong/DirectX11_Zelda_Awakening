#include "stdafx.h"
#include "State_Kuribo_Walk_Shake.h"
#include "GameInstance.h"
#include "Model.h"
#include "Kuribo.h"

CState_Kuribo_Walk_Shake::CState_Kuribo_Walk_Shake(CFsm* pFsm, CMonster* pOwner)
    :CState{ pFsm }
    , m_pOwner{ pOwner }
{
}

HRESULT CState_Kuribo_Walk_Shake::Initialize(_uint iStateNum)
{
    m_iCurrentAnimIndex = m_pOwner->Get_Model()->Get_AnimationIndex("walk_shake");
    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_Kuribo_Walk_Shake::Start_State()
{
    m_pOwner->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.1f, true);
    m_pOwner->Set_AnimationSpeed(m_iCurrentAnimIndex, 50.f);
    return S_OK;
}

void CState_Kuribo_Walk_Shake::Update(_float fTimeDelta)
{
    if (KEY_AWAY(Z))
    {
        m_pOwner->Change_State(CKuribo::IDLE);
    }
}

void CState_Kuribo_Walk_Shake::End_State()
{
}

CState_Kuribo_Walk_Shake* CState_Kuribo_Walk_Shake::Create(CFsm* pFsm, CMonster* pOwner, _uint iStateNum)
{
    CState_Kuribo_Walk_Shake* pInstance = new CState_Kuribo_Walk_Shake(pFsm, pOwner);

    if (FAILED(pInstance->Initialize(iStateNum)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Kuribo_Walk_Shake"));
        Safe_Release(pInstance);
    }

    return pInstance;
}
void CState_Kuribo_Walk_Shake::Free()
{
    __super::Free();
}

