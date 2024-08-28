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

    //Shoot();

    return S_OK;
}

void CState_Octorok_Attack::Update(_float fTimeDelta)
{
    if (strcmp(m_pOwner->Get_Model()->Get_CurrentAnimationName(), "attack_st") == 0)
    {
        if (m_pOwner->Get_Model()->Get_CurrentTrackPosition() > 10)
            Shoot();

        if (m_pOwner->Get_IsEnd_CurrentAnimation())
        {
            m_iCurrentAnimIndex = m_iAttackFinIndex;
            m_pOwner->Get_Model()->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.2f);
            m_pOwner->Get_Model()->Set_AnimationSpeed(m_iCurrentAnimIndex, 80.f);
        }
    }


    if (strcmp(m_pOwner->Get_Model()->Get_CurrentAnimationName(), "attack_ed") == 0 && m_pOwner->Get_IsEnd_CurrentAnimation())
    {
        m_pOwner->Change_State(COctorok::IDLE);
    }


    if (KEY_AWAY(Z))
    {
        m_pOwner->Change_State(COctorok::DEAD);
    }
}

void CState_Octorok_Attack::End_State()
{
    m_iShootCount = 0;
}

void CState_Octorok_Attack::Shoot()
{
    if(m_iShootCount == 0)
    {
        COctorokRock::OCTOROKROCK_DESC RockDesc = {};
        XMStoreFloat3(&RockDesc.vPosition, m_pOwner->Get_Transform()->Get_State(CTransform::STATE_POSITION));
        RockDesc.iDir = m_pOwner->Get_Monster_Dir();
        RockDesc.iCellNum = m_pOwner->Get_CellNum();
        m_pGameInstance->Event_CreateObject(LEVEL_FIELD, TEXT("Layer_OctorokRock"), TEXT("Prototype_GameObject_OctorokRock"), sizeof(RockDesc), &RockDesc);
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

