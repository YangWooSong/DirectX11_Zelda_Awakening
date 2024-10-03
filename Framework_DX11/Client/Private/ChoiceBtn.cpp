#include "stdafx.h"
#include "ChoiceBtn.h"
#include "GameInstance.h"

CChoiceBtn::CChoiceBtn(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CUIObject(pDevice, pContext)
{
}

CChoiceBtn::CChoiceBtn(const CChoiceBtn& Prototype)
    :CUIObject(Prototype)
{
}

HRESULT CChoiceBtn::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CChoiceBtn::Initialize(void* pArg)
{
   CChoiceBtn::CHOICE_BTN_DESC* pDesc = static_cast<CHOICE_BTN_DESC*>(pArg);
    m_iTextureNum = pDesc->iTextureNum;

    /* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_isActive = false;

    return S_OK;
}
void CChoiceBtn::Priority_Update(_float fTimeDelta)
{
}

void CChoiceBtn::Update(_float fTimeDelta)
{
    if (m_isActive)
    {
        m_fAlpha = min(1.f, m_fAlpha + fTimeDelta * 1.f);
    }
    else
        m_fAlpha = 0.f;
}

void CChoiceBtn::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);

    m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);
    
}

HRESULT CChoiceBtn::Render()
{

    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float))))
        return E_FAIL;

    if(m_bSelected)
    {
        if (FAILED(m_pTextureCom[1]->Bind_ShadeResource(m_pShaderCom, "g_Texture", m_iTextureNum)))
            return E_FAIL;
    }
    else
    {
        if (FAILED(m_pTextureCom[0]->Bind_ShadeResource(m_pShaderCom, "g_Texture", m_iTextureNum)))
            return E_FAIL;
    }

    if (FAILED(m_pShaderCom->Begin(4)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Buffers()))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    

    return S_OK;
}

HRESULT CChoiceBtn::Ready_Components()
{
    /* FOR.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_ChoiceBtn"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom[0]))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_ChoiceBtn_Selected"),
        TEXT("Com_Texture1"), reinterpret_cast<CComponent**>(&m_pTextureCom[1]))))
        return E_FAIL;

    /* FOR.Com_VIBuffer */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    return S_OK;
}

CChoiceBtn* CChoiceBtn::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CChoiceBtn* pInstance = new CChoiceBtn(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CChoiceBtn"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CChoiceBtn::Clone(void* pArg)
{
    CChoiceBtn* pInstance = new CChoiceBtn(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CChoiceBtn"));
        Safe_Release(pInstance);
    }

    return pInstance;
}
void CChoiceBtn::Free()
{
    __super::Free();

    for (auto& pTectureCom : m_pTextureCom)
        Safe_Release(pTectureCom);

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pVIBufferCom);
}
