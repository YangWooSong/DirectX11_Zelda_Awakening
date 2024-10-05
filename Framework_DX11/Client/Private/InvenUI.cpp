#include "stdafx.h"
#include "InvenUI.h"
#include "GameInstance.h"
#include "Link.h"
#include "MainUI.h"
#include "ItemIconUI.h"

CInvenUI::CInvenUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CUIObject(pDevice, pContext)
{
}

CInvenUI::CInvenUI(const CInvenUI& Prototype)
    :CUIObject(Prototype)
{
}

HRESULT CInvenUI::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CInvenUI::Initialize(void* pArg)
{
    CUIObject::UI_DESC* pDesc = static_cast<UI_DESC*>(pArg);

    /* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_ChildUI()))
        return E_FAIL;


    m_isActive = true;
  
    return S_OK;
}

void CInvenUI::Priority_Update(_float fTimeDelta)
{
    if (m_isActive)
    {
        for (auto& pChild : m_childUI_List)
            pChild->Priority_Update(fTimeDelta);
    }
}

void CInvenUI::Update(_float fTimeDelta)
{
    if (m_isActive)
    {
        if (static_cast<CMainUI*>(m_pParentUI)->Get_Feather())
        {
            m_childUI_List[FEATHER]->SetActive(true);
        }
        if (static_cast<CMainUI*>(m_pParentUI)->Get_Bomb())
        {
            m_childUI_List[BOMB]->SetActive(true);
        }

        for (auto& pChild : m_childUI_List)
            pChild->Update(fTimeDelta);
    }
}

void CInvenUI::Late_Update(_float fTimeDelta)
{
    if (m_isActive)
    {
        for (auto& pChild : m_childUI_List)
            pChild->Late_Update(fTimeDelta);
    }
}

HRESULT CInvenUI::Render()
{
    if (m_isActive)
    {
        for (auto& pChild : m_childUI_List)
            pChild->Render();
    }
    return S_OK;
}

HRESULT CInvenUI::Ready_ChildUI()
{
    CGameObject* pGameObj = m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_ItemIconUI"));
    CItemIconUI::ITEM_ICON_DESC pDesc{};

    if (pGameObj != nullptr)
    {
        pDesc.fSizeX = 100.f;
        pDesc.fSizeY = 100.f;
        pDesc.fX = g_iWinSizeX * 0.87f;
        pDesc.fY = g_iWinSizeY * 0.1f;
        pDesc.pParent = this;
        pDesc.iItemType = CItemIconUI::INVEN_FEATHER;
        CUIObject* m_pItemIconUI = dynamic_cast<CUIObject*>(pGameObj->Clone(&pDesc));
        m_childUI_List.push_back(m_pItemIconUI);


        pDesc.fX = g_iWinSizeX * 0.95f;
        pDesc.fY = g_iWinSizeY * 0.08f;
        pDesc.pParent = this;
        pDesc.iItemType = CItemIconUI::INVEN_BOMB;
        m_pItemIconUI = dynamic_cast<CUIObject*>(pGameObj->Clone(&pDesc));
        m_childUI_List.push_back(m_pItemIconUI);

    }

    return S_OK;
}

CInvenUI* CInvenUI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CInvenUI* pInstance = new CInvenUI(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CInvenUI"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CInvenUI::Clone(void* pArg)
{
    CInvenUI* pInstance = new CInvenUI(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CInvenUI"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CInvenUI::Free()
{
    __super::Free();
}
