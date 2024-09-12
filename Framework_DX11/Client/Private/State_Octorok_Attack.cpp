#include "stdafx.h"
#include "State_Octorok_Attack.h"
#include "GameInstance.h"
#include "Model.h"
#include "Octorok.h"
#include "OctorokRock.h"

CState_Octorok_Attack::CState_Octorok_Attack(CFsm* pFsm, CMonster* pOwner)
    :CState{ pFsm }
    , m_pOwner{ pOwner }
{
}

HRESULT CState_Octorok_Attack::Initialize(_uint iStateNum)
{
    m_iAttackStartIndex = m_pOwner->Get_Model()->Get_AnimationIndex("attack_st");
    m_iAttackFinIndex = m_pOwner->Get_Model()->Get_AnimationIndex("attack_ed");
    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_Octorok_Attack::Start_State()
{
    m_iCurrentAnimIndex = m_iAttackStartIndex;
    m_pOwner->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.1f);
    m_pOwner->Get_Model()->Set_AnimationSpeed(m_iCurrentAnimIndex, 80.f);
    m_pRock = m_pOwner->Get_PartObject(COctorok::PART_BULLET);
    _float fVoume = m_pOwner->Get_Sound()->Culculate_Volume_Distance(m_pOwner->Get_Pos_vector(), m_pGameInstance->Find_Player(LEVEL_FIELD)->Get_Transform()->Get_State(CTransform::STATE_POSITION), 8.f, 0.8f);
    m_pOwner->Get_Sound()->Play_Sound(TEXT("3_Octarock_Shoot (2).wav"), fVoume);
    return S_OK;
}

void CState_Octorok_Attack::Update(_float fTimeDelta)
{
    if (strcmp(m_pOwner->Get_Model()->Get_CurrentAnimationName(), "attack_st") == 0)
    {
        if (m_pOwner->Get_Model()->Get_CurrentTrackPosition() > 10 )
        {
            Shoot();
        }

        if (m_pOwner->Get_IsEnd_CurrentAnimation())
        {
            m_iCurrentAnimIndex = m_iAttackFinIndex;
            m_pOwner->Get_Model()->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.2f);
            m_pOwner->Get_Model()->Set_AnimationSpeed(m_iCurrentAnimIndex, 80.f);
        }
    }


    if (strcmp(m_pOwner->Get_Model()->Get_CurrentAnimationName(), "attack_ed") == 0 && m_pOwner->Get_IsEnd_CurrentAnimation())
    {
        m_fTimer += fTimeDelta;
          
        if(m_fTimer > 1.f && static_cast<COctorok*>(m_pOwner)->Get_Detect() == true)
        {
            m_fTimer = 0.f;
            m_pOwner->Change_State(COctorok::ATTACK);
        }
    }
}

void CState_Octorok_Attack::End_State()
{
    m_pOwner->Get_PartObject(COctorok::PART_BULLET)->Set_Dead(true);
    m_fTimer = 0.f;
    m_iShootCount = 0;
}

void CState_Octorok_Attack::Shoot()
{
    if(m_iShootCount == 0)
    {
       static_cast<COctorokRock*>( m_pRock)->Shoot();
    }
    m_iShootCount++;
}

CState_Octorok_Attack* CState_Octorok_Attack::Create(CFsm* pFsm, CMonster* pOwner, _uint iStateNum)
{
    CState_Octorok_Attack* pInstance = new CState_Octorok_Attack(pFsm, pOwner);

    if (FAILED(pInstance->Initialize(iStateNum)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Octorok_Attack"));
        Safe_Release(pInstance);
    }

    return pInstance;
}
void CState_Octorok_Attack::Free()
{
    __super::Free();
}

