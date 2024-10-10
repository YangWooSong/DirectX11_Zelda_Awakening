#include "stdafx.h"
#include "NonAnimModel.h"
#include "GameInstance.h"

CNonAnimModel::CNonAnimModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CGameObject(pDevice, pContext)
{
}

CNonAnimModel::CNonAnimModel(const CNonAnimModel& Prototype)
    :CGameObject(Prototype)
{
}

HRESULT CNonAnimModel::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CNonAnimModel::Initialize(void* pArg)
{

    /* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CNonAnimModel::Priority_Update(_float fTimeDelta)
{
}

void CNonAnimModel::Update(_float fTimeDelta)
{
   
}

void CNonAnimModel::Late_Update(_float fTimeDelta)
{
    /* 직교투영을 위한 월드행렬까지 셋팅하게 된다. */
    __super::Late_Update(fTimeDelta);


    m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}
HRESULT CNonAnimModel::Render()
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
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", TEXTURE_TYPE::DIFFUSE, (_uint)i)))
            return E_FAIL;
        /*if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", aiTextureType_NORMALS, i)))
            return E_FAIL;*/

        if (FAILED(m_pShaderCom->Begin(0)))
            return E_FAIL;

        if (FAILED(m_pModelCom->Render((_uint)i)))
            return E_FAIL;
    }

    bool _b = false;
    m_pShaderCom->Bind_RawValue("g_isPicking", &_b, sizeof(bool));

    return S_OK;
}


HRESULT CNonAnimModel::Ready_Components()
{
    /* FOR.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_MARINHOUSE, TEXT("Prototype_Component_Shader_VtxModel"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_MARINHOUSE, TEXT("Prototype_Component_Level_MarinHouse"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    return S_OK;
}



CNonAnimModel* CNonAnimModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CNonAnimModel* pInstance = new CNonAnimModel(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CNonAnimModel"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CNonAnimModel::Clone(void* pArg)
{
    CNonAnimModel* pInstance = new CNonAnimModel(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CNonAnimModel"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CNonAnimModel::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModelCom);
}

