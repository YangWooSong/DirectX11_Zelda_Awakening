#include "stdafx.h"
#include "MainUI.h"
#include"GameInstance.h"
#include "Link.h"

CMainUI::CMainUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CUIObject(pDevice, pContext)
{
}


CMainUI::CMainUI(const CMainUI& Prototype)
    :CUIObject(Prototype)
{
}

HRESULT CMainUI::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CMainUI::Initialize(void* pArg)
{
    MAINUI_DESC* pDesc = static_cast<MAINUI_DESC*>(pArg);
    m_iLevelIndex = pDesc->iLevelIndex;

    m_pPlayer = static_cast<CLink*>(m_pGameInstance->Find_Player(m_iLevelIndex));

    /* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Child_UI()))
        return E_FAIL;

    return S_OK;
}

void CMainUI::Priority_Update(_float fTimeDelta)
{
    for (auto& pChild : m_childUI_List)
        pChild->Priority_Update(fTimeDelta);
}

void CMainUI::Update(_float fTimeDelta)
{
    for (auto& pChild : m_childUI_List)
        pChild->Update(fTimeDelta);
}

void CMainUI::Late_Update(_float fTimeDelta)
{
    for (auto& pChild : m_childUI_List)
        pChild->Late_Update(fTimeDelta);
}

HRESULT CMainUI::Render()
{
    for (auto& pChild : m_childUI_List)
        pChild->Render();

    return S_OK;
}

_int CMainUI::Get_Player_Lupee()
{
    return m_pPlayer->Get_Lupee(); 
}

_int CMainUI::Get_Player_MaxHP()
{
    return m_pPlayer->Get_Player_MaxHP();
}

_int CMainUI::Get_Player_CurHp()
{
    return m_pPlayer->Get_Player_CurrHP();
}

_int CMainUI::Get_Player_KeyCount()
{
    return m_pPlayer->Get_SmallKeyCount();
}

_bool CMainUI::Get_Player_Get_BossKey()
{
    return m_pPlayer->Get_isGet_BossKey();
}

HRESULT CMainUI::Ready_Child_UI()
{
    CGameObject* pGameObj = m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_LupeeUI"));
    if (pGameObj != nullptr)
    {
        CUIObject::UI_DESC pDesc{};
        pDesc.fSizeX = 30.f ;
        pDesc.fSizeY = 45.f ;
        pDesc.fX = g_iWinSizeX * 0.9f;
        pDesc.fY = g_iWinSizeY * 0.2f;
        pDesc.pParent = this;

        CUIObject* m_pLupeeUI = dynamic_cast<CUIObject*>(pGameObj->Clone(&pDesc));
        m_childUI_List.push_back(m_pLupeeUI);
    } 
    
    pGameObj = m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_HPUI"));
    if (pGameObj != nullptr)
    {
        CUIObject::UI_DESC pDesc{};
        pDesc.pParent = this;

        CUIObject* m_pHPUI = dynamic_cast<CUIObject*>(pGameObj->Clone(&pDesc));
        m_childUI_List.push_back(m_pHPUI);
    }

    pGameObj = m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_KeyUI"));
    if (pGameObj != nullptr)
    {
        CUIObject::UI_DESC pDesc{};
        pDesc.pParent = this;

        CUIObject* m_pKeyUI = dynamic_cast<CUIObject*>(pGameObj->Clone(&pDesc));
        m_childUI_List.push_back(m_pKeyUI);
    }
    
    pGameObj = m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_CampusUI"));
    if (pGameObj != nullptr)
    {
        CUIObject::UI_DESC pDesc{};
        pDesc.fSizeX = 83.f;
        pDesc.fSizeY = 83.f;
        pDesc.fX = g_iWinSizeX * 0.97f;
        pDesc.fY = g_iWinSizeY * 0.2f;
        pDesc.pParent = this;

        CUIObject* m_pKeyUI = dynamic_cast<CUIObject*>(pGameObj->Clone(&pDesc));
        m_childUI_List.push_back(m_pKeyUI);
    }

    return S_OK;
}

CMainUI* CMainUI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CMainUI* pInstance = new CMainUI(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CMainUI"));
        Safe_Release(pInstance);
    }

    return pInstance;
}


CGameObject* CMainUI::Clone(void* pArg)
{
    CMainUI* pInstance = new CMainUI(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CMainUI"));
        Safe_Release(pInstance);
    }

    return pInstance;
}
void CMainUI::Free()
{
    __super::Free();

}
