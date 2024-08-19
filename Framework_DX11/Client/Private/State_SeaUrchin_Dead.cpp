#include "stdafx.h"
#include "State_SeaUrchin_Dead.h"
#include "GameInstance.h"
#include "Model.h"
#include "SeaUrchin.h"

CState_SeaUrchin_Dead::CState_SeaUrchin_Dead(CFsm* pFsm, CMonster* pOwner)
    :CState{ pFsm }
    , m_pOwner{ pOwner }
{
}

HRESULT CState_SeaUrchin_Dead::Initialize(_uint iStateNum)
{
    m_iCurrentAnimIndex = m_pOwner->Get_Model()->Get_AnimationIndex("pushed");
    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_SeaUrchin_Dead::Start_State()
{
    m_pOwner->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.1f, true);
    m_pOwner->Set_AnimationSpeed(m_iCurrentAnimIndex, 90.f);
    m_pOwner->Set_Dead(true);
    return S_OK;
}

void CState_SeaUrchin_Dead::Update(_float fTimeDelta)
{
    if (KEY_AWAY(Z))
    {
        m_pOwner->Change_State(CSeaUrchin::IDLE);
    }
}

void CState_SeaUrchin_Dead::End_State()
{
    m_pOwner->Set_Dead(false);
}

CState_SeaUrchin_Dead* CState_SeaUrchin_Dead::Create(CFsm* pFsm, CMonster* pOwner, _uint iStateNum)
{
    CState_SeaUrchin_Dead* pInstance = new CState_SeaUrchin_Dead(pFsm, pOwner);

    if (FAILED(pInstance->Initialize(iStateNum)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_SeaUrchin_Dead"));
        Safe_Release(pInstance);
    }

    return pInstance;
}
void CState_SeaUrchin_Dead::Free()
{
    __super::Free();
}

