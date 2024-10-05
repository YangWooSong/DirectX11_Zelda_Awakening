#define _CRT_NON_CONFORMING_SWPRINTFS

#include "stdafx.h"
#include "LupeeUI.h"
#include "GameInstance.h"
#include "Link.h"
#include "MainUI.h"

CLupeeUI::CLupeeUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CUIObject(pDevice, pContext)
{
}

CLupeeUI::CLupeeUI(const CLupeeUI& Prototype)
    :CUIObject(Prototype)
{
}

HRESULT CLupeeUI::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CLupeeUI::Initialize(void* pArg)
{
    CUIObject::UI_DESC* pDesc = static_cast<UI_DESC*>(pArg);

    /* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_isActive = true;

    m_iPlayerLupee = static_cast<CMainUI*>(m_pParentUI)->Get_Player_Lupee();
    m_iLevelIndex = static_cast<CMainUI*>(m_pParentUI)->Get_LevelIndex();
    m_fOriPosX = m_fX;

    return S_OK;
}

void CLupeeUI::Priority_Update(_float fTimeDelta)
{

}

void CLupeeUI::Update(_float fTimeDelta)
{
    if (m_iLevelIndex == LEVEL_STORE)   //상점에서는 계속 활성화
    {
        m_iPlayerLupee = static_cast<CMainUI*>(m_pParentUI)->Get_Player_Lupee();
        m_isActive = true;
        m_fAlpha = 1.f;
    }
    else
    {
        //금액에 변동이 있으면 활성화
        if (m_iPlayerLupee != static_cast<CMainUI*>(m_pParentUI)->Get_Player_Lupee() )
        {
            m_iPlayerLupee = static_cast<CMainUI*>(m_pParentUI)->Get_Player_Lupee();
            m_isActive = true;
            m_bShow = true;
            m_fActiveTimer = 3.f;
        }

        if (m_bShow)
        {
            Show(fTimeDelta);
        }

        if(m_bShowEnd)
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
}

void CLupeeUI::Late_Update(_float fTimeDelta)
{
    if (m_isActive)
    {
        __super::Late_Update(fTimeDelta);
        m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);
    }
}

HRESULT CLupeeUI::Render()
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

        if (FAILED(m_pShaderCom->Begin(4)))
            return E_FAIL;

        if (FAILED(m_pVIBufferCom->Bind_Buffers()))
            return E_FAIL;

        if (FAILED(m_pVIBufferCom->Render()))
            return E_FAIL;

        //원상복구
        _float fAlpha = 1.f;
        if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &fAlpha, sizeof(_float))))
            return E_FAIL;

        _tchar szText[64];  // 출력할 텍스트를 저장할 버퍼
        swprintf_s(szText, TEXT("%d"), m_iPlayerLupee);  // m_iPlayerLupee 값을 문자열로 변환
        m_pGameInstance->Render_Text(TEXT("Font_Number24"), szText, XMVectorSet(m_fX + 9.f, m_fY - 20.f , 0.f, 1.f), XMVectorSet(1.f, 1.f, 1.f, m_fAlpha), 0.f, XMVectorSet(0.f, 0.f, 0.f, 1.f), 1.f, true);
        

    }

    return S_OK;
}

HRESULT CLupeeUI::Ready_Components()
{
    /* FOR.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_LupeeUI"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    /* FOR.Com_VIBuffer */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    return S_OK;
}

void CLupeeUI::Show(_float fTimeDelta)
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

void CLupeeUI::Hide(_float fTimeDelta)
{
    if (m_fX <= m_fOriPosX)
    {
        m_fX += fTimeDelta * 40.f;
        m_fAlpha = max(0.f, m_fAlpha -= fTimeDelta * 1.f) ;
    }
    else
    {
        m_bHide = false;
        m_isActive = false;
    }
}

CLupeeUI* CLupeeUI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CLupeeUI* pInstance = new CLupeeUI(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CLupeeUI"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CLupeeUI::Clone(void* pArg)
{
    CLupeeUI* pInstance = new CLupeeUI(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CLupeeUI"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLupeeUI::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
}
