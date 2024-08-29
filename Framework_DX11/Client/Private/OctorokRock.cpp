#include "stdafx.h"
#include "OctorokRock.h"
#include "GameInstance.h"


COctorokRock::COctorokRock(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CGameObject(pDevice, pContext)
{
}

COctorokRock::COctorokRock(const COctorokRock& Prototype)
    :CGameObject(Prototype)
{
}

HRESULT COctorokRock::Initialize_Prototype()
{
    return S_OK;
}

HRESULT COctorokRock::Initialize(void* pArg)
{
    OCTOROKROCK_DESC* pDesc = static_cast<OCTOROKROCK_DESC*>(pArg);

    /* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    m_fOffset = { 0.f, 0.5f, 0.f };
    pDesc->vPosition.y += m_fOffset.y;
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&pDesc->vPosition));
    m_pTransformCom->Set_Scaled(0.8f, 0.8f, 0.8f);
    m_iRockDir = pDesc->iDir;
    m_iCellNum = pDesc->iCellNum;

    if (FAILED(Ready_Components()))
        return E_FAIL;

  
    
    m_fSpeed = 7.f;

    return S_OK;
}

void COctorokRock::Priority_Update(_float fTimeDelta)
{
}

void COctorokRock::Update(_float fTimeDelta)
{
    switch (m_iRockDir)
    {
    case FRONT:
        m_pTransformCom->Go_World_Straight(fTimeDelta, m_fSpeed,m_pNavigationCom);
        break;
    case BACK:
        m_pTransformCom->Go_World_Backward(fTimeDelta, m_fSpeed, m_pNavigationCom);
        break;
    case LEFT:
        m_pTransformCom->Go_World_Left(fTimeDelta, m_fSpeed, m_pNavigationCom);
        break;
    case RIGHT:
        m_pTransformCom->Go_World_Right(fTimeDelta, m_fSpeed, m_pNavigationCom);
        break;
    default:
        break;
    }
  
}

void COctorokRock::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);
    m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT COctorokRock::Render()
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

    return S_OK;
}

HRESULT COctorokRock::Ready_Components()
{
    /* FOR.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxModel"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* FOR.Com_Model */
    if (FAILED(__super::Add_Component(LEVEL_FIELD, TEXT("Prototype_Component_Model_OctorokRock"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    /* For.Com_Navigation */
    CNavigation::NAVIGATION_DESC			NaviDesc{};

    NaviDesc.iCurrentIndex = m_iCellNum;

    if (FAILED(__super::Add_Component(LEVEL_FIELD, TEXT("Prototype_Component_Navigation"),
        TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &NaviDesc)))
        return E_FAIL;

    return S_OK;
}

COctorokRock* COctorokRock::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    COctorokRock* pInstance = new COctorokRock(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : COctorokRock"));
        Safe_Release(pInstance);
    }

    return pInstance;
}
CGameObject* COctorokRock::Clone(void* pArg)
{
    COctorokRock* pInstance = new COctorokRock(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : COctorokRock"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void COctorokRock::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModelCom);
    Safe_Release(m_pNavigationCom);
}
