#include "stdafx.h"
#include "State_Rola_Demage.h"
#include "GameInstance.h"
#include "Model.h"
#include "Rola.h"
#include "Link.h"
CState_Rola_Demage::CState_Rola_Demage(CFsm* pFsm, CMonster* pOwner)
    :CState{ pFsm }
    , m_pOwner{ pOwner }
{
}

HRESULT CState_Rola_Demage::Initialize(_uint iStateNum)
{
    m_iCurrentAnimIndex = m_pOwner->Get_Model()->Get_AnimationIndex("damage");
    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_Rola_Demage::Start_State()
{
    m_pOwner->Get_Sound()->Play_Sound(TEXT("3_Monster_Rola_Damage (1).wav"), 0.8f);
    m_pOwner->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.1f);
    m_pOwner->Set_AnimationSpeed(m_iCurrentAnimIndex, 60.f);
    m_pOwner->Minus_Hp();
    static_cast<CRola*>(m_pOwner)->Reset_RedCount();
    static_cast<CRola*>(m_pOwner)->Set_Blink(true);

    return S_OK;
}

void CState_Rola_Demage::Update(_float fTimeDelta)
{
    if (m_pOwner->Get_Model()->Get_CurrentTrackPosition() < 10.f)
    {
        _vector vNewVector = XMVector3Normalize(m_pGameInstance->Find_Player(LEVEL_DUNGEON)->Get_Transform()->Get_State(CTransform::STATE_POSITION) - m_pOwner->Get_Pos_vector());
        m_pGameInstance->Find_Player(LEVEL_DUNGEON)->Get_Transform()->Go_Vector(vNewVector, fTimeDelta, 10.f, static_cast<CLink*>(m_pGameInstance->Find_Player(LEVEL_DUNGEON))->Get_Navigation());
    }

    if ( m_pOwner->Get_IsEnd_CurrentAnimation())
    {
        if(m_pOwner->Get_Hp() > 0)
            m_pOwner->Change_State(CRola::JUMP);
        else
            m_pOwner->Change_State(CRola::DEAD);
    }

}

void CState_Rola_Demage::End_State()
{
}

CState_Rola_Demage* CState_Rola_Demage::Create(CFsm* pFsm, CMonster* pOwner, _uint iStateNum)
{
    CState_Rola_Demage* pInstance = new CState_Rola_Demage(pFsm, pOwner);

    if (FAILED(pInstance->Initialize(iStateNum)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Rola_Demage"));
        Safe_Release(pInstance);
    }

    return pInstance;
}
void CState_Rola_Demage::Free()
{
    __super::Free();
}

