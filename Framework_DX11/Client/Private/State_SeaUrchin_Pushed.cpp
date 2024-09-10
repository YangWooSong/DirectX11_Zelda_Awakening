#include "stdafx.h"
#include "State_SeaUrchin_Pushed.h"
#include "GameInstance.h"
#include "Model.h"
#include "SeaUrchin.h"

CState_SeaUrchin_Pushed::CState_SeaUrchin_Pushed(CFsm* pFsm, CMonster* pOwner)
    :CState{ pFsm }
    , m_pOwner{ pOwner }
{
}

HRESULT CState_SeaUrchin_Pushed::Initialize(_uint iStateNum)
{
    m_iCurrentAnimIndex = m_pOwner->Get_Model()->Get_AnimationIndex("pushed");
    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_SeaUrchin_Pushed::Start_State()
{
    m_pOwner->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.1f, true);
    m_pOwner->Set_AnimationSpeed(m_iCurrentAnimIndex, 50.f);
    return S_OK;
}

void CState_SeaUrchin_Pushed::Update(_float fTimeDelta)
{
    //m_pOwner->Get_Collider()->Get
}

void CState_SeaUrchin_Pushed::End_State()
{
}

CState_SeaUrchin_Pushed* CState_SeaUrchin_Pushed::Create(CFsm* pFsm, CMonster* pOwner, _uint iStateNum)
{
    CState_SeaUrchin_Pushed* pInstance = new CState_SeaUrchin_Pushed(pFsm, pOwner);

    if (FAILED(pInstance->Initialize(iStateNum)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_SeaUrchin_Pushed"));
        Safe_Release(pInstance);
    }

    return pInstance;
}
void CState_SeaUrchin_Pushed::Free()
{
    __super::Free();
}

