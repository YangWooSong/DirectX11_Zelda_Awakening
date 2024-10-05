#include "stdafx.h"
#include "MiniMap.h"
#include "GameInstance.h"
#include "MainUI.h"
#include "MapBackGround.h"
#include "MapUI.h"
#include"ItemIconUI.h"

CMiniMap::CMiniMap(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CUIObject(pDevice, pContext)
{
}

CMiniMap::CMiniMap(const CMiniMap& Prototype)
    :CUIObject(Prototype)
{
}

HRESULT CMiniMap::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CMiniMap::Initialize(void* pArg)
{
    CUIObject::UI_DESC* pDesc = static_cast<UI_DESC*>(pArg);

    /* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_ChildUI()))
        return E_FAIL;


    m_isActive = false;

    return S_OK;
}

void CMiniMap::Priority_Update(_float fTimeDelta)
{
    if (m_isActive)
    {
        for (auto& pChild : m_childUI_List)
            pChild->Priority_Update(fTimeDelta);
    }
}

void CMiniMap::Update(_float fTimeDelta)
{
    if (static_cast<CMainUI*>(m_pParentUI)->Get_Map())
    {
        if (KEY_TAP(KEY::Q))
            m_isActive = !m_isActive;
    }

    if (m_isActive)
    {
        for (auto& pChild : m_childUI_List)
            pChild->Update(fTimeDelta);
    }
}

void CMiniMap::Late_Update(_float fTimeDelta)
{
    if (m_isActive)
    {
        for (auto& pChild : m_childUI_List)
            pChild->Late_Update(fTimeDelta);
    }
}

HRESULT CMiniMap::Render()
{
    if (m_isActive)
    {
        for (auto& pChild : m_childUI_List)
            pChild->Render();
    }
    return S_OK;
}

HRESULT CMiniMap::Ready_ChildUI()
{
    CGameObject* pGameObj = m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_MapBackGround"));
    CMapBackGround::MAP_BACK_DESC pDesc{};

    if (pGameObj != nullptr)
    {
        pDesc.fSizeX = g_iWinSizeX;
        pDesc.fSizeY = g_iWinSizeY;
        pDesc.fX = g_iWinSizeX * 0.5f;
        pDesc.fY = g_iWinSizeY * 0.5f;
        pDesc.pParent = this;
        pDesc.iBackType = CMapBackGround::BACKGROUND;
        CUIObject* m_pItemIconUI = dynamic_cast<CUIObject*>(pGameObj->Clone(&pDesc));
        m_childUI_List.push_back(m_pItemIconUI);

        pDesc.fSizeX = g_iWinSizeX * 0.7f;
        pDesc.fSizeY = g_iWinSizeY * 0.8f;
        pDesc.pParent = this;
        pDesc.iBackType = CMapBackGround::GRID_MAP;
        m_pItemIconUI = dynamic_cast<CUIObject*>(pGameObj->Clone(&pDesc));
        m_childUI_List.push_back(m_pItemIconUI);

    }

  pGameObj = m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_MapUI"));
    CMapUI::MAP_DESC MapDesc{};
    if (pGameObj != nullptr)
    {
        MapDesc.fSizeX = 112.f;
        MapDesc.fSizeY = 72.f;
        MapDesc.fX = 915.f;
        MapDesc.fY = 182.f;
        MapDesc.pParent = this;
        MapDesc.iMapNum = 0;
        MapDesc.iMapType = CMapUI::MASK;

        CUIObject* pMapUI = dynamic_cast<CUIObject*>(pGameObj->Clone(&MapDesc));
        m_childUI_List.push_back(pMapUI);

        MapDesc.fX -= 550.f;
        MapDesc.fY += MapDesc.fSizeY;
        MapDesc.iMapNum = 1;
        pMapUI = dynamic_cast<CUIObject*>(pGameObj->Clone(&MapDesc));
        m_childUI_List.push_back(pMapUI);

        MapDesc.fX += MapDesc.fSizeX;
        MapDesc.iMapNum = 2;
        pMapUI = dynamic_cast<CUIObject*>(pGameObj->Clone(&MapDesc));
        m_childUI_List.push_back(pMapUI);

        MapDesc.fX += MapDesc.fSizeX;
        MapDesc.iMapNum = 3;
        pMapUI = dynamic_cast<CUIObject*>(pGameObj->Clone(&MapDesc));
        m_childUI_List.push_back(pMapUI);

        MapDesc.fX += MapDesc.fSizeX * 3.f - 10.f;
        MapDesc.iMapNum = 4;
        pMapUI = dynamic_cast<CUIObject*>(pGameObj->Clone(&MapDesc));
        m_childUI_List.push_back(pMapUI);

        MapDesc.fX = 477.f;
        MapDesc.fY += MapDesc.fSizeY;
        MapDesc.iMapNum = 7;
        pMapUI = dynamic_cast<CUIObject*>(pGameObj->Clone(&MapDesc));
        m_childUI_List.push_back(pMapUI);

        for(int i = 0; i < 4;++i)
        {
            MapDesc.fX += MapDesc.fSizeX - 2.5f;
            MapDesc.iMapNum = 8 + i;
            pMapUI = dynamic_cast<CUIObject*>(pGameObj->Clone(&MapDesc));
            m_childUI_List.push_back(pMapUI);
        }

        MapDesc.fX = 367.f;
        MapDesc.fY += MapDesc.fSizeY -2.f;
        MapDesc.iMapNum = 14;
        pMapUI = dynamic_cast<CUIObject*>(pGameObj->Clone(&MapDesc));
        m_childUI_List.push_back(pMapUI);

        for (int i = 0; i < 5; ++i)
        {
            MapDesc.fX += MapDesc.fSizeX - 2.5f;
            MapDesc.iMapNum = 15 + i;
            pMapUI = dynamic_cast<CUIObject*>(pGameObj->Clone(&MapDesc));
            m_childUI_List.push_back(pMapUI);
        }

        MapDesc.fX = 586.f;
        MapDesc.fY += MapDesc.fSizeY - 2.f;
        MapDesc.iMapNum = 22;
        pMapUI = dynamic_cast<CUIObject*>(pGameObj->Clone(&MapDesc));
        m_childUI_List.push_back(pMapUI);

        MapDesc.fX += MapDesc.fSizeX - 3.f;
        MapDesc.iMapNum = 23;
        pMapUI = dynamic_cast<CUIObject*>(pGameObj->Clone(&MapDesc));
        m_childUI_List.push_back(pMapUI);

        MapDesc.fX = 367.f;
        MapDesc.fY += MapDesc.fSizeY - 2.f;
        MapDesc.iMapNum = 24;
        pMapUI = dynamic_cast<CUIObject*>(pGameObj->Clone(&MapDesc));
        m_childUI_List.push_back(pMapUI);

        MapDesc.fX += MapDesc.fSizeX - 2.5f;
        MapDesc.iMapNum = 25;
        pMapUI = dynamic_cast<CUIObject*>(pGameObj->Clone(&MapDesc));
        m_childUI_List.push_back(pMapUI);

        MapDesc.fX += MapDesc.fSizeX - 2.5f;
        MapDesc.iMapNum = 26;
        pMapUI = dynamic_cast<CUIObject*>(pGameObj->Clone(&MapDesc));
        m_childUI_List.push_back(pMapUI);
    }

    pGameObj = m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_ItemIconUI"));
    CItemIconUI::ITEM_ICON_DESC IconDesc{};

    if (pGameObj != nullptr)
    {
        IconDesc.fSizeX = 30.f;
        IconDesc.fSizeY = 30.f;
        IconDesc.fX = 320.f;
        IconDesc.fY = 560.f;
        IconDesc.pParent = this;
        IconDesc.iItemType = CItemIconUI::LINK;
        CUIObject* m_pItemIconUI = dynamic_cast<CUIObject*>(pGameObj->Clone(&IconDesc));
        m_childUI_List.push_back(m_pItemIconUI);
    }

    return S_OK;
}

CMiniMap* CMiniMap::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CMiniMap* pInstance = new CMiniMap(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CMiniMap"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CMiniMap::Clone(void* pArg)
{
    CMiniMap* pInstance = new CMiniMap(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CMiniMap"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMiniMap::Free()
{
    __super::Free();
}
