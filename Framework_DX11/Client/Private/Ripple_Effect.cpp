#include "stdafx.h"
#include "Ripple_Effect.h"
#include "GameInstance.h"
CRipple_Effect::CRipple_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :C2DEffects(pDevice, pContext)
{
}


CRipple_Effect::CRipple_Effect(const CRipple_Effect& Prototype)
    :C2DEffects(Prototype)
{
}

HRESULT CRipple_Effect::Initialize_Prototype()
{
      return S_OK;
}

HRESULT CRipple_Effect::Initialize(void* pArg)
{
    RIPPLE_DESC* pDesc = static_cast<RIPPLE_DESC*>(pArg);
    m_iTextureNum = pDesc->iTextureNum;

    /* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (m_iEffectType == PLAYER_ITEM_GET)
    {
        m_bSizeUp = true;
        m_bAlphaChange = true;
        m_fOffset = { 0.f, 1.5f, 0.f };
        m_fMaxSize = 3.f;
    }

    m_isActive = false;
    m_fOriSize = m_pTransformCom->Get_Scaled();
    m_fOriColor = m_fColor;
    return S_OK;
}

void CRipple_Effect::Priority_Update(_float fTimeDelta)
{
}

void CRipple_Effect::Update(_float fTimeDelta)
{
    if (m_isActive)
    {
        m_bReset = false;

        if (m_iEffectType == PLAYER_ITEM_GET)
        {
            AlphaDown(fTimeDelta);
            Lerp_Size_Up(fTimeDelta);
        }
        m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pParentObj->Get_Transform()->Get_State(CTransform::STATE_POSITION) + XMLoadFloat3(&m_fOffset));
    }
    else
    {
        if (m_bReset == false)
        {
            m_bReset = true;
            m_fColor = m_fOriColor;
            m_bSizeUp = true;
            m_pTransformCom->Set_Scaled(m_fOriSize.x, m_fOriSize.y, m_fOriSize.z);
        }
    }
}

void CRipple_Effect::Late_Update(_float fTimeDelta)
{
    if (m_isActive)
    {
       __super::Late_Update(fTimeDelta);
    }
}

HRESULT CRipple_Effect::Render()
{
    if (m_isActive)
    {
        if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fColor.w, sizeof(_float))))
            return E_FAIL;

        if (FAILED(m_pTextureCom->Bind_ShadeResource(m_pShaderCom, "g_Texture", m_iTextureNum)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(4)))
            return E_FAIL;

        if (FAILED(m_pVIBufferCom->Bind_Buffers()))
            return E_FAIL;

        if (FAILED(m_pVIBufferCom->Render()))
            return E_FAIL;

        _float fOne = 1.f;
        if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &fOne, sizeof(_float))))
            return E_FAIL;

    }

    return S_OK;
}

HRESULT CRipple_Effect::Ready_Components()
{
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Ripple"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    return S_OK;
}

void CRipple_Effect::AlphaDown(_float fTimeDelta)
{
    if (m_bAlphaChange)
    {
        m_fColor.w = max(0.f, m_fColor.w - fTimeDelta);
       // if (m_fColor.w == 0.f)
         //   m_bAlphaChange = false;
    }
}

void CRipple_Effect::Lerp_Size_Up(_float fTimeDelta)
{
    _float3 fCurSize = m_pTransformCom->Get_Scaled();

    _float fAmout = fTimeDelta * 3.f;

    if (m_bSizeUp == true)
    {
        _float fSize = min(m_fMaxSize, fCurSize.x + fAmout);
        m_pTransformCom->Set_Scaled(fCurSize.x + fAmout, fCurSize.y + fAmout, fCurSize.z + fAmout);

        if (fSize == m_fMaxSize)
        {
            m_bSizeUp = false;
        }
    }
}

CRipple_Effect* CRipple_Effect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CRipple_Effect* pInstance = new CRipple_Effect(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CRipple_Effect"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CRipple_Effect::Clone(void* pArg)
{

    CRipple_Effect* pInstance = new CRipple_Effect(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CRipple_Effect"));
        Safe_Release(pInstance);
    }

    return pInstance;
}


void CRipple_Effect::Free()
{
    __super::Free();
}
