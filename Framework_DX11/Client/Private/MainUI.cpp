#include "stdafx.h"
#include "MainUI.h"
#include"GameInstance.h"
#include "Link.h"
#include "DialogueUI.h"
#include "FadeInOUt.h"
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

    if (m_bRenderLevelText || m_bRenderBossName)
    {
        if (m_fTextAlpha == 1.f)    //화면에 얼마나 띄웠는지 시간 기록
            m_fTextTimer += fTimeDelta;
        if (m_fTextAlpha == 0.f && m_fTextTimer != 0.f) //한번 떴다가 다시 사라졌을 때 초기화
        {
            m_bRenderLevelText = false;
            m_bRenderBossName = false;
            m_fTextTimer = 0.f;
        }

        if(m_fTextTimer > 1.5f)  //다시 흐려지기
            m_fTextAlpha = max(0.f, m_fTextAlpha - fTimeDelta);
        else
        {
            m_fTextAlpha = min(1.f, m_fTextAlpha + fTimeDelta); //진해지기
        }
    }

    if (m_bStartEnding && m_bEnding == false)
    {
        m_bEnding = true;
        m_childUI_List[FADE_IN_OUT]->Set_TextureNum(1);
        Active_FadeOut();
        m_pGameInstance->Stop_BGM();
        m_pGameInstance->Play_BGM(TEXT("0_Ending.wav"),1.f);
    }

  //  if (KEY_TAP(KEY::E))
  //{
  //    static_cast<CDialogueUI*>(m_childUI_List[DIALOGUE])->Set_OwnerType(CDialogueUI::MARIN);
  //    static_cast<CDialogueUI*>(m_childUI_List[DIALOGUE])->Set_LineNum(3);
  //   m_childUI_List[DIALOGUE]->SetActive(true);
  //}
}

void CMainUI::Late_Update(_float fTimeDelta)
{

    __super::Late_Update(fTimeDelta);
    m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);
    
    for (auto& pChild : m_childUI_List)
        pChild->Late_Update(fTimeDelta);
}

HRESULT CMainUI::Render()
{

    if(m_bRenderLevelText)
    {
        switch (m_iLevelIndex)
        {
        case LEVEL_FIELD:
            m_pGameInstance->Render_Center(TEXT("Font_Mallang30"), TEXT("메베의 마을"), XMVectorSet(g_iWinSizeX *0.14f, g_iWinSizeY * 0.2f, 0.f, 1.f), XMVectorSet(1.f, 1.f, 1.f, m_fTextAlpha), 0.f, XMVectorSet(0.f, 0.f, 0.f, 1.f), 1.f, true);
            break;
        case LEVEL_DUNGEON:
            m_pGameInstance->Render_Center(TEXT("Font_Mallang30"), TEXT("꼬리리의 동굴"), XMVectorSet(g_iWinSizeX * 0.5f, g_iWinSizeY * 0.45f, 0.f, 1.f), XMVectorSet(1.f, 1.f, 1.f, m_fTextAlpha), 0.f, XMVectorSet(0.f, 0.f, 0.f, 1.f), 1.f, true);
            break;
        case LEVEL_STORE:
            m_pGameInstance->Render_Center(TEXT("Font_Mallang30"), TEXT("마을 도구점"), XMVectorSet(g_iWinSizeX * 0.14f, g_iWinSizeY * 0.2f, 0.f, 1.f), XMVectorSet(1.f, 1.f, 1.f, m_fTextAlpha), 0.f, XMVectorSet(0.f, 0.f, 0.f, 1.f), 1.f, true);
            break;
        default:
            break;
        }
       
    }

    if(m_bRenderBossName)
        m_pGameInstance->Render_Center(TEXT("Font_Mallang48"), TEXT("거대 꼬리리"), XMVectorSet(g_iWinSizeX * 0.5f, g_iWinSizeY * 0.7f, 0.f, 1.f), XMVectorSet(1.f, 1.f, 1.f, m_fTextAlpha), 0.f, XMVectorSet(0.f, 0.f, 0.f, 1.f), 1.f, true);

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

_bool CMainUI::Get_Bomb()
{
    return m_pPlayer->Get_Bomb();
}

_bool CMainUI::Get_Feather()
{
    return m_pPlayer->Get_Feather();
}

_bool CMainUI::Get_Map()
{
    return m_pPlayer->Get_Map();
}

_float3 CMainUI::Get_PlayerPos_Float3()
{
    _float3 fPos{};

    XMStoreFloat3(&fPos, m_pPlayer->Get_Position());
    return fPos;
}

void CMainUI::Active_FadeIn()
{
    if(dynamic_cast<CFadeInOut*>(m_childUI_List[FADE_IN_OUT]) != nullptr)
        static_cast<CFadeInOut*>( m_childUI_List[FADE_IN_OUT])->FadeIn();
}

void CMainUI::Active_FadeOut()
{
    if (dynamic_cast<CFadeInOut*>(m_childUI_List[FADE_IN_OUT]) != nullptr)
        static_cast<CFadeInOut*>(m_childUI_List[FADE_IN_OUT])->FadeOut();
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

    pGameObj = m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_InvenUI"));
    if (pGameObj != nullptr)
    {
        CUIObject::UI_DESC pDesc{};
        pDesc.pParent = this;

        CUIObject* m_pInvenUI = dynamic_cast<CUIObject*>(pGameObj->Clone(&pDesc));
        m_childUI_List.push_back(m_pInvenUI);
    }
    
    pGameObj = m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_MiniMap"));
    if (pGameObj != nullptr)
    {
        CUIObject::UI_DESC pDesc{};
        pDesc.pParent = this;

        CUIObject* m_pInvenUI = dynamic_cast<CUIObject*>(pGameObj->Clone(&pDesc));
        m_childUI_List.push_back(m_pInvenUI);
    }

    pGameObj = m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_DialogueUI"));
    if (pGameObj != nullptr)
    {
        CDialogueUI::DIALOGUE_DESC UIDesc{};
        UIDesc.fSizeX = g_iWinSizeX / 2.f;
        UIDesc.fSizeY = g_iWinSizeY / 5.f;
        UIDesc.fX = g_iWinSizeX * 0.5f;
        UIDesc.fY = g_iWinSizeY * 0.75f;
      
        CUIObject* m_pDialogueUI = dynamic_cast<CUIObject*>(pGameObj->Clone(&UIDesc));
      //  m_pDialogueUI->Set_ParentObj(this);
        m_childUI_List.push_back(m_pDialogueUI);
    } 
    
    pGameObj = m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_FadeInOut"));
    if (pGameObj != nullptr)
    {
        CUIObject::UI_DESC pDesc{};
        pDesc.fSizeX = g_iWinSizeX ;
        pDesc.fSizeY = g_iWinSizeY;
        pDesc.fX = g_iWinSizeX * 0.5f;
        pDesc.fY = g_iWinSizeY * 0.5f;
        pDesc.pParent = this;

        CUIObject* m_pUI = dynamic_cast<CUIObject*>(pGameObj->Clone(&pDesc));
       // m_pUI->Set_ParentUI(this);
        m_childUI_List.push_back(m_pUI);
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
