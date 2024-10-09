#include "stdafx.h"
#include "Glow_Effect.h"
#include "GameInstance.h"

CGlow_Effect::CGlow_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :C2DEffects(pDevice, pContext)
{
}

CGlow_Effect::CGlow_Effect(const CGlow_Effect& Prototype)
    :C2DEffects(Prototype)
{
}


HRESULT CGlow_Effect::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CGlow_Effect::Initialize(void* pArg)
{
    /* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_isActive = false;

    if (m_iEffectType == MONSTER_DIED)
    {
        m_iDepth = 1;
        m_fMaxSize = 1.5f;
    }

    m_vOriSize = m_pTransformCom->Get_Scaled();
    return S_OK;
}

void CGlow_Effect::Priority_Update(_float fTimeDelta)
{
}

void CGlow_Effect::Update(_float fTimeDelta)
{
    if (m_isActive )
    {
        m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pParentObj->Get_Transform()->Get_State(CTransform::STATE_POSITION));

        if (m_iEffectType == MONSTER_DIED)
        {
            Lerp_Size(fTimeDelta);
        }

    }
    else
    {
        if (m_bReset == false)
        {
            m_bReset = true;
            m_bSizeUp = true;
            m_pTransformCom->Set_Scaled(m_vOriSize.x, m_vOriSize.y, m_vOriSize.z);
        }
    }
}

void CGlow_Effect::Late_Update(_float fTimeDelta)
{
    if (m_isActive )
    {
        __super::Late_Update(fTimeDelta);
    }
}

HRESULT CGlow_Effect::Render()
{
    if(m_isActive )
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

HRESULT CGlow_Effect::Ready_Components()
{
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Glow"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    return S_OK;
}

void CGlow_Effect::Lerp_Size(_float fTimeDelta)
{
    _float3 fCurSize = m_pTransformCom->Get_Scaled();

    _float fAmout = fTimeDelta * 4.f;

    if (fAmout + fCurSize.x <= m_fMaxSize && m_bSizeDown == false)
        m_pTransformCom->Set_Scaled(fCurSize.x + fAmout, fCurSize.y + fAmout, fCurSize.z + fAmout);
    else
        m_bSizeDown = true;

    if (m_bSizeDown)
    {
        if (fCurSize.x - fAmout >= 0.1f)
            m_pTransformCom->Set_Scaled(fCurSize.x - fAmout, fCurSize.y - fAmout, fCurSize.z - fAmout);
        else
        {
            m_isActive = false;
        //    m_bSizeDown = false;
        }
    }
}

CGlow_Effect* CGlow_Effect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CGlow_Effect* pInstance = new CGlow_Effect(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CGlow_Effect"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CGlow_Effect::Clone(void* pArg)
{
    CGlow_Effect* pInstance = new CGlow_Effect(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CGlow_Effect"));
        Safe_Release(pInstance);
    }

    return pInstance;
}


void CGlow_Effect::Free()
{
    __super::Free();
}