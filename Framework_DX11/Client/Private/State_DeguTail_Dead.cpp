#include "stdafx.h"
#include "State_DeguTail_Dead.h"
#include "GameInstance.h"
#include "Model.h"
#include "DeguTail_00.h"

CState_DeguTail_Dead::CState_DeguTail_Dead(CFsm* pFsm, CMonster* pOwner)
    :CState{ pFsm }
    , m_pOwner{ pOwner }
{
}
HRESULT CState_DeguTail_Dead::Initialize(_uint iStateNum)
{
    m_iCurrentAnimIndex = m_pOwner->Get_Model()->Get_AnimationIndex("dead01");
    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_DeguTail_Dead::Start_State()
{
    m_pOwner->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.1f, true);
    m_pOwner->Set_AnimationSpeed(m_iCurrentAnimIndex, 40.f);

    m_pGameInstance->Stop_BGM();
    m_pOwner->Get_Sound()->Stop();
    m_pOwner->Get_Sound()->Play_Sound(TEXT("3_Monster_Hit.wav"), 1.f);
    return S_OK;
}

void CState_DeguTail_Dead::Update(_float fTimeDelta)
{
    static_cast<CDeguTail_00*>(m_pOwner)->Kill_Parts(fTimeDelta);
    /*if (KEY_AWAY(E))
        m_pOwner->Change_State(CDeguTail_00::WALK);*/
}

void CState_DeguTail_Dead::End_State()
{
}

CState_DeguTail_Dead* CState_DeguTail_Dead::Create(CFsm* pFsm, CMonster* pOwner, _uint iStateNum)
{
    CState_DeguTail_Dead* pInstance = new CState_DeguTail_Dead(pFsm, pOwner);

    if (FAILED(pInstance->Initialize(iStateNum)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_DeguTail_Dead"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_DeguTail_Dead::Free()
{
    __super::Free();
}
