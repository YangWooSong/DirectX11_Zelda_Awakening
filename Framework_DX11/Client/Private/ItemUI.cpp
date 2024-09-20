#include "stdafx.h"
#include "ItemUI.h"
#include "GameInstance.h"
#include "Link.h"

CItemUI::CItemUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CUIObject(pDevice, pContext)
{
}

CItemUI::CItemUI(const CItemUI& Prototype)
    :CUIObject(Prototype)
{
}

HRESULT CItemUI::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CItemUI::Initialize(void* pArg)
{
    CUIObject::UI_DESC* pDesc = static_cast<UI_DESC*>(pArg);

    /* ���������� ���� �����͵��� ��� ���ϳ�. */
    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_isActive = false;

    m_pTransformCom->Set_Scaled(1.f, 1.f, 1.f);

    return S_OK;
}

void CItemUI::Priority_Update(_float fTimeDelta)
{

}

void CItemUI::Update(_float fTimeDelta)
{
    if(m_isActive)
    {
       
    }
}

void CItemUI::Late_Update(_float fTimeDelta)
{
    if (m_isActive)
    {

        _vector vNewPos = m_pParentGameObj->Get_Transform()->Get_State(CTransform::STATE_POSITION);
        vNewPos = XMVectorSetZ(vNewPos, XMVectorGetZ(vNewPos) + 2.f);

        m_pTransformCom->Set_State(CTransform::STATE_POSITION, vNewPos);
        m_pTransformCom->BillBoard(static_cast<CLink*>(m_pParentGameObj)->Get_LevelIndex());

        m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);
    }
}

HRESULT CItemUI::Render()
{
    if (m_isActive)
    {
        if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
            return E_FAIL;
        if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
            return E_FAIL;

        if (FAILED(m_pTextureCom->Bind_ShadeResource(m_pShaderCom, "g_Texture", m_iTextureNum)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(2)))
            return E_FAIL;

        if (FAILED(m_pVIBufferCom->Bind_Buffers()))
            return E_FAIL;

        if (FAILED(m_pVIBufferCom->Render()))
            return E_FAIL;

    }

    return S_OK;
}

HRESULT CItemUI::Ready_Components()
{
    /* FOR.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_ItemIcon"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    /* FOR.Com_VIBuffer */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    return S_OK;
}

CItemUI* CItemUI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CItemUI* pInstance = new CItemUI(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CItemUI"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CItemUI::Clone(void* pArg)
{
    CItemUI* pInstance = new CItemUI(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CItemUI"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CItemUI::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
}
