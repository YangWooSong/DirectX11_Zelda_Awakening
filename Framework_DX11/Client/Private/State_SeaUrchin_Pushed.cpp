#include "stdafx.h"
#include "State_SeaUrchin_Pushed.h"
#include "GameInstance.h"
#include "Model.h"
#include "SeaUrchin.h"
#include "Player.h"

CState_SeaUrchin_Pushed::CState_SeaUrchin_Pushed(CFsm* pFsm, CMonster* pOwner)
    :CState{ pFsm }
    , m_pOwner{ pOwner }
{
}

HRESULT CState_SeaUrchin_Pushed::Initialize(_uint iStateNum)
{
    m_iCurrentAnimIndex = m_pOwner->Get_Model()->Get_AnimationIndex("pushed");
    m_iStateNum = iStateNum;
    m_pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Player(LEVEL_FIELD));
    return S_OK;
}

HRESULT CState_SeaUrchin_Pushed::Start_State()
{
    m_pOwner->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.1f, true);
    m_pOwner->Set_AnimationSpeed(m_iCurrentAnimIndex, 50.f);
    m_pOwner->Get_Sound()->Play_Sound(TEXT("3_SeaUrchin_Pushed.wav"), 1.f);
    return S_OK;
}

void CState_SeaUrchin_Pushed::Update(_float fTimeDelta)
{
   
    _uint iPlayer_Dir = m_pPlayer->Get_Player_Dir();
    _vector vPlayerPos = m_pPlayer->Get_Transform()->Get_State(CTransform::STATE_POSITION);

    _float fCurrZ = XMVectorGetZ(m_pOwner->Get_Pos_vector());

    //위, 아래에서 미는 경우
    //범위는 현재 좌표 +- 콜라이더 크기
    if (fCurrZ + 0.6f < XMVectorGetZ(vPlayerPos) || fCurrZ - 0.6f > XMVectorGetZ(vPlayerPos))
        MoveUpDown(iPlayer_Dir, fTimeDelta);
    else
        MoveRightLeft(iPlayer_Dir, fTimeDelta);
}

void CState_SeaUrchin_Pushed::End_State()
{
    m_pOwner->Get_Sound()->Stop();
}

void CState_SeaUrchin_Pushed::MoveUpDown(_uint iPlayer_Dir, _float fTimeDelta)
{
    if (iPlayer_Dir == CPlayer::FRONT || iPlayer_Dir == CPlayer::LEFTUFRONT || iPlayer_Dir == CPlayer::RIGHTFRONT)
        m_pOwner->Get_Transform()->Go_World_Straight(fTimeDelta, m_fSpeed, m_pOwner->Get_NavigationCom());
    else if(iPlayer_Dir == CPlayer::BACK || iPlayer_Dir == CPlayer::LEFTBACK || iPlayer_Dir == CPlayer::RIGHTBACK)
        m_pOwner->Get_Transform()->Go_World_Backward(fTimeDelta, m_fSpeed, m_pOwner->Get_NavigationCom());
}

void CState_SeaUrchin_Pushed::MoveRightLeft(_uint iPlayer_Dir, _float fTimeDelta)
{
    if (iPlayer_Dir == CPlayer::LEFT || iPlayer_Dir == CPlayer::LEFTUFRONT || iPlayer_Dir == CPlayer::LEFTBACK)
        m_pOwner->Get_Transform()->Go_World_Left(fTimeDelta, m_fSpeed, m_pOwner->Get_NavigationCom());
    else if (iPlayer_Dir == CPlayer::RIGHT || iPlayer_Dir == CPlayer::RIGHTFRONT || iPlayer_Dir == CPlayer::RIGHTBACK)
        m_pOwner->Get_Transform()->Go_World_Right(fTimeDelta, m_fSpeed, m_pOwner->Get_NavigationCom());
}

CState_SeaUrchin_Pushed* CState_SeaUrchin_Pushed::Create(CFsm* pFsm, CMonster* pOwner, _uint iStateNum)
{
    CState_SeaUrchin_Pushed* pInstance = new CState_SeaUrchin_Pushed(pFsm, pOwner);

    if (FAILED(pInstance->Initialize(iStateNum)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_SeaUrchin_Pushed"));
        Safe_Release(pInstance);
    }

    return pInstance;
}
void CState_SeaUrchin_Pushed::Free()
{
    __super::Free();
}

