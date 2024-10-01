#include "stdafx.h"
#include "DeguTail_04.h"
#include "GameInstance.h"
#include "Monster.h"
#include "DeguTail_01.h"
#include "DeguTail_00.h"

CDeguTail_04::CDeguTail_04(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CPartObject(pDevice, pContext)
{
}

CDeguTail_04::CDeguTail_04(const CDeguTail_04& Prototype)
    :CPartObject(Prototype)
{
}

HRESULT CDeguTail_04::Initialize_Prototype()
{
    return S_OK;
}


HRESULT CDeguTail_04::Initialize(void* pArg)
{
    DEGUTAIL_DESC* pDesc = static_cast<DEGUTAIL_DESC*>(pArg);

    /* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    m_pTransformCom->Set_Scaled(pDesc->vSize.x, pDesc->vSize.y, pDesc->vSize.z);
    m_iCellNum = pDesc->iCellNum;
    m_pParent = pDesc->pParent;
    m_fSize = pDesc->vSize;
    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_pHeadFsm = pDesc->pFsm;
    Safe_AddRef(m_pHeadFsm);

    m_iWalkAnimIndex = m_pModelCom->Get_AnimationIndex("wait_move03");
    m_iGuardAnimIndex = m_pModelCom->Get_AnimationIndex("guard03");
    m_iHurtAnimIndex = m_pModelCom->Get_AnimationIndex("damage03");
    m_iDeadAnimIndex = m_pModelCom->Get_AnimationIndex("dead03");
    m_iAppearAnimIndex = m_pModelCom->Get_AnimationIndex("demo_pop");

    m_iCurrentAnimIndex = m_iAppearAnimIndex;
    m_pModelCom->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.1f);
    m_pModelCom->Set_AnimationSpeed(m_iCurrentAnimIndex, 60.f);
    m_bRender = false;

    Set_LayerTag(TEXT("Layer_DeguTail_04"));
    m_eObjType = CGameObject::ANIM_MONSTER;
    return S_OK;
}

void CDeguTail_04::Priority_Update(_float fTimeDelta)
{
    if (m_isDead)
    {
        m_pColliderCom->Set_IsActive(false);
        m_bRender = false;
    }
}

void CDeguTail_04::Update(_float fTimeDelta)
{
    Set_Animation();

    m_pParentWorldMatrixVector = static_cast<CDeguTail_01*>(m_pParent)->Get_Parent_WorlMatrix_Vector();

    m_pModelCom->Play_Animation(fTimeDelta);

    if (m_pNavigationCom != nullptr)
        m_pNavigationCom->SetUp_OnCell(m_pTransformCom, 0.5f, fTimeDelta);

    m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_Ptr());
    //////////////////////////////////////////////////////////////////////

    if (m_pHeadFsm->Get_CurrentState() != CDeguTail_00::DEAD && m_pHeadFsm->Get_CurrentState() != CDeguTail_00::GUARD && m_pHeadFsm->Get_CurrentState() != CDeguTail_00::APPEAR)
    {
        if (m_pParentWorldMatrixVector.size() > 0)
        {
            m_fTimer += fTimeDelta;
        }
        else
        {
            if (m_pHeadFsm->Get_CurrentState() == CDeguTail_00::HURT)
                m_bRender = false;
        }

        if (m_fTimer > 0.2f)
        {
            if (m_pParentWorldMatrixVector.size() > 0)
            {
                m_pTransformCom->Set_WorldMatrix(m_pParentWorldMatrixVector.back());
                m_pTransformCom->Set_Scaled(m_fSize.x, m_fSize.y, m_fSize.z);
                static_cast<CDeguTail_01*>(m_pParent)->Vec_PopBackp();
            }
            else
            {
                m_fTimer = 0.f;
            }

        }
    }

}

void CDeguTail_04::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);

    m_pGameInstance->Add_ColliderList(m_pColliderCom);
    m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CDeguTail_04::Render()
{
    if(m_bRender)
    {
        if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
            return E_FAIL;
        if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
            return E_FAIL;

        _uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

        for (size_t i = 0; i < iNumMeshes; i++)
        {
            m_pModelCom->Bind_MeshBoneMatrices(m_pShaderCom, "g_BoneMatrices", (_uint)i);

            if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", TEXTURE_TYPE::DIFFUSE, (_uint)i)))
                return E_FAIL;


            if (FAILED(m_pShaderCom->Begin(0)))
                return E_FAIL;

            if (FAILED(m_pModelCom->Render((_uint)i)))
                return E_FAIL;
        }

#ifdef _DEBUG
        m_pColliderCom->Render();
#endif
    }
    return S_OK;
}

void CDeguTail_04::OnCollisionEnter(CGameObject* pOther)
{
    if (m_pColliderCom->Get_IsColl())
    {
        if (pOther->Get_LayerTag() == TEXT("Layer_Sword"))
        {
            //두번 공격당하는거 방지
            if(m_pHeadFsm->Get_CurrentState() != CDeguTail_00::HURT)
                m_pHeadFsm->Change_State(CDeguTail_00::HURT);
        }
    }
}

void CDeguTail_04::OnCollisionStay(CGameObject* pOther)
{
}

void CDeguTail_04::OnCollisionExit(CGameObject* pOther)
{
}


HRESULT CDeguTail_04::Ready_Components()
{
    /* FOR.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* FOR.Com_Model */
    if (FAILED(__super::Add_Component(LEVEL_DUNGEON, TEXT("Prototype_Component_Model_DeguTail03"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    /* For.Com_Navigation */
    CNavigation::NAVIGATION_DESC			NaviDesc{};

    NaviDesc.iCurrentIndex = m_iCellNum;
    NaviDesc.iOwnerType = CNavigation::NONPLAYER;

    if (FAILED(__super::Add_Component(LEVEL_DUNGEON, TEXT("Prototype_Component_Navigation"),
        TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &NaviDesc)))
        return E_FAIL;

    /* For.Com_Collider */
    CBounding_AABB::BOUNDING_AABB_DESC			ColliderDesc{};
    ColliderDesc.vExtents = _float3(0.5f, 0.7f, 0.5f);
    ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vExtents.y, 0.f);

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
        TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
        return E_FAIL;
    m_pColliderCom->Set_Owner(this);

    return S_OK;
}

void CDeguTail_04::Set_Animation()
{
    if (m_pHeadFsm->Get_CurrentState() == CDeguTail_00::WALK && m_iCurrentAnimIndex != m_iWalkAnimIndex)
    {
        m_bRender = true;
        m_iCurrentAnimIndex = m_iWalkAnimIndex;
        m_pModelCom->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.1f, true);
        m_pModelCom->Set_AnimationSpeed(m_iCurrentAnimIndex, 50.f);
    }

    if (m_pHeadFsm->Get_CurrentState() == CDeguTail_00::HURT && m_iCurrentAnimIndex != m_iHurtAnimIndex)
    {
        m_iCurrentAnimIndex = m_iHurtAnimIndex;
        m_pModelCom->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.1f, true);
        m_pModelCom->Set_AnimationSpeed(m_iCurrentAnimIndex, 50.f);
    }
    else if (m_iCurrentAnimIndex == m_iHurtAnimIndex && m_pModelCom->Get_IsEnd_CurrentAnimation())
    {
        m_iCurrentAnimIndex = m_iWalkAnimIndex;
        m_pModelCom->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.1f, true);
        m_pModelCom->Set_AnimationSpeed(m_iCurrentAnimIndex, 40.f);
    }

    if (m_pHeadFsm->Get_CurrentState() == CDeguTail_00::GUARD && m_iCurrentAnimIndex != m_iGuardAnimIndex)
    {
        m_iCurrentAnimIndex = m_iGuardAnimIndex;
        m_pModelCom->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.2f);
        m_pModelCom->Set_AnimationSpeed(m_iCurrentAnimIndex, 60.f);
    }

    if (m_pHeadFsm->Get_CurrentState() == CDeguTail_00::DEAD && m_iCurrentAnimIndex != m_iDeadAnimIndex)
    {
        m_iCurrentAnimIndex = m_iDeadAnimIndex;
        m_pModelCom->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.1f, true);
        m_pModelCom->Set_AnimationSpeed(m_iCurrentAnimIndex, 40.f);
    }
}

void CDeguTail_04::Set_Parent(CGameObject* pParent)
{
    m_pParent = static_cast<CMonster*>(pParent);
}

CDeguTail_04* CDeguTail_04::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CDeguTail_04* pInstance = new CDeguTail_04(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CDeguTail_04"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CDeguTail_04::Clone(void* pArg)
{
    CDeguTail_04* pInstance = new CDeguTail_04(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CDeguTail_04"));
        Safe_Release(pInstance);
    }

    return pInstance;
}


void CDeguTail_04::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModelCom);
    Safe_Release(m_pNavigationCom);
    Safe_Release(m_pColliderCom);
    Safe_Release(m_pHeadFsm);
}

