#include "stdafx.h"
#include "Cross_Effect.h"
#include "GameInstance.h"

CCross_Effect::CCross_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :C2DEffects(pDevice, pContext)
{
}

CCross_Effect::CCross_Effect(const CCross_Effect& Prototype)
    :C2DEffects(Prototype)
{
}


HRESULT CCross_Effect::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CCross_Effect::Initialize(void* pArg)
{
    /* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_pTransformCom->Set_Scaled(1.f, 1.f, 1.f);
    m_isActive = false;
    m_iDepth = 2;
    return S_OK;
}

void CCross_Effect::Priority_Update(_float fTimeDelta)
{
}

void CCross_Effect::Update(_float fTimeDelta)
{
    if (m_isActive)
    {
        m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pParentObj->Get_Transform()->Get_State(CTransform::STATE_POSITION));

        if(m_iEffectType == MONSTER_DIED)
            Lerp_Size(fTimeDelta);
    }
}

void CCross_Effect::Late_Update(_float fTimeDelta)
{
    if (m_isActive)
    {
        __super::Late_Update(fTimeDelta);
    }
}

HRESULT CCross_Effect::Render()
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

        if (FAILED(m_pShaderCom->Begin(7)))
            return E_FAIL;

        if (FAILED(m_pVIBufferCom->Bind_Buffers()))
            return E_FAIL;

        if (FAILED(m_pVIBufferCom->Render()))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CCross_Effect::Ready_Components()
{
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Cross_Color"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    return S_OK;
}

void CCross_Effect::Lerp_Size(_float fTimeDelta)
{
    _float3 fCurSize = m_pTransformCom->Get_Scaled();

    _float fAmout = fTimeDelta * 4.f;

    if (fAmout + fCurSize.x <= 3.f && m_bAlphaDown == false)
        m_pTransformCom->Set_Scaled(fCurSize.x + fAmout, fCurSize.y + fAmout, fCurSize.z + fAmout);
    else
        m_bAlphaDown = true;

    if (m_bAlphaDown)
    {
        if(m_fColor.w > 0)
            m_fColor.w -= fTimeDelta * 4.f;
    }
}

CCross_Effect* CCross_Effect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CCross_Effect* pInstance = new CCross_Effect(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CCross_Effect"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CCross_Effect::Clone(void* pArg)
{
    CCross_Effect* pInstance = new CCross_Effect(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CCross_Effect"));
        Safe_Release(pInstance);
    }

    return pInstance;
}


void CCross_Effect::Free()
{
    __super::Free();
}