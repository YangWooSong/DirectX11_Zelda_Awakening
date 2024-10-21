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

    m_isActive = false;
    
    ITEM_EXPLAIN_DESC* pDesc2 = static_cast<ITEM_EXPLAIN_DESC*>(pArg);
    m_iItemIndex = pDesc2->iItemType;
    m_iLevelIndex = LEVEL_STORE;
    m_pTransformCom->Set_Scaled(100.f, 100.f, 1.0f);
 
    return S_OK;
}

void CItemExplainUI::Priority_Update(_float fTimeDelta)
{

}

void CItemExplainUI::Update(_float fTimeDelta)
{
    _vector vPos = m_pParentGameObj->Get_Transform()->Get_State(CTransform::STATE_POSITION);
    _float3 fPos = {};
   
    XMStoreFloat3(&fPos, vPos);
    m_ParentPos = fPos;
   /* fPos = { fPos.x + 0.7f ,fPos.y ,fPos.z - 0.6f };*/

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&fPos));
    
}

void CItemExplainUI::Late_Update(_float fTimeDelta)
{
    if (m_isActive)
    {
        m_pTransformCom->BillBoard(LEVEL_STORE);
        m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);
    }
}

HRESULT CItemExplainUI::Render()
{
    if (m_isActive)
    {
        if(m_iItemIndex == BOMB)
        {
            m_pGameInstance->Render_Center(TEXT("Mallang24_Bold"), TEXT("벽을 부수는 폭탄"), XMVectorSet(g_iWinSizeX * 0.4f, g_iWinSizeY * 0.25f, 0.f, 1.f), XMVectorSet(1.f, 1.f, 1.f, m_fAlpha), 0.f, XMVectorSet(0.f, 0.f, 0.f, 1.f), 1.f, true);
          m_pGameInstance->Render_Text(TEXT("Font_Number24"), TEXT("5 "), XMVectorSet(g_iWinSizeX * 0.36f, g_iWinSizeY * 0.27f, 0.f, 1.f), XMVectorSet(1.f, 0.5f, 0.5f, m_fAlpha), 0.f, XMVectorSet(0.f, 0.f, 0.f, 1.f), 1.f, true);
         m_pGameInstance->Render_Text(TEXT("Mallang24_Bold"), TEXT("   루피"), XMVectorSet(g_iWinSizeX * 0.36f, g_iWinSizeY * 0.27f, 0.f, 1.f), XMVectorSet(1.f, 0.5f, 0.5f, m_fAlpha), 0.f, XMVectorSet(0.f, 0.f, 0.f, 1.f), 1.f, true);
        }
        else
        {
            m_pGameInstance->Render_Center(TEXT("Mallang24_Bold"), TEXT("체력을 늘려주는 하트"), XMVectorSet(g_iWinSizeX * 0.5f, g_iWinSizeY * 0.25f, 0.f, 1.f), XMVectorSet(1.f, 1.f, 1.f, m_fAlpha), 0.f, XMVectorSet(0.f, 0.f, 0.f, 1.f), 1.f, true);
            m_pGameInstance->Render_Text(TEXT("Font_Number24"), TEXT("10    "), XMVectorSet(g_iWinSizeX * 0.45f, g_iWinSizeY * 0.27f, 0.f, 1.f), XMVectorSet(1.f, 0.5f, 0.5f, m_fAlpha), 0.f, XMVectorSet(0.f, 0.f, 0.f, 1.f), 1.f, true);
            m_pGameInstance->Render_Text(TEXT("Mallang24_Bold"), TEXT("    루피"), XMVectorSet(g_iWinSizeX * 0.45f, g_iWinSizeY * 0.27f, 0.f, 1.f), XMVectorSet(1.f, 0.5f, 0.5f, m_fAlpha), 0.f, XMVectorSet(0.f, 0.f, 0.f, 1.f), 1.f, true);
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
