#include "stdafx.h"
#include "KeyUI.h"
#include "GameInstance.h"
#include "Link.h"
#include "MainUI.h"
#include "ItemIconUI.h"

CKeyUI::CKeyUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CUIObject(pDevice, pContext)
{
}

CKeyUI::CKeyUI(const CKeyUI& Prototype)
    :CUIObject(Prototype)
{
}

HRESULT CKeyUI::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CKeyUI::Initialize(void* pArg)
{
    CUIObject::UI_DESC* pDesc = static_cast<UI_DESC*>(pArg);

    /* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_ChildUI()))
        return E_FAIL;

    if(static_cast<CMainUI*>(m_pParentUI)->Get_LevelIndex() == LEVEL_DUNGEON)
        m_isActive = true;
    else
        m_isActive = false;
    return S_OK;
}

void CKeyUI::Priority_Update(_float fTimeDelta)
{
    if(m_isActive)
    {
        for (auto& pChild : m_childUI_List)
            pChild->Priority_Update(fTimeDelta);
    }
}

void CKeyUI::Update(_float fTimeDelta)
{
    if (m_isActive)
    {
        if (static_cast<CMainUI*>(m_pParentUI)->Get_Player_KeyCount() == 2)
        {
            m_childUI_List[SMALL1]->SetActive(true);
            m_childUI_List[SMALL2]->SetActive(true);
        }
        else if (static_cast<CMainUI*>(m_pParentUI)->Get_Player_KeyCount() == 1)
        {
            m_childUI_List[SMALL1]->SetActive(true);
            m_childUI_List[SMALL2]->SetActive(false);
        }
        else
        {
            m_childUI_List[SMALL1]->SetActive(false);
            m_childUI_List[SMALL2]->SetActive(false);
        }

        if(static_cast<CMainUI*>(m_pParentUI)->Get_Player_Get_BossKey())
            m_childUI_List[BOSS]->SetActive(true);
        else
            m_childUI_List[BOSS]->SetActive(false);

        for (auto& pChild : m_childUI_List)
            pChild->Update(fTimeDelta);
    }
}

void CKeyUI::Late_Update(_float fTimeDelta)
{
    if (m_isActive)
    {
        for (auto& pChild : m_childUI_List)
            pChild->Late_Update(fTimeDelta);
    }
}

HRESULT CKeyUI::Render()
{
    if (m_isActive)
    {
        for (auto& pChild : m_childUI_List)
            pChild->Render();
    }
    return S_OK;
}

HRESULT CKeyUI::Ready_ChildUI()
{
    CGameObject* pGameObj = m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_ItemIconUI"));
    CItemIconUI::ITEM_ICON_DESC pDesc{};

    if (pGameObj != nullptr)
    {
        pDesc.fSizeX = 38.f;
        pDesc.fSizeY = 38.f;
        pDesc.fX = g_iWinSizeX * 0.06f;
        pDesc.fY = g_iWinSizeY * 0.11f;
        pDesc.pParent = this;
        pDesc.iItemType = CItemIconUI::SMALL_KEY;
        CUIObject* m_pItemIconUI = dynamic_cast<CUIObject*>(pGameObj->Clone(&pDesc));
        m_childUI_List.push_back(m_pItemIconUI);

        pDesc.fSizeX = 38.f;
        pDesc.fSizeY = 38.f;
        pDesc.fX += 36.f;
        pDesc.pParent = this;
        pDesc.iItemType = CItemIconUI::SMALL_KEY;
        m_pItemIconUI = dynamic_cast<CUIObject*>(pGameObj->Clone(&pDesc));
        m_childUI_List.push_back(m_pItemIconUI);

        pDesc.fSizeX = 38.f;
        pDesc.fSizeY = 36.f;
        pDesc.fX = g_iWinSizeX * 0.06f;
        pDesc.pParent = this;
        pDesc.iItemType = CItemIconUI::BOSS_KEY;
        m_pItemIconUI = dynamic_cast<CUIObject*>(pGameObj->Clone(&pDesc));
        m_childUI_List.push_back(m_pItemIconUI);

    }

    return S_OK;
}

CKeyUI* CKeyUI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CKeyUI* pInstance = new CKeyUI(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CKeyUI"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CKeyUI::Clone(void* pArg)
{
    CKeyUI* pInstance = new CKeyUI(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CKeyUI"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CKeyUI::Free()
{
    __super::Free();
}
