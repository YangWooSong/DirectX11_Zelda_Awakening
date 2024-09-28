#include "stdafx.h"
#include "State_Togezo_Run.h"
#include "GameInstance.h"
#include "Model.h"
#include "Togezo.h"

CState_Togezo_Run::CState_Togezo_Run(CFsm* pFsm, CMonster* pOwner)
    :CState{ pFsm }
    , m_pOwner{ pOwner }
{
}

HRESULT CState_Togezo_Run::Initialize(_uint iStateNum)
{
    m_iRunStAnimIndex = m_pOwner->Get_Model()->Get_AnimationIndex("run_st");
    m_iRunLpAnimIndex = m_pOwner->Get_Model()->Get_AnimationIndex("run");
    m_iCurrentAnimIndex = m_iRunStAnimIndex;
    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_Togezo_Run::Start_State()
{
    m_pOwner->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.1f);
    m_pOwner->Set_AnimationSpeed(m_iCurrentAnimIndex, 60.f);

    m_iDir = m_pOwner->Get_Monster_Dir();

    return S_OK;
}

void CState_Togezo_Run::Update(_float fTimeDelta)
{
    m_pOwner->Get_Sound()->Set_PlaySpeed(3.f);
    m_pOwner->Get_Sound()->Play_SoundRepeat(TEXT("3_Octarock_Shoot (2).wav"), 0.3f);

    if (strcmp(m_pOwner->Get_Model()->Get_CurrentAnimationName(), "run_st") == 0 && m_pOwner->Get_IsEnd_CurrentAnimation())
    {
        m_iCurrentAnimIndex = m_iRunLpAnimIndex;
        m_pOwner->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.f, true);
        m_pOwner->Set_AnimationSpeed(m_iCurrentAnimIndex, 60.f);
    }

 /*   switch (m_iDir)
    {
    case CMonster::RIGHT:
        m_pOwner->Get_Transform()->Go_World_Right(fTimeDelta, 7.f, m_pOwner->Get_NavigationCom(), &m_bMove);
        break;
    case CMonster::LEFT:
        m_pOwner->Get_Transform()->Go_World_Left(fTimeDelta, 7.f, m_pOwner->Get_NavigationCom(), &m_bMove);
        break;
    case CMonster::FRONT:
        m_pOwner->Get_Transform()->Go_World_Straight(fTimeDelta, 7.f, m_pOwner->Get_NavigationCom(), &m_bMove);
        break;
    case CMonster::BACK:
        m_pOwner->Get_Transform()->Go_World_Backward(fTimeDelta, 7.f, m_pOwner->Get_NavigationCom(), &m_bMove);
        break;
    }*/
   
    m_pOwner->Get_Transform()->Go_Straight(fTimeDelta, 7.f, m_pOwner->Get_NavigationCom(),&m_bMove);

    if (m_bMove == false)
        m_pOwner->Change_State(CTogezo::IDLE);
}

void CState_Togezo_Run::End_State()
{
    m_pOwner->Get_Sound()->Set_PlaySpeed(1.f);
}

CState_Togezo_Run* CState_Togezo_Run::Create(CFsm* pFsm, CMonster* pOwner, _uint iStateNum)
{
    CState_Togezo_Run* pInstance = new CState_Togezo_Run(pFsm, pOwner);

    if (FAILED(pInstance->Initialize(iStateNum)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Togezo_Run"));
        Safe_Release(pInstance);
    }

    return pInstance;
}
void CState_Togezo_Run::Free()
{
    __super::Free();
}

