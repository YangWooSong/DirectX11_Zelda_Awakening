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
    m_pOwner->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.1f);
    m_pOwner->Set_AnimationSpeed(m_iCurrentAnimIndex, 60.f);

    //Set_TargetPos();
    return S_OK;
}

void CState_Rola_Jump::Update(_float fTimeDelta)
{
    //Anim 연결 세팅
    if (strcmp(m_pOwner->Get_Model()->Get_CurrentAnimationName(), "jump_st") == 0 && m_pOwner->Get_IsEnd_CurrentAnimation())
    {
        m_iCurrentAnimIndex = m_iJumpAnimIndex;
        m_pOwner->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.1f);
        m_pOwner->Set_AnimationSpeed(m_iCurrentAnimIndex, 40.f);
    }
  /*  if (strcmp(m_pOwner->Get_Model()->Get_CurrentAnimationName(), "jump") == 0 && m_pOwner->Get_IsEnd_CurrentAnimation())
    {
       
    }*/
    if (strcmp(m_pOwner->Get_Model()->Get_CurrentAnimationName(), "jump_ed") == 0 && m_pOwner->Get_IsEnd_CurrentAnimation())
    {
        if (*m_pTargetIndex == 0 || *m_pTargetIndex == 4)
        {
            *m_pAddDir *= -1;
            m_pOwner->Change_State(CRola::PUSH);
        }

        Set_TargetPos();

        m_pOwner->Change_State(CRola::JUMP);
    }


    if (fabs(m_pOwner->Get_Pos().x - m_vTargetPos[*m_pTargetIndex].x) < 0.1
        && fabs(m_pOwner->Get_Pos().y - m_vTargetPos[*m_pTargetIndex].y) < 0.1
        && fabs(m_pOwner->Get_Pos().z - m_vTargetPos[*m_pTargetIndex].z) < 0.1)
    {
        if(m_iCurrentAnimIndex == m_iJumpAnimIndex)
        {
            m_iCurrentAnimIndex = m_iJumpEdAnimIndex;
            m_pOwner->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.1f);
            m_pOwner->Set_AnimationSpeed(m_iCurrentAnimIndex, 60.f);
        }
 
    }

    if(strcmp(m_pOwner->Get_Model()->Get_CurrentAnimationName(), "jump") == 0)
    {
        _float3 vTarget = m_vTargetPos[*m_pTargetIndex];
        _float fLerpSpeed = min(1.f, fTimeDelta * 4.f);
        _vector vChangePos = XMVectorLerp(m_pOwner->Get_Pos_vector(), XMLoadFloat3(&vTarget), fLerpSpeed);
        m_pOwner->Get_Transform()->Set_State(CTransform::STATE_POSITION, vChangePos);
    }

}

void CState_Rola_Jump::End_State()
{
  
}

void CState_Rola_Jump::Set_TargetPos()
{
    

    *m_pTargetIndex += *m_pAddDir;
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

