#include "stdafx.h"
#include "State_Togezo_Discover.h"
#include "GameInstance.h"
#include "Model.h"
#include "Togezo.h"

CState_Togezo_Discover::CState_Togezo_Discover(CFsm* pFsm, CMonster* pOwner)
    :CState{ pFsm }
    , m_pOwner{ pOwner }
{
}

HRESULT CState_Togezo_Discover::Initialize(_uint iStateNum)
{
    m_iCurrentAnimIndex = m_pOwner->Get_Model()->Get_AnimationIndex("discover");
    m_iStateNum = iStateNum;

    m_bDetectHori = static_cast<CTogezo*>(m_pOwner)->Get_bDetectHor();
    m_bDetectVer = static_cast<CTogezo*>(m_pOwner)->Get_bDetectVer();

    return S_OK;
}

HRESULT CState_Togezo_Discover::Start_State()
{
    m_pOwner->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.1f);
    m_pOwner->Set_AnimationSpeed(m_iCurrentAnimIndex, 70.f);
    m_pOwner->Get_Sound()->Play_Sound(TEXT("3_Togezo_Discover.wav"), 1.f);

    Set_Dir();

    return S_OK;
}

void CState_Togezo_Discover::Update(_float fTimeDelta)
{
    //switch (m_pOwner->Get_Monster_Dir())
    //{
    //case CMonster::RIGHT:
    //    m_pOwner->Get_Transform()->Turn_Lerp(_vector{-1.f, 0.f, 0.f}, 5.f, fTimeDelta);
    //    break;
    //case  CMonster::LEFT:
    //    m_pOwner->Get_Transform()->Turn_Lerp(_vector{ 1.f, 0.f, 0.f }, 5.f, fTimeDelta);
    //    break;
    //case  CMonster::FRONT:
    //    m_pOwner->Get_Transform()->Turn_Lerp(_vector{ 0.f, 0.f, -1.f }, 5.f, fTimeDelta);
    //    break;
    //case  CMonster::BACK:
    //    m_pOwner->Get_Transform()->Turn_Lerp(_vector{ 0.f, 0.f, 1.f }, 5.f, fTimeDelta);
    //    break;
    //}

    m_pOwner->Get_Transform()->Turn_Lerp(m_vTarget, 10.f, fTimeDelta);

    if(m_pOwner->Get_IsEnd_CurrentAnimation())
        m_pOwner->Change_State(CTogezo::RUN);
}

void CState_Togezo_Discover::End_State()
{
}

void CState_Togezo_Discover::Set_Dir()
{
    /*if (*m_bDetectHori)
    {
        _vector vPlayerPos = m_pGameInstance->Find_Player(LEVEL_DUNGEON)->Get_Transform()->Get_State(CTransform::STATE_POSITION);
        _float fPlayerX = XMVectorGetX(vPlayerPos);   
        _float fmyX = XMVectorGetX(m_pOwner->Get_Pos_vector());

        if (fmyX < fPlayerX)
            m_pOwner->Set_Monster_Dir(CMonster::RIGHT);
        else
            m_pOwner->Set_Monster_Dir(CMonster::LEFT);
    }
    else if (*m_bDetectVer)
    {
        _vector vPlayerPos = m_pGameInstance->Find_Player(LEVEL_DUNGEON)->Get_Transform()->Get_State(CTransform::STATE_POSITION);
        _float fPlayerZ = XMVectorGetZ(vPlayerPos);
        _float  fmyZ = XMVectorGetZ(m_pOwner->Get_Pos_vector());

        if (fmyZ < fPlayerZ)
            m_pOwner->Set_Monster_Dir(CMonster::FRONT);
        else
            m_pOwner->Set_Monster_Dir(CMonster::BACK);
    }*/

    m_vTarget = XMVector3Normalize(m_pGameInstance->Find_Player(LEVEL_DUNGEON)->Get_Transform()->Get_State(CTransform::STATE_POSITION) - m_pOwner->Get_Pos_vector());
}

CState_Togezo_Discover* CState_Togezo_Discover::Create(CFsm* pFsm, CMonster* pOwner, _uint iStateNum)
{
    CState_Togezo_Discover* pInstance = new CState_Togezo_Discover(pFsm, pOwner);

    if (FAILED(pInstance->Initialize(iStateNum)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Togezo_Discover"));
        Safe_Release(pInstance);
    }

    return pInstance;
}
void CState_Togezo_Discover::Free()
{
    __super::Free();
}

