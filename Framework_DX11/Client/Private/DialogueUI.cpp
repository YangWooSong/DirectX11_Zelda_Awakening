#include "stdafx.h"
#include "DialogueUI.h"
#include "GameInstance.h"
#include "ChoiceBtn.h"

CDialogueUI::CDialogueUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CUIObject(pDevice, pContext)
{
}

CDialogueUI::CDialogueUI(const CDialogueUI& Prototype)
    :CUIObject(Prototype)
{
}

HRESULT CDialogueUI::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CDialogueUI::Initialize(void* pArg)
{
    CDialogueUI::DIALOGUE_DESC* pDesc = static_cast<DIALOGUE_DESC*>(pArg);
    iOwnerType = pDesc->iOwnerType;

    /* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;
    
    if (FAILED(Ready_ChildUI()))
        return E_FAIL;

    m_isActive = false;

    return S_OK;
}

void CDialogueUI::Priority_Update(_float fTimeDelta)
{
    if (iOwnerType != LINK)
    {
       m_fSizeX = g_iWinSizeX / 2.f;
       m_fSizeY = g_iWinSizeY / 5.f;
       m_fX = g_iWinSizeX * 0.5f;
       m_fY = g_iWinSizeY * 0.75f;
    }  
    else
    {
        if(iLineIndex != HORN)
        {
            m_fSizeX = g_iWinSizeX / 1.6f;
            m_fSizeY = g_iWinSizeY / 3.8f;
            m_fX = g_iWinSizeX * 0.5f;
            m_fY = g_iWinSizeY * 0.8f;
        }
        else
        {
            m_fSizeX = g_iWinSizeX / 1.4f;
            m_fSizeY = g_iWinSizeY / 2.f;
            m_fX = g_iWinSizeX * 0.5f;
            m_fY = g_iWinSizeY * 0.8f;
        }
    }
}

void CDialogueUI::Update(_float fTimeDelta)
{
    if (m_isActive)
    {
        if (iLineIndex != 0 && iOwnerType == TOOTOOLSHOPKEEPER)
        {
            for (auto& m_pChild : m_childUI_List)
            {
                m_pChild->SetActive(true);
                m_pChild->Update(fTimeDelta);
            }
        }
    }
    else
    {
        for (auto& m_pChild : m_childUI_List)
            m_pChild->SetActive(false);
    }

    for (auto& m_pChild : m_childUI_List)
    {
        m_pChild->Update(fTimeDelta);
    }
}

void CDialogueUI::Late_Update(_float fTimeDelta)
{
    if (m_isActive)
    {
        if (iLineIndex != 0 && iOwnerType == TOOTOOLSHOPKEEPER)
        {
            for (auto& m_pChild : m_childUI_List)
                m_pChild->Late_Update(fTimeDelta);
        }

        __super::Late_Update(fTimeDelta);

        m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);
    }
}

HRESULT CDialogueUI::Render()
{
    if (m_isActive)
    {
        if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
            return E_FAIL;

        if(iOwnerType != LINK)
        {
             if (FAILED(m_pTextureCom->Bind_ShadeResource(m_pShaderCom, "g_Texture", 0)))
                  return E_FAIL;
        }
        else
        {
            if (iLineIndex != HORN)
            {
                if (FAILED(m_pTextureCom->Bind_ShadeResource(m_pShaderCom, "g_Texture", 1)))
                    return E_FAIL;
            }
            else
            {

                if (FAILED(m_pTextureCom->Bind_ShadeResource(m_pShaderCom, "g_Texture", 2)))
                    return E_FAIL;
            }
        }

        if (FAILED(m_pShaderCom->Begin(3)))
            return E_FAIL;

        if (FAILED(m_pVIBufferCom->Bind_Buffers()))
            return E_FAIL;

        if (FAILED(m_pVIBufferCom->Render()))
            return E_FAIL;


        Render_Text();
      
        if (iLineIndex != 0)
        {
            for (auto& m_pChild : m_childUI_List)
                m_pChild->Render();
        }
    }

    return S_OK;
}

HRESULT CDialogueUI::Render_Text()
{
    if(iOwnerType == TOOTOOLSHOPKEEPER)
    {
        m_pGameInstance->Render_Text(TEXT("Font_Gulim24"), TEXT("도구점 주인"), XMVectorSet(m_fX * 0.58f, m_fY * 0.86f, 0.f, 1.f), XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f, XMVectorSet(0.f, 0.f, 0.f, 1.f), 0.6f);

        switch (iLineIndex)
        {
        case 0:
            m_pGameInstance->Render_Text(TEXT("Font_Gulim24"),TEXT("사고싶은 물건을 골라와."), XMVectorSet(m_fX * 0.75f, m_fY*0.97f , 0.f, 1.f), XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f, XMVectorSet(0.f, 0.f, 0.f, 1.f), 0.8f);
            break;
        case 1:
            m_pGameInstance->Render_Text(TEXT("Font_Gulim24"), TEXT("폭탄은 "), XMVectorSet(m_fX * 0.8f, m_fY * 0.97f, 0.f, 1.f), XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f, XMVectorSet(0.f, 0.f, 0.f, 1.f), 0.8f);
            m_pGameInstance->Render_Text(TEXT("Font_Gulim24"), TEXT("          5루피 "), XMVectorSet(m_fX * 0.8f, m_fY * 0.97f, 0.f, 1.f), XMVectorSet(1.f, 0.5f, 0.5f, 1.f), 0.f, XMVectorSet(0.f, 0.f, 0.f, 1.f), 0.8f);
            m_pGameInstance->Render_Text(TEXT("Font_Gulim24"), TEXT("                 야."), XMVectorSet(m_fX * 0.8f, m_fY * 0.97f, 0.f, 1.f), XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f, XMVectorSet(0.f, 0.f, 0.f, 1.f), 0.8f);
            break;
        case 2:
            m_pGameInstance->Render_Text(TEXT("Font_Gulim24"), TEXT("하트는 "), XMVectorSet(m_fX * 0.8f, m_fY * 0.97f, 0.f, 1.f), XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f, XMVectorSet(0.f, 0.f, 0.f, 1.f), 0.8f);
            m_pGameInstance->Render_Text(TEXT("Font_Gulim24"), TEXT("         10루피 "), XMVectorSet(m_fX * 0.8f, m_fY * 0.97f, 0.f, 1.f), XMVectorSet(1.f, 0.5f, 0.5f, 1.f), 0.f, XMVectorSet(0.f, 0.f, 0.f, 1.f), 0.8f);
            m_pGameInstance->Render_Text(TEXT("Font_Gulim24"), TEXT("                  야."), XMVectorSet(m_fX * 0.8f, m_fY * 0.97f, 0.f, 1.f), XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f, XMVectorSet(0.f, 0.f, 0.f, 1.f), 0.8f);
            break;
        default:
            break;
        }
       
    }   
    else if (iOwnerType == DEGUTAIL)
    {
        m_pGameInstance->Render_Center(TEXT("Font_Gulim24"), TEXT("크르르!!크르르!!"), XMVectorSet(g_iWinSizeX * 0.5f, m_fY * 0.97f, 0.f, 1.f), XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f, XMVectorSet(0.f, 0.f, 0.f, 1.f), 0.8f, false);
        m_pGameInstance->Render_Center(TEXT("Font_Gulim24"), TEXT("침입자다! 방해꾼이다!"), XMVectorSet(g_iWinSizeX * 0.5f, g_iWinSizeY * 0.78f, 0.f, 1.f), XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f, XMVectorSet(0.f, 0.f, 0.f, 1.f), 0.8f, false);
    }
    else if (iOwnerType == LINK)
    {
        switch (iLineIndex)
        {
        case SWORD:
            m_pGameInstance->Render_Text(TEXT("Font_Gulim24"), TEXT(" 잃어버린 "), XMVectorSet(m_fX * 0.77f, m_fY * 0.95f, 0.f, 1.f), XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f, XMVectorSet(0.f, 0.f, 0.f, 1.f), 0.8f);
            m_pGameInstance->Render_Text(TEXT("Font_Gulim24"), TEXT("              검 "), XMVectorSet(m_fX * 0.77f, m_fY * 0.95f, 0.f, 1.f), XMVectorSet(1.f, 0.5f, 0.5f, 1.f), 0.f, XMVectorSet(0.f, 0.f, 0.f, 1.f), 0.8f);
            m_pGameInstance->Render_Text(TEXT("Font_Gulim24"), TEXT("                 을 찾았다! "), XMVectorSet(m_fX * 0.77f,  m_fY * 0.95f, 0.f, 1.f), XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f, XMVectorSet(0.f, 0.f, 0.f, 1.f), 0.8f);
            m_pGameInstance->Render_Text(TEXT("Font_Gulim24"), TEXT("  \"P\"로 휘둘러 보자! "), XMVectorSet(m_fX * 0.78f, m_fY * 1.f, 0.f, 1.f), XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f, XMVectorSet(0.f, 0.f, 0.f, 1.f), 0.8f);
            break;
        case SMALL_KEY:
            m_pGameInstance->Render_Text(TEXT("Font_Gulim24"), TEXT("작은 열쇠"), XMVectorSet(m_fX * 0.77f, m_fY * 0.95f, 0.f, 1.f), XMVectorSet(1.f, 0.5f, 0.5f, 1.f), 0.f, XMVectorSet(0.f, 0.f, 0.f, 1.f), 0.8f);
            m_pGameInstance->Render_Text(TEXT("Font_Gulim24"), TEXT("            를 손에 넣었다!"), XMVectorSet(m_fX * 0.77f, m_fY * 0.95f, 0.f, 1.f), XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f, XMVectorSet(0.f, 0.f, 0.f, 1.f), 0.8f);
            m_pGameInstance->Render_Text(TEXT("Font_Gulim24"), TEXT("이제 잠긴 문을 열 수 있다"), XMVectorSet(m_fX * 0.75f, m_fY * 1.f, 0.f, 1.f), XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f, XMVectorSet(0.f, 0.f, 0.f, 1.f), 0.8f);
            break;
        case CAMPUS:
            m_pGameInstance->Render_Text(TEXT("Font_Gulim24"), TEXT("나침반"), XMVectorSet(m_fX * 0.77f, m_fY * 0.95f, 0.f, 1.f), XMVectorSet(1.f, 0.5f, 0.5f, 1.f), 0.f, XMVectorSet(0.f, 0.f, 0.f, 1.f), 0.8f);
            m_pGameInstance->Render_Text(TEXT("Font_Gulim24"), TEXT("         을 손에 넣었다!"), XMVectorSet(m_fX * 0.77f, m_fY * 0.95f, 0.f, 1.f), XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f, XMVectorSet(0.f, 0.f, 0.f, 1.f), 0.8f);
            m_pGameInstance->Render_Text(TEXT("Font_Gulim24"), TEXT("열쇠가 근처에 있으면 소리로 알 수 있다"), XMVectorSet(m_fX * 0.65f, m_fY * 1.f, 0.f, 1.f), XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f, XMVectorSet(0.f, 0.f, 0.f, 1.f), 0.8f);
            break;
        case MAP:
            m_pGameInstance->Render_Text(TEXT("Font_Gulim24"), TEXT("드디어"), XMVectorSet(m_fX * 0.79f, m_fY * 0.95f, 0.f, 1.f), XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f, XMVectorSet(0.f, 0.f, 0.f, 1.f), 0.8f);
            m_pGameInstance->Render_Text(TEXT("Font_Gulim24"), TEXT("          지도"), XMVectorSet(m_fX * 0.79f, m_fY * 0.95f, 0.f, 1.f), XMVectorSet(1.f, 0.5f, 0.5f, 1.f), 0.f, XMVectorSet(0.f, 0.f, 0.f, 1.f), 0.8f);
            m_pGameInstance->Render_Text(TEXT("Font_Gulim24"), TEXT("                를 찾았다!"), XMVectorSet(m_fX * 0.79f, m_fY * 0.95f, 0.f, 1.f), XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f, XMVectorSet(0.f, 0.f, 0.f, 1.f), 0.8f);
            m_pGameInstance->Render_Text(TEXT("Font_Gulim24"), TEXT(" \"Q\"로 지도 화면을 열어 보자!"), XMVectorSet(m_fX * 0.7f, m_fY * 1.f, 0.f, 1.f), XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f, XMVectorSet(0.f, 0.f, 0.f, 1.f), 0.8f);
            break;
        case LUPEE:
            m_pGameInstance->Render_Text(TEXT("Font_Gulim24"), TEXT("20루피"), XMVectorSet(m_fX * 0.8f, m_fY * 0.95f, 0.f, 1.f), XMVectorSet(1.f, 0.5f, 0.5f, 1.f), 0.f, XMVectorSet(0.f, 0.f, 0.f, 1.f), 0.8f);
            m_pGameInstance->Render_Text(TEXT("Font_Gulim24"), TEXT("         를 손에 넣었다!"), XMVectorSet(m_fX * 0.8f, m_fY * 0.95f, 0.f, 1.f), XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f, XMVectorSet(0.f, 0.f, 0.f, 1.f), 0.8f);
            m_pGameInstance->Render_Text(TEXT("Font_Gulim24"), TEXT(" 조금 기쁘다"), XMVectorSet(m_fX *0.87f, m_fY * 1.f, 0.f, 1.f), XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f, XMVectorSet(0.f, 0.f, 0.f, 1.f), 0.8f);
            break;
        case STONEBEAK:
            m_pGameInstance->Render_Text(TEXT("Font_Gulim24"), TEXT("석상의 부리"), XMVectorSet(m_fX * 0.79f, m_fY * 0.95f, 0.f, 1.f), XMVectorSet(1.f, 0.5f, 0.5f, 1.f), 0.f, XMVectorSet(0.f, 0.f, 0.f, 1.f), 0.8f);
            m_pGameInstance->Render_Text(TEXT("Font_Gulim24"), TEXT("                를 찾았다!"), XMVectorSet(m_fX * 0.79f, m_fY * 0.95f, 0.f, 1.f), XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f, XMVectorSet(0.f, 0.f, 0.f, 1.f), 0.8f);
            m_pGameInstance->Render_Text(TEXT("Font_Gulim24"), TEXT("이제 부엉이 석상을 찾자"), XMVectorSet(m_fX * 0.77f, m_fY * 1.f, 0.f, 1.f), XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f, XMVectorSet(0.f, 0.f, 0.f, 1.f), 0.8f);
            break; 
        case FEATHER:
            m_pGameInstance->Render_Text(TEXT("Font_Gulim24"), TEXT("로크의 깃털"), XMVectorSet(m_fX * 0.75f, m_fY * 0.95f, 0.f, 1.f), XMVectorSet(1.f, 0.5f, 0.5f, 1.f), 0.f, XMVectorSet(0.f, 0.f, 0.f, 1.f), 0.8f);
            m_pGameInstance->Render_Text(TEXT("Font_Gulim24"), TEXT("                을 손에 넣었다!"), XMVectorSet(m_fX * 0.75f, m_fY * 0.95f, 0.f, 1.f), XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f, XMVectorSet(0.f, 0.f, 0.f, 1.f), 0.8f);
            m_pGameInstance->Render_Text(TEXT("Font_Gulim24"), TEXT("왠지 몸이 가벼워진 것 같다"), XMVectorSet(m_fX * 0.75f, m_fY * 1.f, 0.f, 1.f), XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f, XMVectorSet(0.f, 0.f, 0.f, 1.f), 0.8f);
            break;
        case BOSS_KEY:
            m_pGameInstance->Render_Text(TEXT("Font_Gulim24"), TEXT("보스의 열쇠"), XMVectorSet(m_fX * 0.76f, m_fY * 0.95f, 0.f, 1.f), XMVectorSet(1.f, 0.5f, 0.5f, 1.f), 0.f, XMVectorSet(0.f, 0.f, 0.f, 1.f), 0.8f);
            m_pGameInstance->Render_Text(TEXT("Font_Gulim24"), TEXT("                를 손에 넣었다!"), XMVectorSet(m_fX * 0.76f, m_fY * 0.95f, 0.f, 1.f), XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f, XMVectorSet(0.f, 0.f, 0.f, 1.f), 0.8f);
            m_pGameInstance->Render_Text(TEXT("Font_Gulim24"), TEXT("이제 보스의 방 문을 열 수 있다"), XMVectorSet(m_fX * 0.72f, m_fY * 1.f, 0.f, 1.f), XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f, XMVectorSet(0.f, 0.f, 0.f, 1.f), 0.8f);
            break;
        case HORN:
            m_pGameInstance->Render_Text(TEXT("Font_Gulim24"), TEXT("고둥의 호른"), XMVectorSet(m_fX * 0.78f, m_fY * 0.98f, 0.f, 1.f), XMVectorSet(1.f, 0.5f, 0.5f, 1.f), 0.f, XMVectorSet(0.f, 0.f, 0.f, 1.f), 0.8f);
            m_pGameInstance->Render_Text(TEXT("Font_Gulim24"), TEXT("               을 찾았다!"), XMVectorSet(m_fX * 0.78f, m_fY * 0.98f, 0.f, 1.f), XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f, XMVectorSet(0.f, 0.f, 0.f, 1.f), 0.8f);
            break;
        }
    }
    else if (iOwnerType == STATUE)
    {
        switch (iLineIndex)
        {
        case ROOM9:
            m_pGameInstance->Render_Text(TEXT("Font_Gulim24"), TEXT("날카로운 가시를 지닌 것"), XMVectorSet(m_fX * 0.77f, m_fY * 0.94f, 0.f, 1.f), XMVectorSet(0.49f, 0.98f, 1.f, 1.f), 0.f, XMVectorSet(0.f, 0.f, 0.f, 1.f), 0.8f);
            m_pGameInstance->Render_Text(TEXT("Font_Gulim24"), TEXT(" 방패로 튕겨 내라......"), XMVectorSet(m_fX * 0.78f, m_fY * 1.f, 0.f, 1.f), XMVectorSet(0.49f, 0.98f, 1.f, 1.f), 0.f, XMVectorSet(0.f, 0.f, 0.f, 1.f), 0.8f);
            break;
        case ROOM14:
            m_pGameInstance->Render_Text(TEXT("Font_Gulim24"), TEXT("닫힌 방의 문"), XMVectorSet(m_fX * 0.87f, m_fY * 0.94f, 0.f, 1.f), XMVectorSet(0.49f, 0.98f, 1.f, 1.f), 0.f, XMVectorSet(0.f, 0.f, 0.f, 1.f), 0.8f);
            m_pGameInstance->Render_Text(TEXT("Font_Gulim24"), TEXT("사각 블록을 움직여라"), XMVectorSet(m_fX * 0.79f, m_fY * 1.f, 0.f, 1.f), XMVectorSet(0.49f, 0.98f, 1.f, 1.f), 0.f, XMVectorSet(0.f, 0.f, 0.f, 1.f), 0.8f);
            break;
        }
    }
    return S_OK;
}

HRESULT CDialogueUI::Ready_Components()
{
    /* FOR.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Dialogue"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    /* FOR.Com_VIBuffer */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CDialogueUI::Ready_ChildUI()
{
 
    CGameObject* pGameObj = m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_ChoiceBtn"));
    if (pGameObj != nullptr)
    {
        CChoiceBtn::CHOICE_BTN_DESC Desc;
        Desc.fSizeX = 230.f;
        Desc.fSizeY = 60.f;
        Desc.fX = g_iWinSizeX * 0.85f;
        Desc.fY = g_iWinSizeY * 0.6f;
        Desc.iTextureNum = CChoiceBtn::BUY; 

        CUIObject* m_pChoiceBtn = dynamic_cast<CUIObject*>(pGameObj->Clone(&Desc));
        m_pChoiceBtn->Set_ParentObj(this);
        m_childUI_List.push_back(m_pChoiceBtn);

        Desc.fSizeX = 230.f;
        Desc.fSizeY = 60.f;
        Desc.fX = g_iWinSizeX * 0.85f;
        Desc.fY = g_iWinSizeY * 0.7f;
        Desc.iTextureNum = CChoiceBtn::NOTBUY;

        m_pChoiceBtn = dynamic_cast<CUIObject*>(pGameObj->Clone(&Desc));
        m_pChoiceBtn->Set_ParentObj(this);
        m_childUI_List.push_back(m_pChoiceBtn);

    }

    return S_OK;
}

CDialogueUI* CDialogueUI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CDialogueUI* pInstance = new CDialogueUI(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CDialogueUI"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CDialogueUI::Clone(void* pArg)
{
    CDialogueUI* pInstance = new CDialogueUI(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CDialogueUI"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CDialogueUI::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
}
