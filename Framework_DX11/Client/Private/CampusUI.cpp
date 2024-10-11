#include "stdafx.h"
#include "CampusUI.h"
#include "GameInstance.h"
#include "Link.h"
#include "MainUI.h"

CCampusUI::CCampusUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CUIObject(pDevice, pContext)
{
}

CCampusUI::CCampusUI(const CCampusUI& Prototype)
    :CUIObject(Prototype)
{
}

HRESULT CCampusUI::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CCampusUI::Initialize(void* pArg)
{
    CUIObject::UI_DESC* pDesc = static_cast<UI_DESC*>(pArg);

    /* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_isActive = true;
    m_fOriPosX = m_fX;
    return S_OK;
}

void CCampusUI::Priority_Update(_float fTimeDelta)
{

}

void CCampusUI::Update(_float fTimeDelta)
{
    if (static_cast<CMainUI*>(m_pParentUI)->Get_Active_Campus())
    {
        m_isActive = true;
        m_bShow = true;
        m_fActiveTimer = 3.f;
        static_cast<CMainUI*>(m_pParentUI)->SEt_Active_Campus(false);
    }

    if (m_bShow)
    {
        Show(fTimeDelta);
    }

    if (m_bShowEnd)
    {
        m_fActiveTimer -= fTimeDelta;
        if (m_fActiveTimer <= 0.f)
        {
            m_bHide = true;
            m_bShowEnd = false;
            m_fActiveTimer = 0.f;
        }
    }
    if (m_bHide)
        Hide(fTimeDelta);

}

void CCampusUI::Late_Update(_float fTimeDelta)
{
    if (m_isActive)
    {
        __super::Late_Update(fTimeDelta);
        m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);
    }
}

HRESULT CCampusUI::Render()
{
    if (m_isActive)
    {
        if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
            return E_FAIL;

        if (FAILED(m_pTextureCom->Bind_ShadeResource(m_pShaderCom, "g_Texture", 1)))
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
    }

    return S_OK;
}

HRESULT CCampusUI::Ready_Components()
{
    /* FOR.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_ItemIcon"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    /* FOR.Com_VIBuffer */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    return S_OK;
}

void CCampusUI::Show(_float fTimeDelta)
{
    if (m_fX > m_fTargetX)
    {
        m_fX -= fTimeDelta * 40.f;
        m_fAlpha = min(1.f, m_fAlpha += fTimeDelta * 1.f);
    }
    else
    {
        m_bShow = false;
        m_bShowEnd = true;
    }
}

void CCampusUI::Hide(_float fTimeDelta)
{
    if (m_fX <= m_fOriPosX)
    {
        m_fX += fTimeDelta * 40.f;
        m_fAlpha = max(0.f, m_fAlpha -= fTimeDelta * 1.f);
    }
    else
    {
        m_bHide = false;
        m_isActive = false;
    }
}

CCampusUI* CCampusUI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CCampusUI* pInstance = new CCampusUI(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CCampusUI"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CCampusUI::Clone(void* pArg)
{
    CCampusUI* pInstance = new CCampusUI(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CCampusUI"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCampusUI::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
}
