#include "stdafx.h"
#include "State_Octorok_Dead.h"
#include "GameInstance.h"
#include "Model.h"
#include "Octorok.h"

CState_Octorok_Dead::CState_Octorok_Dead(CFsm* pFsm, CMonster* pOwner)
    :CState{ pFsm }
    , m_pOwner{ pOwner }
{
}

HRESULT CState_Octorok_Dead::Initialize(_uint iStateNum)
{
    m_iCurrentAnimIndex = m_pOwner->Get_Model()->Get_AnimationIndex("dead");
    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_Octorok_Dead::Start_State()
{
    m_pOwner->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.1f);
    m_pOwner->Set_Dead(true);
    return S_OK;
}

void CState_Octorok_Dead::Update(_float fTimeDelta)
{
    if (KEY_AWAY(Z))
    {
        m_pOwner->Change_State(COctorok::IDLE);
    }
}

void CState_Octorok_Dead::End_State()
{
    m_pOwner->Set_Dead(false);
}

CState_Octorok_Dead* CState_Octorok_Dead::Create(CFsm* pFsm, CMonster* pOwner, _uint iStateNum)
{
    CState_Octorok_Dead* pInstance = new CState_Octorok_Dead(pFsm, pOwner);

    if (FAILED(pInstance->Initialize(iStateNum)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Octorok_Dead"));
        Safe_Release(pInstance);
    }

    return pInstance;
}
void CState_Octorok_Dead::Free()
{
    __super::Free();
}

