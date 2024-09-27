#include "stdafx.h"
#include "State_Rola_Jump.h"
#include "GameInstance.h"
#include "Model.h"
#include "Rola.h"

CState_Rola_Jump::CState_Rola_Jump(CFsm* pFsm, CMonster* pOwner)
    :CState{ pFsm }
    , m_pOwner{ pOwner }
{
}

HRESULT CState_Rola_Jump::Initialize(_uint iStateNum)
{
    m_iJumpStAnimIndex = m_pOwner->Get_Model()->Get_AnimationIndex("jump_st");
    m_iJumpAnimIndex = m_pOwner->Get_Model()->Get_AnimationIndex("jump");
    m_iJumpEdAnimIndex = m_pOwner->Get_Model()->Get_AnimationIndex("jump_ed");
    m_iCurrentAnimIndex = m_iJumpStAnimIndex;
    m_iStateNum = iStateNum;

    m_pAddDir = static_cast<CRola*>(m_pOwner)->Get_AddDir();
    m_pTargetIndex = static_cast<CRola*>(m_pOwner)->Get_TargetPosIndex();

    return S_OK;
}

HRESULT CState_Rola_Jump::Start_State()
{
    m_iCurrentAnimIndex = m_iJumpStAnimIndex;
    m_pOwner->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.f);
    m_pOwner->Set_AnimationSpeed(m_iCurrentAnimIndex, 60.f);

    m_bJumpEnd = false;


   Set_TargetPos();
    return S_OK;
}

void CState_Rola_Jump::Update(_float fTimeDelta)
{
    //타겟 위치 바라보기
    m_fTurnTimer += fTimeDelta;
    if(m_fTurnTimer < 0.3f)
        m_pOwner->Get_Transform()->Turn_Lerp(m_vTargetLook, 5.f, fTimeDelta);

    //Anim 연결 세팅
    if (strcmp(m_pOwner->Get_Model()->Get_CurrentAnimationName(), "jump_st") == 0 && m_pOwner->Get_IsEnd_CurrentAnimation())
    {
        m_iCurrentAnimIndex = m_iJumpAnimIndex;
        m_pOwner->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.1f);
        m_pOwner->Set_AnimationSpeed(m_iCurrentAnimIndex, 40.f);
    }
    if (strcmp(m_pOwner->Get_Model()->Get_CurrentAnimationName(), "jump") == 0 && m_fLerpSpeed > 0.5f)
    {
        m_iCurrentAnimIndex = m_iJumpEdAnimIndex;
        m_pOwner->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.05f);
        m_pOwner->Set_AnimationSpeed(m_iCurrentAnimIndex, 60.f);
        *static_cast<CRola*>(m_pOwner)->Get_Arrived() = true;

        if(m_bJumpSound == false)
        {
            m_bJumpSound = true;
            m_pOwner->Get_Sound()->Play_Sound(TEXT("3_Monster_Rola_Jump_End.wav"), 0.8f);
        }
    }
    if (strcmp(m_pOwner->Get_Model()->Get_CurrentAnimationName(), "jump_ed") == 0 && m_pOwner->Get_IsEnd_CurrentAnimation() && m_bJumpEnd == false)
    {
        m_bJumpEnd = true;
        if (*m_pTargetIndex == 0 || *m_pTargetIndex == 4)
        {
           // m_baArrived = true;
            m_pOwner->Change_State(CRola::PUSH);
        }
        else
        {
            m_pOwner->Change_State(CRola::JUMP);
        }
  
    }

    if(strcmp(m_pOwner->Get_Model()->Get_CurrentAnimationName(), "jump") == 0)
    {
        m_fLerpSpeed = min(1.f, m_fLerpSpeed += fTimeDelta * 0.4f);
        _vector vChangePos = XMVectorLerp(m_pOwner->Get_Pos_vector(), XMLoadFloat3(&m_vTargetPos[*m_pTargetIndex]), m_fLerpSpeed);
        m_pOwner->Get_Transform()->Set_State(CTransform::STATE_POSITION, vChangePos);

    }

}

void CState_Rola_Jump::End_State()
{
    m_fLerpSpeed = 0.f;
    m_fTurnTimer = 0.f;
    if (*m_pTargetIndex == 0 || *m_pTargetIndex == 4)
    {
       /* if(m_baArrived)
        {
            
            m_baArrived = false;
            *m_pAddDir *= -1;
        }*/
    }
    m_bJumpSound = false;

}

void CState_Rola_Jump::Set_TargetPos()
{

    *m_pTargetIndex += *m_pAddDir;

    if (*m_pTargetIndex > 4)
        *m_pTargetIndex = 4;
    if(*m_pTargetIndex < 0)
        *m_pTargetIndex = 0;

    m_vTargetLook = XMVector3Normalize(XMLoadFloat3(&m_vTargetPos[*m_pTargetIndex]) - m_pOwner->Get_Pos_vector());

}

void CState_Rola_Jump::Find_ClosestPos()
{
    _int CloseIndex = 0;
    _float MinDistance = 100.f;

    for (int i = 0; i < 5; ++i)
    {
        if (*m_pAddDir == 1)
        {
            if (i < *m_pTargetIndex)
                continue;
        }
        else
        {
            if (i > *m_pTargetIndex)
                continue;
        }

        _float distance = XMVectorGetX(XMVector3Length((m_pOwner->Get_Pos_vector() - XMLoadFloat3(&m_vTargetPos[i]))));

        if (distance < MinDistance)
        {
            MinDistance = distance;
            CloseIndex = i;
        }
    }
}

CState_Rola_Jump* CState_Rola_Jump::Create(CFsm* pFsm, CMonster* pOwner, _uint iStateNum)
{
    CState_Rola_Jump* pInstance = new CState_Rola_Jump(pFsm, pOwner);

    if (FAILED(pInstance->Initialize(iStateNum)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Rola_Jump"));
        Safe_Release(pInstance);
    }

    return pInstance;
}
void CState_Rola_Jump::Free()
{
    __super::Free();
}

