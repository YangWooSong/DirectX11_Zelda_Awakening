#include "stdafx.h"
#include "MapBackGround.h"
#include "GameInstance.h"
#include "MainUI.h"

CMapBackGround::CMapBackGround(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CUIObject(pDevice, pContext)
{
}

CMapBackGround::CMapBackGround(const CMapBackGround& Prototype)
    :CUIObject(Prototype)
{
}

HRESULT CMapBackGround::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CMapBackGround::Initialize(void* pArg)
{
    MAP_BACK_DESC* pDesc = static_cast<MAP_BACK_DESC*>(pArg);
    m_iBackType = pDesc->iBackType;

    /* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_isActive = false;
    m_iDepth = 3;
    return S_OK;
}

void CMapBackGround::Priority_Update(_float fTimeDelta)
{
}

void CMapBackGround::Update(_float fTimeDelta)
{

}

void CMapBackGround::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);
    m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CMapBackGround::Render()
{
   
        if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
            return E_FAIL;

        if (FAILED(m_pTextureCom->Bind_ShadeResource(m_pShaderCom, "g_Texture", 0)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(0)))
            return E_FAIL;

        if (FAILED(m_pVIBufferCom->Bind_Buffers()))
            return E_FAIL;

        if (FAILED(m_pVIBufferCom->Render()))
            return E_FAIL;

        if(m_iBackType == GRID_MAP)
            m_pGameInstance->Render_Text(TEXT("Font_BossName36"), TEXT("지도"), XMVectorSet(g_iWinSizeX * 0.5f - 50.f, 100.f, 0.f, 1.f), XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f, XMVectorSet(0.f, 0.f, 0.f, 1.f), 0.8f);
   
        return S_OK;
}

HRESULT CMapBackGround::Ready_Components()
{
    /* FOR.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    if(m_iBackType == BACKGROUND)
    {
        if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Map_Background"),
            TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
            return E_FAIL;
    }
    else  if (m_iBackType == GRID_MAP)
    {
        if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_DgnMapGrid"),
            TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
            return E_FAIL;
    }

    /* FOR.Com_VIBuffer */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    return S_OK;
}


CMapBackGround* CMapBackGround::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CMapBackGround* pInstance = new CMapBackGround(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CMapBackGround"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CMapBackGround::Clone(void* pArg)
{
    CMapBackGround* pInstance = new CMapBackGround(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CMapBackGround"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMapBackGround::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
}
