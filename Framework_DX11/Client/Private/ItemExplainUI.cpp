#define _CRT_NON_CONFORMING_SWPRINTFS

#include "stdafx.h"
#include "ItemExplainUI.h"
#include "GameInstance.h"
#include "Link.h"
#include "MainUI.h"

CItemExplainUI::CItemExplainUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CUIObject(pDevice, pContext)
{
}

CItemExplainUI::CItemExplainUI(const CItemExplainUI& Prototype)
    :CUIObject(Prototype)
{
}

HRESULT CItemExplainUI::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CItemExplainUI::Initialize(void* pArg)
{
    CUIObject::UI_DESC* pDesc = static_cast<UI_DESC*>(pArg);

    /* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_isActive = true;

    m_iLevelIndex = static_cast<CMainUI*>(m_pParentUI)->Get_LevelIndex();
    
    ITEM_EXPLAIN_DESC* pDesc2 = static_cast<ITEM_EXPLAIN_DESC*>(pArg);
    m_iItemIndex = pDesc2->iItemType;
    
    return S_OK;
}

void CItemExplainUI::Priority_Update(_float fTimeDelta)
{

}

void CItemExplainUI::Update(_float fTimeDelta)
{
    
}

void CItemExplainUI::Late_Update(_float fTimeDelta)
{
    if (m_isActive)
    {
        __super::Late_Update(fTimeDelta);
        m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);
    }
}

HRESULT CItemExplainUI::Render()
{
    if (m_isActive)
    {
        if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
            return E_FAIL;

        if (FAILED(m_pTextureCom->Bind_ShadeResource(m_pShaderCom, "g_Texture", 0)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float))))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(10)))
            return E_FAIL;

        if (FAILED(m_pVIBufferCom->Bind_Buffers()))
            return E_FAIL;

        if (FAILED(m_pVIBufferCom->Render()))
            return E_FAIL;

        //원상복구
        _float fAlpha = 1.f;
        if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &fAlpha, sizeof(_float))))
            return E_FAIL;

        if(m_iItemIndex == BOMB)
        {
            m_pGameInstance->Render_Text(TEXT("Font_Number24"), TEXT("%d"), XMVectorSet(m_fX + 9.f, m_fY - 20.f, 0.f, 1.f), XMVectorSet(1.f, 1.f, 1.f, m_fAlpha), 0.f, XMVectorSet(0.f, 0.f, 0.f, 1.f), 1.f, true);
          m_pGameInstance->Render_Text(TEXT("Font_Number24"), TEXT("%d"), XMVectorSet(m_fX + 9.f, m_fY - 20.f, 0.f, 1.f), XMVectorSet(1.f, 1.f, 1.f, m_fAlpha), 0.f, XMVectorSet(0.f, 0.f, 0.f, 1.f), 1.f, true);
        }
        else
        {
            m_pGameInstance->Render_Text(TEXT("Font_Number24"), TEXT("%d"), XMVectorSet(m_fX + 9.f, m_fY - 20.f, 0.f, 1.f), XMVectorSet(1.f, 1.f, 1.f, m_fAlpha), 0.f, XMVectorSet(0.f, 0.f, 0.f, 1.f), 1.f, true);
            m_pGameInstance->Render_Text(TEXT("Font_Number24"), TEXT("%d"), XMVectorSet(m_fX + 9.f, m_fY - 20.f, 0.f, 1.f), XMVectorSet(1.f, 1.f, 1.f, m_fAlpha), 0.f, XMVectorSet(0.f, 0.f, 0.f, 1.f), 1.f, true);

        }


    }

    return S_OK;
}

HRESULT CItemExplainUI::Ready_Components()
{
    /* FOR.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Black"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    /* FOR.Com_VIBuffer */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    return S_OK;
}

void CItemExplainUI::Show(_float fTimeDelta)
{
    if (m_fAlpha != 1.f)
        m_fAlpha = min(1.f, m_fAlpha += fTimeDelta * 1.f);

}

void CItemExplainUI::Hide(_float fTimeDelta)
{
    if(m_fAlpha != 0.f)
        m_fAlpha = max(0.f, m_fAlpha -= fTimeDelta * 1.f);

}

CItemExplainUI* CItemExplainUI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CItemExplainUI* pInstance = new CItemExplainUI(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CItemExplainUI"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CItemExplainUI::Clone(void* pArg)
{
    CItemExplainUI* pInstance = new CItemExplainUI(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CItemExplainUI"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CItemExplainUI::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
}
