#include "stdafx.h"
#include "State_Octorok_Idle.h"
#include "GameInstance.h"
#include "Model.h"
#include "Octorok.h"

CState_Octorok_Idle::CState_Octorok_Idle(CFsm* pFsm, CMonster* pOwner)
    :CState{ pFsm }
    , m_pOwner{ pOwner }
{
}

HRESULT CState_Octorok_Idle::Initialize(_uint iStateNum)
{
    m_iCurrentAnimIndex = m_pOwner->Get_Model()->Get_AnimationIndex("wait");
    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_Octorok_Idle::Start_State()
{
    m_pOwner->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.1f, true);
    m_pOwner->Set_AnimationSpeed(m_iCurrentAnimIndex, 40.f);
    m_iOrigin_Dir = m_pOwner->Get_Monster_Dir();
    return S_OK;
}

void CState_Octorok_Idle::Update(_float fTimeDelta)
{
    m_iTimer += fTimeDelta;

    if (m_iTimer > 2.f)
    {
        m_iTimer = 0.f;
        m_pOwner->Change_State(COctorok::WALK);
    }

    if (KEY_AWAY(Z))
    {
        m_pOwner->Change_State(COctorok::ATTACK);
    }
 
}

void CState_Octorok_Idle::End_State()
{

}

CState_Octorok_Idle* CState_Octorok_Idle::Create(CFsm* pFsm, CMonster* pOwner, _uint iStateNum)
{
    CState_Octorok_Idle* pInstance = new CState_Octorok_Idle(pFsm, pOwner);

    if (FAILED(pInstance->Initialize(iStateNum)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Octorok_Idle"));
        Safe_Release(pInstance);
    }

    return pInstance;
}
void CState_Octorok_Idle::Free()
{
    __super::Free();
}

