#include "stdafx.h"
#include "State_Rola_Dead.h"
#include "GameInstance.h"
#include "Model.h"
#include "Rola.h"
#include "PlayerCamera.h"

CState_Rola_Dead::CState_Rola_Dead(CFsm* pFsm, CMonster* pOwner)
    :CState{ pFsm }
    , m_pOwner{ pOwner }
{
}

HRESULT CState_Rola_Dead::Initialize(_uint iStateNum)
{
    m_iCurrentAnimIndex = m_pOwner->Get_Model()->Get_AnimationIndex("dead");
    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_Rola_Dead::Start_State()
{
    m_pGameInstance->Pause_BGM();
    m_pOwner->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.1f, true);
    m_pOwner->Set_AnimationSpeed(m_iCurrentAnimIndex, 70.f);
    static_cast<CPlayerCamera*>(m_pGameInstance->Find_Camera(LEVEL_DUNGEON))->Zoom_In(1.1f, 60.f);
    return S_OK;
}

void CState_Rola_Dead::Update(_float fTimeDelta)
{

    if (m_fTimer > 3.f)
    {
        if(m_bPlaySound==false)
        {
            m_bPlaySound = true;
            m_pOwner->Get_Sound()->Play_Sound(TEXT("3_Monster_Rola_Dead.wav"), 0.8f);
        }

        m_pOwner->Set_Dead(true);
        m_pOwner->SetActive(false);
    }

    m_fTimer += fTimeDelta;

}

void CState_Rola_Dead::End_State()
{
    m_fTimer = 0.f;
    m_bPlaySound = false;
   
}

CState_Rola_Dead* CState_Rola_Dead::Create(CFsm* pFsm, CMonster* pOwner, _uint iStateNum)
{
    CState_Rola_Dead* pInstance = new CState_Rola_Dead(pFsm, pOwner);

    if (FAILED(pInstance->Initialize(iStateNum)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Rola_Dead"));
        Safe_Release(pInstance);
    }

    return pInstance;
}
void CState_Rola_Dead::Free()
{
    __super::Free();
}

