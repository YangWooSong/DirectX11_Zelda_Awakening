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

    /* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_isActive = false;

    m_pTransformCom->Set_Scaled(m_MinSize.x, m_MinSize.y, m_MinSize.z);

    return S_OK;
}

void CItemUI::Priority_Update(_float fTimeDelta)
{

}

void CItemUI::Update(_float fTimeDelta)
{
    if(m_isActive)
    {
        if(m_bShow == false)
        {
            //처음 작동
            m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pParentGameObj->Get_Transform()->Get_State(CTransform::STATE_POSITION));
            m_bShow = true;
            m_bOff = false;
        }
        _vector vNewPos = m_pParentGameObj->Get_Transform()->Get_State(CTransform::STATE_POSITION);
        m_TargetPos = XMVectorSetZ(vNewPos, XMVectorGetZ(vNewPos) + 2.5f);

        Lerp_Size(fTimeDelta);
        Lerp_Pos(fTimeDelta);
    }
    else
    {
        if (m_bOff == false)
        {
            //처음 꺼질 때
            m_bOff = true;
            m_pTransformCom->Set_Scaled(m_MinSize.x, m_MinSize.y, m_MinSize.z);
            m_bShow = false;
            m_bSizeDown = false;
        }
    }
}

void CItemUI::Late_Update(_float fTimeDelta)
{
    if (m_isActive)
    {
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

void CItemUI::Lerp_Size(_float fTimeDelta)
{
    _float3 fCurSize = m_pTransformCom->Get_Scaled();

    _float fAmout = fTimeDelta * 2.f;

    if (fAmout + fCurSize.x <= 2.f && m_bSizeDown == false)
        m_pTransformCom->Set_Scaled(fCurSize.x + fAmout, fCurSize.y + fAmout, fCurSize.z + fAmout);
    else
        m_bSizeDown = true;

    if (m_bSizeDown)
    {
        if (fAmout + fCurSize.x >= 1.8f)
            m_pTransformCom->Set_Scaled(fCurSize.x - fAmout, fCurSize.y - fAmout, fCurSize.z - fAmout);
    }

}

void CItemUI::Lerp_Pos(_float fTimeDelta)
{
    _float fLerpSpeed = min(1.0f, 5.f * fTimeDelta);
    _vector vNewPos = XMVectorLerp(m_pTransformCom->Get_State(CTransform::STATE_POSITION), m_TargetPos, fLerpSpeed);
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, vNewPos);
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
