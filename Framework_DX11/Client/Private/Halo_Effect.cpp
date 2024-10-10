#include "stdafx.h"
#include "Halo_Effect.h"
#include "GameInstance.h"
CHalo_Effect::CHalo_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :C3D_Effects(pDevice, pContext)
{
}
CHalo_Effect::CHalo_Effect(const CHalo_Effect& Prototype)
    :C3D_Effects(Prototype)
{
}

HRESULT CHalo_Effect::Initialize_Prototype()
{
    __super::Initialize_Prototype();
    return S_OK;
}

HRESULT CHalo_Effect::Initialize(void* pArg)
{
    __super::Initialize(pArg);

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_pModelCom->Add_Texture_to_Material(TEXT("../Bin/Resources/Zelda/Effect/rainbow_01.dds"), TEXTURE_TYPE::DIFFUSE, 0);
    m_pModelCom->Add_Texture_to_Material(TEXT("../Bin/Resources/Zelda/Effect/halo_00.dds"), TEXTURE_TYPE::DIFFUSE, 0);

    return S_OK;
}

void CHalo_Effect::Priority_Update(_float fTimeDelta)
{
    if (m_isActive)
    {
    
        __super::Priority_Update(fTimeDelta);
    }
}

void CHalo_Effect::Update(_float fTimeDelta)
{
    m_fTexMove += fTimeDelta * 0.1;
    _vector vOffeset = { 0.f, 1.3f,0.f };
    m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(m_pParentMatrix));
    m_pTransformCom->Set_Scaled(3.f, 3.f, 0.1f);
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_State(CTransform::STATE_POSITION) + vOffeset);
}

void CHalo_Effect::Late_Update(_float fTimeDelta)
{
    m_pTransformCom->BillBoard(m_iLevelIndex);
    __super::Late_Update(fTimeDelta);
}

HRESULT CHalo_Effect::Render()
{
    if (m_isActive)
    {
        if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
            return E_FAIL;
        if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
            return E_FAIL;
        if (FAILED(m_pShaderCom->Bind_RawValue("g_MoveTexCoord", &m_fTexMove, sizeof(_float))))
            return E_FAIL;

        _uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

        for (size_t i = 0; i < iNumMeshes; i++)
        {
            if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", TEXTURE_TYPE::DIFFUSE, 0,1)))
                return E_FAIL;
            
            if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_AlphaTexture", TEXTURE_TYPE::DIFFUSE, 0, 2)))
                return E_FAIL;

            if (FAILED(m_pShaderCom->Begin(7)))
                return E_FAIL;

            if (FAILED(m_pModelCom->Render((_uint)i)))
                return E_FAIL;
        }

        _float fZero = 0.f;
        if (FAILED(m_pShaderCom->Bind_RawValue("g_MoveTexCoord", &fZero, sizeof(_float))))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CHalo_Effect::Ready_Components()
{
    /* FOR.Com_Model */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_halo_00"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    //if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_halo_00"),
    //    TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pHaloTexture))))
    //    return E_FAIL;

    //if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_rainbow_01"),
    //    TEXT("Com_Texture1"), reinterpret_cast<CComponent**>(&m_pRainbowTexture))))
    //    return E_FAIL;

    return S_OK;
}

C3D_Effects* CHalo_Effect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CHalo_Effect* pInstance = new CHalo_Effect(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CHalo_Effect"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CHalo_Effect::Clone(void* pArg)
{
    CHalo_Effect* pInstance = new CHalo_Effect(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CHalo_Effect"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CHalo_Effect::Free()
{
    __super::Free();

    //Safe_Release(m_pHaloTexture);
    //Safe_Release(m_pRainbowTexture);
}

