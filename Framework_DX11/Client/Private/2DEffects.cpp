#include "stdafx.h"
#include "2DEffects.h"
#include "GameInstance.h"
#include "Link.h"

C2DEffects::C2DEffects(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CUIObject(pDevice, pContext)
{
}

C2DEffects::C2DEffects(const C2DEffects& Prototype)
    :CUIObject(Prototype)
{
}

HRESULT C2DEffects::Initialize_Prototype()
{
    return S_OK;
}

HRESULT C2DEffects::Initialize(void* pArg)
{
    /* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    EFFECT_DESC* pDesc = static_cast<EFFECT_DESC*>(pArg);
    if(pDesc != nullptr)
    {
        m_fColor = pDesc->fColor;
        m_iLevelIndex = pDesc->iLevelIndex;
        m_pParentObj = pDesc->pParent;
        m_iEffectType = pDesc->iEffectType;
        m_pTransformCom->Set_Scaled(pDesc->vScale.x,pDesc->vScale.y, pDesc->vScale.z);
    }
        
    m_isActive = false;


    return S_OK;
}

void C2DEffects::Priority_Update(_float fTimeDelta)
{

}

void C2DEffects::Update(_float fTimeDelta)
{
  
}

void C2DEffects::Late_Update(_float fTimeDelta)
{
  
   m_pTransformCom->BillBoard(m_iLevelIndex);
   m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);
    
}

HRESULT C2DEffects::Render()
{
    return S_OK;
}

HRESULT C2DEffects::Ready_Components()
{
    /* FOR.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* FOR.Com_VIBuffer */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    return S_OK;
}



C2DEffects* C2DEffects::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    C2DEffects* pInstance = new C2DEffects(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : C2DEffects"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* C2DEffects::Clone(void* pArg)
{
    C2DEffects* pInstance = new C2DEffects(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : C2DEffects"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void C2DEffects::Free()
{
    __super::Free();

    for (auto& iter : m_Child_List)
        Safe_Release(iter);
    m_Child_List.clear();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
}
