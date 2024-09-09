#include "stdafx.h"
#include "State_DeguTail_Guard.h"
#include "GameInstance.h"
#include "Model.h"
#include "DeguTail_00.h"

CState_DeguTail_Guard::CState_DeguTail_Guard(CFsm* pFsm, CMonster* pOwner)
    :CState{ pFsm }
    , m_pOwner{ pOwner }
{
}
HRESULT CState_DeguTail_Guard::Initialize(_uint iStateNum)
{
    m_iCurrentAnimIndex = m_pOwner->Get_Model()->Get_AnimationIndex("guard01");
    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_DeguTail_Guard::Start_State()
{
    m_pOwner->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.2f);
    m_pOwner->Set_AnimationSpeed(m_iCurrentAnimIndex, 60.f);
    m_pOwner->Get_Sound()->Play_Sound(TEXT("2_DeguTail_Guard.wav"), 0.6f);
    return S_OK;
}

void CState_DeguTail_Guard::Update(_float fTimeDelta)
{
    if (m_pOwner->Get_IsEnd_CurrentAnimation())
    {
        m_pOwner->Change_State(CDeguTail_00::WALK);
    }
}

void CState_DeguTail_Guard::End_State()
{
}

CState_DeguTail_Guard* CState_DeguTail_Guard::Create(CFsm* pFsm, CMonster* pOwner, _uint iStateNum)
{
    CState_DeguTail_Guard* pInstance = new CState_DeguTail_Guard(pFsm, pOwner);

    if (FAILED(pInstance->Initialize(iStateNum)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_DeguTail_Guard"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_DeguTail_Guard::Free()
{
    __super::Free();
}
