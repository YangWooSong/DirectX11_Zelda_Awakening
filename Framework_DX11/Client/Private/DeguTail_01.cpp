#include "stdafx.h"
#include "DeguTail_01.h"
#include "GameInstance.h"
#include "Monster.h"
#include "DeguTail_00.h"

CDeguTail_01::CDeguTail_01(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CPartObject(pDevice, pContext)
{
}

CDeguTail_01::CDeguTail_01(const CDeguTail_01& Prototype)
    :CPartObject(Prototype)
{
}

HRESULT CDeguTail_01::Initialize_Prototype()
{
    return S_OK;
}


HRESULT CDeguTail_01::Initialize(void* pArg)
{
    DEGUBODY_DESC* pDesc = static_cast<DEGUBODY_DESC*>(pArg);

    /* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    m_pTransformCom->Set_Scaled(pDesc->vSize.x, pDesc->vSize.y, pDesc->vSize.z);
    m_iCellNum = pDesc->iCellNum;
    m_pParent = pDesc->pParent;
    m_fSize = pDesc->vSize;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_pParentWorldMatrixVector = static_cast<CDeguTail_00*>(m_pParent)->Get_Parent_WorlMatrix_Vector();
    return S_OK;
}

void CDeguTail_01::Priority_Update(_float fTimeDelta)
{
}

void CDeguTail_01::Update(_float fTimeDelta)
{
    if (m_pNavigationCom != nullptr)
        m_pNavigationCom->SetUp_OnCell(m_pTransformCom, 0.5f, fTimeDelta);

    m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_Ptr());
/////////////////////////////////////////


    if (m_pParent->Get_Fsm()->Get_CurrentState() == CDeguTail_00::WALK)
    {
        m_fTimer += fTimeDelta;
    }

    if (m_fTimer > 0.3f )
    {
        if (m_pParentWorldMatrixVector->size() > 0)
        {
            m_pTransformCom->Set_WorldMatrix(m_pParentWorldMatrixVector->back());
            m_pTransformCom->Set_Scaled(m_fSize.x, m_fSize.y, m_fSize.z);
            m_pParentWorldMatrixVector->pop_back();
        }
        else
            m_fTimer = 0.f;
      //  m_bMove = true;
    }
    
}

void CDeguTail_01::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);
    m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CDeguTail_01::Render()
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
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", TEXTURE_TYPE::DIFFUSE, i)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(0)))
            return E_FAIL;

        if (FAILED(m_pModelCom->Render(i)))
            return E_FAIL;
    }
#ifdef _DEBUG
    m_pColliderCom->Render();
#endif
    return S_OK;
}


HRESULT CDeguTail_01::Ready_Components()
{
    /* FOR.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* FOR.Com_Model */
    if (FAILED(__super::Add_Component(LEVEL_DUNGEON, TEXT("Prototype_Component_Model_DeguTail02"),
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
    ColliderDesc.vExtents = _float3(0.6f, 0.7f, 0.6f);
    ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vExtents.y, 0.f);

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
        TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
        return E_FAIL;

    return S_OK;
}

void CDeguTail_01::Set_Parent(CGameObject* pParent)
{
    m_pParent = static_cast<CMonster*>(pParent);
}

CDeguTail_01* CDeguTail_01::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CDeguTail_01* pInstance = new CDeguTail_01(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CDeguTail_01"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CDeguTail_01::Clone(void* pArg)
{
    CDeguTail_01* pInstance = new CDeguTail_01(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CDeguTail_01"));
        Safe_Release(pInstance);
    }

    return pInstance;
}


void CDeguTail_01::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModelCom);
    Safe_Release(m_pNavigationCom);
    Safe_Release(m_pColliderCom);
}

