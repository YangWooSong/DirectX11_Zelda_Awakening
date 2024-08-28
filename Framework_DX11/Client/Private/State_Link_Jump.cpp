#include "stdafx.h"
#include "State_Link_Jump.h"
#include "GameInstance.h"
#include "Link.h"
#include "PlayerCamera.h"

CState_Link_Jump::CState_Link_Jump(CFsm* pFsm, CPlayer* pPlayer)
    :CState{ pFsm }
    , m_pPlayer{ pPlayer }
{
}

HRESULT CState_Link_Jump::Initialize(_uint iStateNum)
{
    m_iJumpAnimIndex = m_pPlayer->Get_Model()->Get_AnimationIndex("jump");
    m_iLandAnimIndex = m_pPlayer->Get_Model()->Get_AnimationIndex("land");
    m_iStateNum = iStateNum;
    m_fMoveSpeed = 1.5f;
    m_pPlayer_Navigation = m_pPlayer->Get_Navigation();
    return S_OK;
}

HRESULT CState_Link_Jump::Start_State()
{
    m_iCurrentAnimIndex = m_iJumpAnimIndex;
    m_pPlayer->Get_Model() -> SetUp_NextAnimation(m_iCurrentAnimIndex, 0.02f);
    m_pPlayer->Get_Model()->Set_AnimationSpeed(m_iCurrentAnimIndex, 35.f);
    m_bJump = true;
    m_bStart = true;
    if(m_pPlayer->Get_LevelIndex()== LEVEL_FIELD)
    {
        CPlayerCamera* pCamera = static_cast<CPlayerCamera*>(m_pGameInstance->Find_Camera(LEVEL_FIELD));
        pCamera->Set_FollowPlayer(false);
    }

    return S_OK;
}

void CState_Link_Jump::Update(_float fTimeDelta)
{

    if (strcmp(m_pPlayer->Get_Model()->Get_CurrentAnimationName(), "jump") == 0 && m_pPlayer->Get_IsEnd_CurrentAnimation())
    {
        m_iCurrentAnimIndex = m_iLandAnimIndex;
        m_pPlayer->Get_Model()->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.2f);
        m_pPlayer->Get_Model()->Set_AnimationSpeed(m_iCurrentAnimIndex, 60.f);
    }

    if (strcmp(m_pPlayer->Get_Model()->Get_CurrentAnimationName(), "land") == 0 && m_pPlayer->Get_IsEnd_CurrentAnimation())
    {
        m_pPlayer->Change_State(CLink::IDLE);
    }

    if (m_bJump)
          Jump();

    m_pPlayer->Go_Straight(fTimeDelta, m_fMoveSpeed, m_pPlayer_Navigation);
}

void CState_Link_Jump::End_State()
{
    m_iCurrentAnimIndex = m_iJumpAnimIndex;
    if (m_pPlayer->Get_LevelIndex() == LEVEL_FIELD)
    {
        CPlayerCamera* pCamera = static_cast<CPlayerCamera*>(m_pGameInstance->Find_Camera(LEVEL_FIELD));
        pCamera->Set_FollowPlayer(true);
    }
}

void CState_Link_Jump::Jump()
{
    _float3 fCurrentPos = {};
    XMStoreFloat3(&fCurrentPos, m_pPlayer->Get_Transform()->Get_State(CTransform::STATE_POSITION));

    //점프 처음 시작할 때 위치 저장
    if (m_bStart == true)
    {
        m_fJumpStartY = fCurrentPos.y;
        m_bStart = false;
    }

    if (m_fJumpStartY + m_fJumpMax < fCurrentPos.y) //점프 최대치 도달시 방향 전환
        m_fJumpAmount = m_fJumpAmount * (-1.f);

    if (m_fJumpStartY > fCurrentPos.y)
    {
        fCurrentPos.y = m_fJumpStartY;
        m_pPlayer->Get_Transform()->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&fCurrentPos));
        m_fJumpAmount = m_fJumpAmount * (-1.f);
        m_bJump = false;
        return;
    }

    fCurrentPos.y += m_fJumpAmount;
    m_pPlayer->Get_Transform()->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&fCurrentPos)); 
}

CState_Link_Jump* CState_Link_Jump::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum)
{
    CState_Link_Jump* pInstance = new CState_Link_Jump(pFsm, pPlayer);

    if (FAILED(pInstance->Initialize(iStateNum)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Link_Jump"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_Link_Jump::Free()
{
    __super::Free();
}
