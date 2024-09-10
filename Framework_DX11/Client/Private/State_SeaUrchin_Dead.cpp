#include "stdafx.h"
#include "State_SeaUrchin_Dead.h"
#include "GameInstance.h"
#include "Model.h"
#include "SeaUrchin.h"

CState_SeaUrchin_Dead::CState_SeaUrchin_Dead(CFsm* pFsm, CMonster* pOwner)
    :CState{ pFsm }
    , m_pOwner{ pOwner }
{
}

HRESULT CState_SeaUrchin_Dead::Initialize(_uint iStateNum)
{
    m_iCurrentAnimIndex = m_pOwner->Get_Model()->Get_AnimationIndex("pushed");
    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_SeaUrchin_Dead::Start_State()
{
    m_pOwner->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.1f, true);
    m_pOwner->Set_AnimationSpeed(m_iCurrentAnimIndex, 90.f);
    m_pOwner->Get_Collider()->Set_IsActive(false);
    m_pOwner->Set_Dead(true);
    m_pOwner->Get_Sound()->Play_Sound(TEXT("3_Octarock_Damage.wav"), 0.8f);
    m_pOwner->Get_Sound()->Set_PlaySpeed(2.f);
    m_vNewLook = XMVector3Normalize(m_pGameInstance->Find_Player(LEVEL_FIELD)->Get_Transform()->Get_State(CTransform::STATE_POSITION) - m_pOwner->Get_Pos_vector());
    return S_OK;
}

void CState_SeaUrchin_Dead::Update(_float fTimeDelta)
{
    m_fTimer += fTimeDelta;
    if (m_fTimer > 0.4f)
    {
        m_pOwner->Get_Sound()->Set_PlaySpeed(1.f);
        m_pOwner->Get_Sound()->Play_Sound(TEXT("3_Monster_Explosion.wav"), 0.8f);
        m_pOwner->Set_Dead(true);
        m_pOwner->Change_State(CSeaUrchin::IDLE);
        static_cast<CSeaUrchin*>(m_pOwner)->Set_Render(false);
    }

    m_pOwner->Get_Transform()->Turn_Lerp(m_vNewLook, 3.f, fTimeDelta);
    m_pOwner->Get_Transform()->Go_Vector(m_vNewLook, fTimeDelta, -20.f, m_pOwner->Get_NavigationCom());

}

void CState_SeaUrchin_Dead::End_State()
{
}

CState_SeaUrchin_Dead* CState_SeaUrchin_Dead::Create(CFsm* pFsm, CMonster* pOwner, _uint iStateNum)
{
    CState_SeaUrchin_Dead* pInstance = new CState_SeaUrchin_Dead(pFsm, pOwner);

    if (FAILED(pInstance->Initialize(iStateNum)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_SeaUrchin_Dead"));
        Safe_Release(pInstance);
    }

    return pInstance;
}
void CState_SeaUrchin_Dead::Free()
{
    __super::Free();
}

