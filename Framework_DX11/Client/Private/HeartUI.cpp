#include "stdafx.h"
#include "HeartUI.h"
#include "GameInstance.h"
#include "Link.h"
#include "MainUI.h"

CHeartUI::CHeartUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CUIObject(pDevice, pContext)
{
}

CHeartUI::CHeartUI(const CHeartUI& Prototype)
    :CUIObject(Prototype)
{
}

HRESULT CHeartUI::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CHeartUI::Initialize(void* pArg)
{
    /* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    HEART_DESC* pDesc = static_cast<HEART_DESC*>(pArg);
    m_bNew = pDesc->bNew;

    if (m_bNew)
    {
        m_fOri_Sizex = m_fSizeX;
        m_fSizeX = 0.1f;
        m_fSizeY = 0.1f;
    }
    return S_OK;
}

void CHeartUI::Priority_Update(_float fTimeDelta)
{

}

void CHeartUI::Update(_float fTimeDelta)
{
    if (m_bStartDecrease)
    {
        if (m_fInvisibleAngle >= 360.f)
            m_bStartDecrease = false;

        _float tmp = m_fInvisibleAngle + 200.f * fTimeDelta;
        m_fInvisibleAngle = min(360.f, tmp);
    }

    if (m_bStartIncrease)
    {
        if (m_fInvisibleAngle <= 0.f)
            m_bStartIncrease = false;

        _float tmp = m_fInvisibleAngle - 10.f * fTimeDelta;
        m_fInvisibleAngle = max(0.f, tmp);
    }

    if (m_bNew)
    {
        if (m_fSizeX == m_fOri_Sizex)
            m_bNew = false;

        m_fSizeX = min(m_fOri_Sizex, m_fSizeX += 10.f * fTimeDelta);
        m_fSizeY = min(m_fOri_Sizex, m_fSizeX += 10.f * fTimeDelta);
    }
}

void CHeartUI::Late_Update(_float fTimeDelta)
{
    if (m_isActive)
    {
        __super::Late_Update(fTimeDelta);
        m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);
    }
}

HRESULT CHeartUI::Render()
{
    if (m_isActive)
    {
        if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_RawValue("g_fAngle", &m_fInvisibleAngle, sizeof(_float))))
            return E_FAIL;

        if (FAILED(m_pTextureCom->Bind_ShadeResource(m_pShaderCom, "g_Texture", 1)))
            return E_FAIL;


        if (FAILED(m_pShaderCom->Begin(5)))
            return E_FAIL;

        if (FAILED(m_pVIBufferCom->Bind_Buffers()))
            return E_FAIL;

        if (FAILED(m_pVIBufferCom->Render()))
            return E_FAIL;

    }

    return S_OK;
}

HRESULT CHeartUI::Ready_Components()
{
    /* FOR.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_HeartUI"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    /* FOR.Com_VIBuffer */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    return S_OK;
}


CHeartUI* CHeartUI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CHeartUI* pInstance = new CHeartUI(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CHeartUI"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CHeartUI::Clone(void* pArg)
{
    CHeartUI* pInstance = new CHeartUI(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CHeartUI"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CHeartUI::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
}
