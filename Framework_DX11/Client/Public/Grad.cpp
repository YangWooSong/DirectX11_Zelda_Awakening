#include "stdafx.h"
#include "Grad.h"
#include "GameInstance.h"

CGrad::CGrad(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :C2DEffects(pDevice, pContext)
{
}

CGrad::CGrad(const CGrad& Prototype)
    :C2DEffects(Prototype)
{
}


HRESULT CGrad::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CGrad::Initialize(void* pArg)
{
    /* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_isActive = true;

    EFFECT_DESC* pDesc = static_cast<EFFECT_DESC*>(pArg);
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&pDesc->vPosition));
    m_fTurnAnlge = pDesc->vRotation.y;
    m_pTransformCom->RotationThreeAxis(pDesc->vRotation);
    m_iDepth = 1;

    return S_OK;
}

void CGrad::Priority_Update(_float fTimeDelta)
{
}

void CGrad::Update(_float fTimeDelta)
{
    if (m_isActive)
    {

    }
}

void CGrad::Late_Update(_float fTimeDelta)
{
    if (m_isActive)
    {
        //m_pTransformCom->BillBoard_RotZ(LEVEL_MARINHOUSE, m_fTurnAnlge);
        m_pGameInstance->Add_RenderObject(CRenderer::RG_BLOOM, this);
    }
}

HRESULT CGrad::Render()
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

        if (FAILED(m_pTextureCom->Bind_ShadeResource(m_pShaderCom, "g_Texture", 0)))
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

HRESULT CGrad::Ready_Components()
{
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Grad"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    return S_OK;
}


CGrad* CGrad::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CGrad* pInstance = new CGrad(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CGrad"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CGrad::Clone(void* pArg)
{
    CGrad* pInstance = new CGrad(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CGrad"));
        Safe_Release(pInstance);
    }

    return pInstance;
}


void CGrad::Free()
{
    __super::Free();
}