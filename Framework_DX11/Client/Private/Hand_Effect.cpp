#include "stdafx.h"
#include "Hand_Effect.h"
#include "GameInstance.h"
#include "Player.h"
CHand_Effect::CHand_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :C2DEffects(pDevice, pContext)
{
}

CHand_Effect::CHand_Effect(const CHand_Effect& Prototype)
    :C2DEffects(Prototype)
{
}


HRESULT CHand_Effect::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CHand_Effect::Initialize(void* pArg)
{
    
    HAND_EFFECT_DESC* pDesc = static_cast<HAND_EFFECT_DESC*>(pArg);

    m_pSocketBoneMatrix = pDesc->pSocketBoneMatrix;
    m_pParentMatrix = pDesc->pParentWorldMatrix;

    /* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_isActive = false;

    m_iDepth = 1;

    return S_OK;
}

void CHand_Effect::Priority_Update(_float fTimeDelta)
{

}

void CHand_Effect::Update(_float fTimeDelta)
{
    if (m_isActive)
    { 
        _matrix		SocketMatrix = XMLoadFloat4x4(m_pSocketBoneMatrix);

        for (size_t i = 0; i < 3; i++)
        {
            SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);
        }
        XMStoreFloat4x4(&m_WorldMatrix, SocketMatrix * XMLoadFloat4x4(m_pParentMatrix) );

        m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(&m_WorldMatrix)) ; 
        m_pTransformCom->Set_Scaled(2.f, 2.f, 2.f);
   }
}

void CHand_Effect::Late_Update(_float fTimeDelta)
{
    if (m_isActive)
    {
        m_pTransformCom->BillBoard(m_iLevelIndex);
        m_pGameInstance->Add_RenderObject(CRenderer::RG_BLOOM, this);
    }
}

HRESULT CHand_Effect::Render()
{
    if (m_isActive)
    {
        if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_RawValue("g_fColor", &m_fColor, sizeof(_float4))))
            return E_FAIL;

        if (FAILED(m_pTextureCom->Bind_ShadeResource(m_pShaderCom, "g_Texture", 2)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(6)))
            return E_FAIL;

        if (FAILED(m_pVIBufferCom->Bind_Buffers()))
            return E_FAIL;

        if (FAILED(m_pVIBufferCom->Render()))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CHand_Effect::Ready_Components()
{
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Flash"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    return S_OK;
}

CHand_Effect* CHand_Effect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CHand_Effect* pInstance = new CHand_Effect(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CHand_Effect"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CHand_Effect::Clone(void* pArg)
{
    CHand_Effect* pInstance = new CHand_Effect(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CHand_Effect"));
        Safe_Release(pInstance);
    }

    return pInstance;
}


void CHand_Effect::Free()
{
    __super::Free();
}