#include "stdafx.h"
#include "State_DeguTail_Walk.h"
#include "GameInstance.h"
#include "Model.h"
#include "DeguTail_00.h"

CState_DeguTail_Walk::CState_DeguTail_Walk(CFsm* pFsm, CMonster* pOwner)
    :CState{ pFsm }
    , m_pOwner{ pOwner }
{
}
HRESULT CState_DeguTail_Walk::Initialize(_uint iStateNum)
{
    m_iCurrentAnimIndex = m_pOwner->Get_Model()->Get_AnimationIndex("wait_move01");
    m_iStateNum = iStateNum;
    m_pOwnerNavigation = m_pOwner->Get_NavigationCom();
    m_pOwnerDegu = static_cast<CDeguTail_00*>(m_pOwner);
    return S_OK;
}

HRESULT CState_DeguTail_Walk::Start_State()
{
    m_pOwner->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.1f, true);
    m_pOwner->Set_AnimationSpeed(m_iCurrentAnimIndex, 50.f);
    m_bAngry = m_pOwnerDegu->Get_Angry();
    m_fSpeed = 5.f;
    return S_OK;
}

void CState_DeguTail_Walk::Update(_float fTimeDelta)
{
    m_pOwnerDegu->Add_Vec_Matrix();

    if (m_bReflect == true)
    {
        m_fTimer = 0.f;
        m_bReflect = false;
    }

    if (m_bReflect == false)
        m_fTimer += fTimeDelta;

    if (m_iStopCount == m_iRandomStopCount || m_fTimer > 1.f)
    {
        m_iRandomStopCount = (int)m_pGameInstance->Get_Random(2,3);
        m_fTimer = 0.f;
        m_iStopCount = 0;
        m_iReflectDir *= -1.f;
    }
  
    if (m_bAngry)
    {
        m_fAngryTimer += fTimeDelta;
        m_fSpeed = 10.f;
    }

    if (m_fAngryTimer > 3.f)
    {
        if (m_pOwner->Get_Sound()->Get_IsPlaying())
            m_pOwner->Get_Sound()->Stop();
        m_fAngryTimer = 0.f;
        m_bAngry = false;
        m_fSpeed = 5.f;
        m_pOwnerDegu->Set_Angry(false);
    }

    m_pOwner->Get_Transform()->Turn_Lerp(m_pOwner->Get_Transform()->Get_State(CTransform::STATE_RIGHT)* m_iReflectDir, 2.f, fTimeDelta);
 
    m_pOwner->Go_Straight_in_Room_Reverse(fTimeDelta, m_fSpeed, &m_bReflect, m_pOwnerNavigation, &m_iStopCount);
   // m_pOwner->Go_Straight_in_Room(fTimeDelta, m_fSpeed, m_pOwnerNavigation);
}

void CState_DeguTail_Walk::End_State()
{
    m_fAngryTimer = 0.f;
}

CState_DeguTail_Walk* CState_DeguTail_Walk::Create(CFsm* pFsm, CMonster* pOwner, _uint iStateNum)
{
    CState_DeguTail_Walk* pInstance = new CState_DeguTail_Walk(pFsm, pOwner);

    if (FAILED(pInstance->Initialize(iStateNum)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_DeguTail_Walk"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_DeguTail_Walk::Free()
{
    __super::Free();
}
