#define _CRT_NON_CONFORMING_SWPRINTFS

#include "stdafx.h"
#include "QuestUI.h"
#include "GameInstance.h"
#include "Link.h"
#include "MainUI.h"

CQuestUI::CQuestUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CUIObject(pDevice, pContext)
{
}

CQuestUI::CQuestUI(const CQuestUI& Prototype)
    :CUIObject(Prototype)
{
}

HRESULT CQuestUI::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CQuestUI::Initialize(void* pArg)
{
    CUIObject::UI_DESC* pDesc = static_cast<UI_DESC*>(pArg);

    /* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_isActive = false;

    m_iLevelIndex = static_cast<CMainUI*>(m_pParentUI)->Get_LevelIndex();
    m_fOriPosX = m_fX;

    return S_OK;
}

void CQuestUI::Priority_Update(_float fTimeDelta)
{

}

void CQuestUI::Update(_float fTimeDelta)
{
  //  m_fX = 200.f;
   // m_fY = 100.f;

    m_iKillMonsterCount = static_cast<CMainUI*>(m_pParentUI)->Get_Player_KillMonsterCount();

    //활성화하면 나타나게 (퀘스트 시작)
    if (m_isActive && m_bShow == false && m_bShowEnd == false)
        m_bShow = true;

    if (m_bShow)
    {
        Show(fTimeDelta);
    }

    //닫을때 (퀘스트 끝)
  /*  if (m_bShowEnd && m_isActive == false)
    {
        if (m_fActiveTimer <= 0.f)
        {
            m_bHide = true;
            m_bShowEnd = false;
        }
    }*/

    if(m_bHide)
        Hide(fTimeDelta);
}

void CQuestUI::Late_Update(_float fTimeDelta)
{
    if (m_isActive)
    {
        __super::Late_Update(fTimeDelta);
        m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);
    }
}

HRESULT CQuestUI::Render()
{
    if (m_isActive && m_bHideEnd == false)
    {
        if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
            return E_FAIL;

        if (FAILED(m_pTextureCom[0]->Bind_ShadeResource(m_pShaderCom, "g_Texture", 0)))
            return E_FAIL;       
        
        if (FAILED(m_pTextureCom[1]->Bind_ShadeResource(m_pShaderCom, "g_DissolveTexture", 0)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float))))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(14)))
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
        swprintf_s(szText, TEXT("%d / 3"), m_iKillMonsterCount);  // m_iPlayerLupee 값을 문자열로 변환


        m_pGameInstance->Render_Text(TEXT("Mallang24_Bold"), TEXT("몬스터를 처치하자"), XMVectorSet(m_fX - 80.f , m_fY - 35.f, 0.f, 1.f), XMVectorSet(1.f, 1.f, 1.f, m_fAlpha), 0.f, XMVectorSet(0.f, 0.f, 0.f, 1.f), 0.8f, true);
        if(m_iKillMonsterCount <3)
             m_pGameInstance->Render_Text(TEXT("Font_Number24"), szText, XMVectorSet(m_fX - 80.f, m_fY , 0.f, 1.f), XMVectorSet(1.f, 0.5f, 0.5f, m_fAlpha), 0.f, XMVectorSet(0.f, 0.f, 0.f, 1.f), 1.f, true);
        else
             m_pGameInstance->Render_Text(TEXT("Font_Number24"), szText, XMVectorSet(m_fX - 80.f, m_fY , 0.f, 1.f), XMVectorSet(0.5f, 1.f, 0.5f, m_fAlpha), 0.f, XMVectorSet(0.f, 0.f, 0.f, 1.f), 1.f, true);


    }

    return S_OK;
}

HRESULT CQuestUI::Ready_Components()
{
    /* FOR.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Black"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom[0]))))
        return E_FAIL;  
    
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_QuestGrad"),
        TEXT("Com_Texture1"), reinterpret_cast<CComponent**>(&m_pTextureCom[1]))))
        return E_FAIL;

    /* FOR.Com_VIBuffer */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    return S_OK;
}

void CQuestUI::Show(_float fTimeDelta)
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

void CQuestUI::Hide(_float fTimeDelta)
{
    if (m_fX <= m_fOriPosX)
    {
        m_fX += fTimeDelta * 40.f;
        m_fAlpha = max(0.f, m_fAlpha -= fTimeDelta * 1.f);
    }
    else
    {
        m_bHideEnd = true;
        m_isActive == false;
    }
}

CQuestUI* CQuestUI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CQuestUI* pInstance = new CQuestUI(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CQuestUI"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CQuestUI::Clone(void* pArg)
{
    CQuestUI* pInstance = new CQuestUI(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CQuestUI"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CQuestUI::Free()
{
    __super::Free();

    for (auto& iter : m_pTextureCom)
        Safe_Release(iter);

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pVIBufferCom);
}
