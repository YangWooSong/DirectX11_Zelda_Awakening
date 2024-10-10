#include "stdafx.h"
#include "Smoke_Effect.h"
#include "GameInstance.h"

CSmoke_Effect::CSmoke_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :C2DEffects(pDevice, pContext)
{
}

CSmoke_Effect::CSmoke_Effect(const CSmoke_Effect& Prototype)
    :C2DEffects(Prototype)
{
}


HRESULT CSmoke_Effect::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CSmoke_Effect::Initialize(void* pArg)
{
    /* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_isActive = false;
    m_iDepth = 0;
    return S_OK;
}

void CSmoke_Effect::Priority_Update(_float fTimeDelta)
{
}

void CSmoke_Effect::Update(_float fTimeDelta)
{
    if (m_isActive)
    {
        m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pParentObj->Get_Transform()->Get_State(CTransform::STATE_POSITION));
        m_bReset = false;
        if (m_iEffectType == MONSTER_DIED_EFFECT)
        {
            m_fOutLineColor = { 1.f, 1.f, 1.f, 1.f };
            m_fDissovle = max(0.f, m_fDissovle - fTimeDelta * 1.f);
            m_bDissolve = true;
        }
        if (m_iEffectType == BOMB_EXPLOSION_EFFECT)
        {
            m_iDepth = 0;
            m_fOutLineColor = { 0.8f, 0.3f, 0.f, 1.f };
            m_fDissovle = max(0.f, m_fDissovle - fTimeDelta);
            m_bDissolve = true;
        }
    }
    else
    {
        if (m_bReset == false)
        {
            m_bReset = true;
            m_bDissolve = false;
            m_fDissovle = 1.f;
        }
    }
}

void CSmoke_Effect::Late_Update(_float fTimeDelta)
{
    if (m_isActive)
    {
        __super::Late_Update(fTimeDelta);
    }
}

HRESULT CSmoke_Effect::Render()
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

        if (FAILED(m_pShaderCom->Bind_RawValue("g_fOutLineColor", &m_fOutLineColor, sizeof(_float4))))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_RawValue("g_fDissolve", &m_fDissovle, sizeof(_float))))
            return E_FAIL;
        
        if (FAILED(m_pShaderCom->Bind_RawValue("m_bDissolve", &m_bDissolve, sizeof(_bool))))
            return E_FAIL;

        if (FAILED(m_pTextureCom->Bind_ShadeResource(m_pShaderCom, "g_Texture", 0)))
            return E_FAIL;

        if (FAILED(m_pDissolveTextureCom->Bind_ShadeResource(m_pShaderCom, "g_DissolveTexture", 0)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(9)))
            return E_FAIL;

        if (FAILED(m_pVIBufferCom->Bind_Buffers()))
            return E_FAIL;

        if (FAILED(m_pVIBufferCom->Render()))
            return E_FAIL;

        _bool bFalse = false;
        if (FAILED(m_pShaderCom->Bind_RawValue("m_bDissolve", &bFalse, sizeof(_bool))))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CSmoke_Effect::Ready_Components()
{
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Smoke"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Dissolve"),
        TEXT("Com_Texture1"), reinterpret_cast<CComponent**>(&m_pDissolveTextureCom))))
        return E_FAIL;

    return S_OK;
}

CSmoke_Effect* CSmoke_Effect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CSmoke_Effect* pInstance = new CSmoke_Effect(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CSmoke_Effect"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CSmoke_Effect::Clone(void* pArg)
{
    CSmoke_Effect* pInstance = new CSmoke_Effect(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CSmoke_Effect"));
        Safe_Release(pInstance);
    }

    return pInstance;
}


void CSmoke_Effect::Free()
{
    __super::Free();

    Safe_Release(m_pDissolveTextureCom);
}