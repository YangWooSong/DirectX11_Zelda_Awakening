#include "stdafx.h"
#include "State_SeaUrchin_Idle.h"
#include "GameInstance.h"
#include "Model.h"
#include "SeaUrchin.h"

CState_SeaUrchin_Idle::CState_SeaUrchin_Idle(CFsm* pFsm, CMonster* pOwner)
    :CState{ pFsm }
    , m_pOwner{ pOwner }
{
}

HRESULT CState_SeaUrchin_Idle::Initialize(_uint iStateNum)
{
    m_iCurrentAnimIndex = m_pOwner->Get_Model()->Get_AnimationIndex("wait");
    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_SeaUrchin_Idle::Start_State()
{
    m_pOwner->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.1f, true);
    m_pOwner->Set_AnimationSpeed(m_iCurrentAnimIndex, 40.f);
    return S_OK;
}

void CState_SeaUrchin_Idle::Update(_float fTimeDelta)
{
    if (KEY_AWAY(Z))
    {
        m_pOwner->Change_State(CSeaUrchin::PUSHED);
    }
}

void CState_SeaUrchin_Idle::End_State()
{
}

CState_SeaUrchin_Idle* CState_SeaUrchin_Idle::Create(CFsm* pFsm, CMonster* pOwner, _uint iStateNum)
{
    CState_SeaUrchin_Idle* pInstance = new CState_SeaUrchin_Idle(pFsm, pOwner);

    if (FAILED(pInstance->Initialize(iStateNum)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_SeaUrchin_Idle"));
        Safe_Release(pInstance);
    }

    return pInstance;
}
void CState_SeaUrchin_Idle::Free()
{
    __super::Free();
}

