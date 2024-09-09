#include "stdafx.h"
#include "State_Octorok_Walk.h"
#include "GameInstance.h"
#include "Model.h"
#include "Octorok.h"
#include "Detector.h"


CState_Octorok_Walk::CState_Octorok_Walk(CFsm* pFsm, CMonster* pOwner)
    :CState{ pFsm }
    , m_pOwner{ pOwner }
{
}

HRESULT CState_Octorok_Walk::Initialize(_uint iStateNum)
{
    m_iCurrentAnimIndex = m_pOwner->Get_Model()->Get_AnimationIndex("walk");
    m_iStateNum = iStateNum;
    m_fSpeed = 2.f;
    m_pNavigation = m_pOwner->Get_NavigationCom();
    m_pDetector = static_cast<CDetector*>(m_pOwner->Get_PartObject(COctorok::PART_DETECTOR));
    return S_OK;
}

HRESULT CState_Octorok_Walk::Start_State()
{
    m_pOwner->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.1f, true);
    m_pOwner->Set_AnimationSpeed(m_iCurrentAnimIndex, 80.f);
    m_pOwner->Set_NewRandom_Dir();
    m_iOwnerDir = m_pOwner->Get_Monster_Dir();
 
    return S_OK;
}

void CState_Octorok_Walk::Update(_float fTimeDelta)
{
    //방향 전환
    switch (m_iOwnerDir)
    {
    case COctorok::FRONT:
        //m_pOwner->Get_Transform()->RotationThreeAxis(_float3(0.f, 0.f, 0.f));
        m_fTargetRot = { 0.f, 0.f, 0.f };
        break;
    case COctorok::LEFT:
        //m_pOwner->Get_Transform()->RotationThreeAxis(_float3(0.f, 270.f, 0.f));
        m_fTargetRot = { 0.f, 270.f, 0.f };
        break;
    case COctorok::BACK:
        //m_pOwner->Get_Transform()->RotationThreeAxis(_float3(0.f, 180.f, 0.f));
        m_fTargetRot = { 0.f, 180.f, 0.f };
        break;
    case COctorok::RIGHT:
        //m_pOwner->Get_Transform()->RotationThreeAxis(_float3(0.f, 90.f, 0.f));
        m_fTargetRot = { 0.f, 90, 0.f };
        break;
    default:
        break;
    }

    m_pOwner->Get_Transform()->Turn_Lerp_Angle(m_pOwner->Get_Rotation(), m_fTargetRot, fTimeDelta);
    m_pOwner->Set_Rotation(m_pOwner->Get_Transform()->Get_Rot());

    if (m_pOwner->Get_Transform()->Get_IsTurning() == false)
    {
        m_pDetector->Set_Active_Collider(true);
        m_pOwner->Set_AnimationSpeed(m_iCurrentAnimIndex, 70.f);
        m_pOwner->Go_Straight(fTimeDelta, m_fSpeed, m_pNavigation);

        m_fTimer += fTimeDelta;

        if (m_fTimer > 2.f)
        {
            m_fTimer = 0.f;
            m_pOwner->Change_State(COctorok::IDLE);
        }
    }
    else
        m_pDetector->Set_Active_Collider(false);

    if (KEY_AWAY(Z))
    {
       // m_pOwner->Change_State(COctorok::ATTACK);
        m_pOwner->Change_State(COctorok::IDLE);
    }
}

void CState_Octorok_Walk::End_State()
{
    m_pOwner->Get_Sound()->Stop();
}

CState_Octorok_Walk* CState_Octorok_Walk::Create(CFsm* pFsm, CMonster* pOwner, _uint iStateNum)
{
    CState_Octorok_Walk* pInstance = new CState_Octorok_Walk(pFsm, pOwner);

    if (FAILED(pInstance->Initialize(iStateNum)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Octorok_Walk"));
        Safe_Release(pInstance);
    }

    return pInstance;
}
void CState_Octorok_Walk::Free()
{
    __super::Free();
}

