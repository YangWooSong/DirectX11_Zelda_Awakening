#include "stdafx.h"
#include "MapUI.h"
#include "GameInstance.h"
#include "MainUI.h"

CMapUI::CMapUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CUIObject(pDevice, pContext)
{
}

CMapUI::CMapUI(const CMapUI& Prototype)
    :CUIObject(Prototype)
{
}

HRESULT CMapUI::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CMapUI::Initialize(void* pArg)
{
    MAP_DESC* pDesc = static_cast<MAP_DESC*>(pArg);
    m_iMapType = pDesc->iMapType;
    m_iMapNUm = pDesc->iMapNum;

    /* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_isActive = false;
    m_iDepth = 4;

    Set_RoomNum();

    return S_OK;
}

void CMapUI::Priority_Update(_float fTimeDelta)
{
}

void CMapUI::Update(_float fTimeDelta)
{

}

void CMapUI::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);
    m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CMapUI::Render()
{

    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;

    if(m_iMapType == MASK)
    {
        if (FAILED(m_pTextureCom[0]->Bind_ShadeResource(m_pShaderCom, "g_Texture", m_iMapNUm)))
            return E_FAIL;
    }
    else
    {
        if (FAILED(m_pTextureCom[1]->Bind_ShadeResource(m_pShaderCom, "g_Texture", m_iMapNUm)))
            return E_FAIL;
    }

    if (FAILED(m_pShaderCom->Begin(0)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Buffers()))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;


    return S_OK;
}

void CMapUI::Set_RoomNum()
{
    switch (m_iMapNUm)
    {
    case 0:
        m_iRoomNum = 13;
        break;
    case 1:
        m_iRoomNum = 15;
        break;
    case 2:
    case 3:
        m_iRoomNum =14;
        break;
    case 4:
        m_iRoomNum =12;
        break;
    case 7:
    case 8:
    case 9:
    case 15:
    case 16:
    case 17:
        m_iRoomNum = 7;
        break;
    case 10:
        m_iRoomNum = 9;
        break;
    case 11:
        m_iRoomNum = 11;
        break;
    case 18:
        m_iRoomNum = 8;
        break;
    case 19:
        m_iRoomNum = 10;
        break;
    case 22:
        m_iRoomNum = 4;
        break;
    case 23:
        m_iRoomNum = 5;
        break;
    case 24:
        m_iRoomNum = 3;
        break;
    case 25:
        m_iRoomNum = 2;
        break;
    case 26:
        m_iRoomNum = 1;
        m_iMapType = REVEAL;
        break;
    default:
        break;
    }
}

HRESULT CMapUI::Ready_Components()
{
    /* FOR.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MapMask"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom[0]))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MapReveal"),
        TEXT("Com_Texture1"), reinterpret_cast<CComponent**>(&m_pTextureCom[1]))))
        return E_FAIL;

    /* FOR.Com_VIBuffer */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    return S_OK;
}


CMapUI* CMapUI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CMapUI* pInstance = new CMapUI(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CMapUI"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CMapUI::Clone(void* pArg)
{
    CMapUI* pInstance = new CMapUI(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CMapUI"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMapUI::Free()
{
    __super::Free();

    for (auto& p : m_pTextureCom)
        Safe_Release(p);

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pVIBufferCom);
}
