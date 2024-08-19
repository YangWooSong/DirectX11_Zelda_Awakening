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

    return S_OK;
}

HRESULT CState_Rola_Jump::Start_State()
{
    m_pOwner->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.1f);
    m_pOwner->Set_AnimationSpeed(m_iCurrentAnimIndex, 60.f);
    return S_OK;
}

void CState_Rola_Jump::Update(_float fTimeDelta)
{
    if (strcmp(m_pOwner->Get_Model()->Get_CurrentAnimationName(), "jump_st") == 0 && m_pOwner->Get_IsEnd_CurrentAnimation())
    {
        m_iCurrentAnimIndex = m_iJumpAnimIndex;
        m_pOwner->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.1f);
        m_pOwner->Set_AnimationSpeed(m_iCurrentAnimIndex, 60.f);
    }
    if (strcmp(m_pOwner->Get_Model()->Get_CurrentAnimationName(), "jump") == 0 && m_pOwner->Get_IsEnd_CurrentAnimation())
    {
        m_iCurrentAnimIndex = m_iJumpEdAnimIndex;
        m_pOwner->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.1f);
        m_pOwner->Set_AnimationSpeed(m_iCurrentAnimIndex, 60.f);
    }
    if (strcmp(m_pOwner->Get_Model()->Get_CurrentAnimationName(), "jump_ed") == 0 && m_pOwner->Get_IsEnd_CurrentAnimation())
    {
        m_pOwner->Change_State(CRola::IDLE);
    }

}

void CState_Rola_Jump::End_State()
{
    m_iCurrentAnimIndex = m_iJumpStAnimIndex;
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

