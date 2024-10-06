#include "stdafx.h"
#include "ItemIconUI.h"
#include "GameInstance.h"
#include "Link.h"
#include "MainUI.h"

CItemIconUI::CItemIconUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CUIObject(pDevice, pContext)
{
}

CItemIconUI::CItemIconUI(const CItemIconUI& Prototype)
    :CUIObject(Prototype)
{
}

HRESULT CItemIconUI::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CItemIconUI::Initialize(void* pArg)
{
    /* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    ITEM_ICON_DESC* pDesc = static_cast<ITEM_ICON_DESC*>(pArg);
    m_iIconType = pDesc->iItemType;

    if(m_iIconType == LINK)
        m_isActive = true;
    else
        m_isActive = false;

    m_iDepth = 2;

    return S_OK;
}

void CItemIconUI::Priority_Update(_float fTimeDelta)
{
}

void CItemIconUI::Update(_float fTimeDelta)
{
   
}

void CItemIconUI::Late_Update(_float fTimeDelta)
{
    if(m_isActive)
    {
        __super::Late_Update(fTimeDelta);
        m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);
    }
    
}

HRESULT CItemIconUI::Render()
{
    if (m_isActive)
    {
        if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
            return E_FAIL;

        if (FAILED(m_pTextureCom->Bind_ShadeResource(m_pShaderCom, "g_Texture", m_iIconType)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(0)))
            return E_FAIL;

        if (FAILED(m_pVIBufferCom->Bind_Buffers()))
            return E_FAIL;

        if (FAILED(m_pVIBufferCom->Render()))
            return E_FAIL;

    }
    return S_OK;
}

HRESULT CItemIconUI::Ready_Components()
{
    /* FOR.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_ItemIconUI"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    /* FOR.Com_VIBuffer */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    return S_OK;
}


CItemIconUI* CItemIconUI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CItemIconUI* pInstance = new CItemIconUI(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CItemIconUI"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CItemIconUI::Clone(void* pArg)
{
    CItemIconUI* pInstance = new CItemIconUI(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CItemIconUI"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CItemIconUI::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
}
