#include "stdafx.h"
#include "Tarin.h"
#include "GameInstance.h"

CTarin::CTarin(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CNPC{ pDevice, pContext }
{
}


CTarin::CTarin(const CTarin& Prototype)
    : CNPC{ Prototype }
{
}

HRESULT CTarin::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CTarin::Initialize(void* pArg)
{
    /* 직교투영을 위한 데이터들을 모두 셋하낟. */
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_PartObjects()))
        return E_FAIL;

    m_pTransformCom->RotationThreeAxis(_float3(0.f, -45.f, 0.f));

    m_iAnimIndex =  m_pModelCom->Get_AnimationIndex("eat");
    m_pModelCom->SetUp_NextAnimation(m_iAnimIndex, 0.1f, true);
    m_pModelCom->Set_AnimationSpeed(m_iAnimIndex, 40.f);
    return S_OK;
}

void CTarin::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
}

void CTarin::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);

}

void CTarin::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);
}

HRESULT CTarin::Render()
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

    __super::Render();

    return S_OK;
}



HRESULT CTarin::Ready_Components()
{
    __super::Ready_Components();

    /* FOR.Com_Model */
    if (FAILED(__super::Add_Component(LEVEL_MARINHOUSE, TEXT("Prototype_Component_Model_NPC_Tarin"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CTarin::Ready_PartObjects()
{
    return S_OK;
}


CTarin* CTarin::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CTarin* pInstance = new CTarin(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CTarin"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CTarin::Clone(void* pArg)
{
    CTarin* pInstance = new CTarin(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CTarin"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CTarin::Free()
{
    __super::Free();

    Safe_Release(m_pFsmCom);
}
