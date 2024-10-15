#include "stdafx.h"
#include "FadeInOut.h"
#include "GameInstance.h"
#include "MainUI.h"

CFadeInOut::CFadeInOut(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CUIObject(pDevice, pContext)
{
}

CFadeInOut::CFadeInOut(const CFadeInOut& Prototype)
    :CUIObject(Prototype)
{
}

HRESULT CFadeInOut::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CFadeInOut::Initialize(void* pArg)
{
    /* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_isActive = false;

    m_iDepth = 10;

    return S_OK;
}

void CFadeInOut::Priority_Update(_float fTimeDelta)
{
}

void CFadeInOut::Update(_float fTimeDelta)
{
    if(m_isActive)
    {
        if (m_bFadeIn)
        {
            m_fAlpha = max(0.f, m_fAlpha - fTimeDelta*3.f);

            if (m_fAlpha == 0.f)
            {
                m_isActive = false;
                static_cast<CMainUI*>(m_pParentUI)->Set_isFinishFadeIn(true);
                m_bFadeIn = false;
            }
        }

        if (m_bFadeOut)
        {
            m_fAlpha = min(1.f, m_fAlpha + fTimeDelta * 3.f);

            if (m_fAlpha == 1.f)
            {
                m_isActive = false;
                static_cast<CMainUI*>(m_pParentUI)->Set_isFinishFadeOut(true);
                m_bFadeOut = false;
            }
        }
    }
}

void CFadeInOut::Late_Update(_float fTimeDelta)
{
    if(m_isActive )
    {
        __super::Late_Update(fTimeDelta);
        m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);
    }
}

HRESULT CFadeInOut::Render()
{

    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float))))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShadeResource(m_pShaderCom, "g_Texture", 0)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Begin(12)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Buffers()))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    _float fOne = 1.f;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &fOne, sizeof(_float))))
        return E_FAIL;

    return S_OK;
}

void CFadeInOut::FadeIn()
{
    static_cast<CMainUI*>(m_pParentUI)->Set_isFinishFadeIn(false);
    m_bFadeIn = true;
    m_fAlpha = 1.f;
    m_isActive = true;
}

void CFadeInOut::FadeOut()
{
    static_cast<CMainUI*>(m_pParentUI)->Set_isFinishFadeOut(false);
    m_bFadeOut = true;
    m_fAlpha = 0.f;
    m_isActive = true;
}

HRESULT CFadeInOut::Ready_Components()
{
    /* FOR.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* FOR.Com_Texture */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Black"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    /* FOR.Com_VIBuffer */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    return S_OK;
}


CFadeInOut* CFadeInOut::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CFadeInOut* pInstance = new CFadeInOut(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CFadeInOut"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CFadeInOut::Clone(void* pArg)
{
    CFadeInOut* pInstance = new CFadeInOut(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CFadeInOut"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CFadeInOut::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
}
