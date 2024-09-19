#include "stdafx.h"
#include "InteractUI.h"
#include "GameInstance.h"

CInteractUI::CInteractUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CUIObject(pDevice, pContext)
{
}

CInteractUI::CInteractUI(const CInteractUI& Prototype)
    :CUIObject(Prototype)
{
}

HRESULT CInteractUI::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CInteractUI::Initialize(void* pArg)
{
    CUIObject::UI_DESC* pDesc = static_cast<UI_DESC*>(pArg);

    /* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_isActive = false;

    return S_OK;
}

void CInteractUI::Priority_Update(_float fTimeDelta)
{

}

void CInteractUI::Update(_float fTimeDelta)
{
    _matrix vPlayerPos = m_pParentGameObj->Get_Transform()->Get_WorldMatrix();
    vPlayerPos = vPlayerPos * XMLoadFloat4x4(&m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW));
  //  vPlayerPos = vPlayerPos * XMLoadFloat4x4(&m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ));

    _float4 vPlayer;
    XMStoreFloat4(&vPlayer, vPlayerPos.r[3]);
    
    _float3 vNewPos = { vPlayer.x, vPlayer.y, 0.f};

    m_fX =  vNewPos.x * m_fViewWidth/18.f + m_fViewWidth / 2.f + 20.f;
    m_fY = -vNewPos.y * m_fViewHeight /10.f + m_fViewHeight / 2.f + 20.f;

  //  float screenX = ((clipPos.m128_f32[0] + 1.0f) / 2.0f) * screenWidth;
 //   float screenY = ((1.0f - clipPos.m128_f32[1]) / 2.0f) * screenHeight;
}

void CInteractUI::Late_Update(_float fTimeDelta)
{

    if (m_isActive)
    {
        __super::Late_Update(fTimeDelta);
        m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);
    }
}

HRESULT CInteractUI::Render()
{
    if(m_isActive)
    {
        if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
            return E_FAIL;

        if (FAILED(m_pTextureCom->Bind_ShadeResource(m_pShaderCom, "g_Texture", m_iTextureNum)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(0)))
            return E_FAIL;
        
        if (FAILED(m_pVIBufferCom->Bind_Buffers()))
            return E_FAIL;

        if (FAILED(m_pVIBufferCom->Render()))
            return E_FAIL;

    }

    return S_OK;
}

HRESULT CInteractUI::Ready_Components()
{
    /* FOR.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_OpenBox"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    /* FOR.Com_VIBuffer */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    return S_OK;
}

CInteractUI* CInteractUI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CInteractUI* pInstance = new CInteractUI(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CInteractUI"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CInteractUI::Clone(void* pArg)
{
    CInteractUI* pInstance = new CInteractUI(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CInteractUI"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CInteractUI::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
}
