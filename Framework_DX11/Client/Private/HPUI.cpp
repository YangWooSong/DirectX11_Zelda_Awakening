#include "stdafx.h"
#include "HPUI.h"
#include "GameInstance.h"
#include "Link.h"
#include "MainUI.h"
#include "HeartUI.h"

CHPUI::CHPUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CUIObject(pDevice, pContext)
{
}

CHPUI::CHPUI(const CHPUI& Prototype)
    :CUIObject(Prototype)
{
}

HRESULT CHPUI::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CHPUI::Initialize(void* pArg)
{
    CUIObject::UI_DESC* pDesc = static_cast<UI_DESC*>(pArg);

    /* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    m_iMaxHP = static_cast<CMainUI*>(m_pParentUI)->Get_Player_MaxHP();
    m_iCurHp = static_cast<CMainUI*>(m_pParentUI)->Get_Player_CurHp();

    if (FAILED(Ready_ChildUI()))
        return E_FAIL;

    Set_HeartUI();

    return S_OK;
}

void CHPUI::Priority_Update(_float fTimeDelta)
{
    for (auto& pChild : m_childUI_List)
        pChild->Priority_Update(fTimeDelta);
}

void CHPUI::Update(_float fTimeDelta)
{
    if(m_iCurHp > static_cast<CMainUI*>(m_pParentUI)->Get_Player_CurHp())
    {
        if(m_iCurHp -1 >= 0)
        {
            static_cast<CHeartUI*>(m_childUI_List[m_iCurHp - 1])->Set_Decrease(true);
            m_iCurHp--;
        }
    }
    if (m_iMaxHP < static_cast<CMainUI*>(m_pParentUI)->Get_Player_MaxHP())
    {
        Add_Hp();
    }
    for (auto& pChild : m_childUI_List)
        pChild->Update(fTimeDelta);
}

void CHPUI::Late_Update(_float fTimeDelta)
{
    for (auto& pChild : m_childUI_List)
        pChild->Late_Update(fTimeDelta);
}

HRESULT CHPUI::Render()
{
    for (auto& pChild : m_childUI_List)
        pChild->Render();
    return S_OK;
}

HRESULT CHPUI::Ready_ChildUI()
{
    CGameObject* pGameObj = m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_HeartUI"));
    CHeartUI::HEART_DESC pDesc{};

    if (pGameObj != nullptr)
    {
        pDesc.fSizeX = 30.f;
        pDesc.fSizeY = 30.f;
        pDesc.fX = g_iWinSizeX * 0.06f;
        pDesc.fY = g_iWinSizeY * 0.06f;
        pDesc.pParent = this;
        pDesc.bNew = false;
    }

    for(int i = 0; i < m_iMaxHP; ++i)
    { 
        CUIObject* m_pLupeeUI = dynamic_cast<CUIObject*>(pGameObj->Clone(&pDesc));
        m_childUI_List.push_back(m_pLupeeUI);

        pDesc.fX += 28.f;
    }
  
    return S_OK;
}

void CHPUI::Add_Hp()
{
    CGameObject* pGameObj = m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_HeartUI"));
    CHeartUI::HEART_DESC pDesc{};

    if (pGameObj != nullptr)
    {
        pDesc.fSizeX = 30.f;
        pDesc.fSizeY = 30.f;
        pDesc.fX = g_iWinSizeX * 0.06f + 224.f ;
        pDesc.fY = g_iWinSizeY * 0.06f;
        pDesc.pParent = this;
        pDesc.bNew = true;

        CUIObject* m_pLupeeUI = dynamic_cast<CUIObject*>(pGameObj->Clone(&pDesc));
        m_childUI_List.push_back(m_pLupeeUI);
    }

    m_iMaxHP++;

    while(m_iCurHp != m_iMaxHP-1)
    {
        static_cast<CHeartUI*>(m_childUI_List[ m_iCurHp])->Set_Increase(true);
        m_iCurHp++;
    }
  

}

void CHPUI::Set_HeartUI()
{
    if (m_iCurHp != m_iMaxHP)
    {
        int i = 0;

        for (auto& ui : m_childUI_List)
        {
            i++;
            if (i > m_iCurHp)
                static_cast<CHeartUI*>(ui)->Set_Decrease(true);
        }
    }
}


CHPUI* CHPUI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CHPUI* pInstance = new CHPUI(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CHPUI"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CHPUI::Clone(void* pArg)
{
    CHPUI* pInstance = new CHPUI(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CHPUI"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CHPUI::Free()
{
    __super::Free();
}
