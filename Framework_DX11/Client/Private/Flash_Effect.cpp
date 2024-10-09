#include "stdafx.h"
#include "Flash_Effect.h"
#include "GameInstance.h"

CFlash_Effect::CFlash_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :C2DEffects(pDevice, pContext)
{
}

CFlash_Effect::CFlash_Effect(const CFlash_Effect& Prototype)
    :C2DEffects(Prototype)
{
}


HRESULT CFlash_Effect::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CFlash_Effect::Initialize(void* pArg)
{
    FLASH_DESC* pDesc = static_cast<FLASH_DESC*>(pArg);
    m_iTextureNum = pDesc->iTextureNum;

    /* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if(m_iEffectType ==BOMB_FUSE)
    {
        m_fOffset = { 0.f, 0.8f,0.f };
    }
    if (m_iEffectType == PLAYER_ITEM_GET)
    {
        m_iDepth = 2;
        m_fOffset = { 0.f, 1.3f, 0.f };
        m_bAlphaDown = true;
        m_fSpeed = 0.5f;

    }
    else
    {
        m_iDepth = 2;
        m_bAlphaDown = true;
        m_fSpeed = 3.f;
    }
    m_fOriColor = m_fColor;
    m_isActive = false;
    return S_OK;
}

void CFlash_Effect::Priority_Update(_float fTimeDelta)
{
}

void CFlash_Effect::Update(_float fTimeDelta)
{
    if (m_isActive)
    {
        m_bReset = false;

        if (m_iEffectType == BOMB_FUSE)
        {
            Lerp_Size(fTimeDelta);
            m_fAngle = min(360.f, m_fAngle + fTimeDelta * 100.f);

            if (m_fAngle == 360.f)
                m_fAngle = 0.f;
        }
        if (m_iEffectType == BOMB_EXPLOSIONT)
            AlphaDown(fTimeDelta);
        if (m_iEffectType == PLAYER_ITEM_GET)
        {
           /* AlphaDown(fTimeDelta);
            Change_Color(fTimeDelta);*/

            m_fAngle = min(360.f, m_fAngle + fTimeDelta * 100.f);

            if (m_fAngle == 360.f)
                m_fAngle = 0.f;
        }

        m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pParentObj->Get_Transform()->Get_State(CTransform::STATE_POSITION) + XMLoadFloat3(&m_fOffset));
    }
    else
    {
        if (m_bReset == false)
        {
            m_bReset = true;
            m_fColor = m_fOriColor;
        }
    }
}

void CFlash_Effect::Late_Update(_float fTimeDelta)
{
    if (m_isActive)
    {
        if (m_iEffectType == BOMB_FUSE || m_iEffectType == PLAYER_ITEM_GET)
        {
            m_pTransformCom->BillBoard_RotZ(m_iLevelIndex, m_fAngle);
            m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);
        }
        else
            __super::Late_Update(fTimeDelta);
    }
}

HRESULT CFlash_Effect::Render()
{
    if (m_isActive)
    {
        if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_RawValue("g_fColor", &m_fColor, sizeof(_float4))))
            return E_FAIL;

        if (FAILED(m_pTextureCom->Bind_ShadeResource(m_pShaderCom, "g_Texture", m_iTextureNum)))
            return E_FAIL;

        if (m_iEffectType == BOMB_FUSE)
        {
            if (FAILED(m_pShaderCom->Begin(8)))
                return E_FAIL;
        }
        else
        {
            if (FAILED(m_pShaderCom->Begin(7)))
                return E_FAIL;
        }

        if (FAILED(m_pVIBufferCom->Bind_Buffers()))
            return E_FAIL;

        if (FAILED(m_pVIBufferCom->Render()))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CFlash_Effect::Ready_Components()
{
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Flash"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    return S_OK;
}

void CFlash_Effect::AlphaDown(_float fTimeDelta)
{
    if (m_bAlphaDown)
    {
        if (m_fColor.w > 0)
            m_fColor.w -= fTimeDelta * m_fSpeed;
    }
}

void CFlash_Effect::Lerp_Size(_float fTimeDelta)
{
    _float3 fCurSize = m_pTransformCom->Get_Scaled();

    _float fAmout = fTimeDelta * 4.f;

    if (m_bSizedown == false)
    {
        _float fSize = min(0.8f, fCurSize.x + fAmout);
        m_pTransformCom->Set_Scaled(fCurSize.x + fAmout, fCurSize.y + fAmout, fCurSize.z + fAmout);
        if (fSize == 0.8f)
            m_bSizedown = true;
    }

    if (m_bSizedown)
    {
        _float fSize = max(0.1f, fCurSize.x - fAmout);
        m_pTransformCom->Set_Scaled(fSize, fSize, fSize);
        if (fSize == 0.1f)
            m_bSizedown = false;
    }
}

void CFlash_Effect::Change_Color(_float fTimeDelta)
{
    m_fColor.x = (0.f, m_fColor.x- fTimeDelta * 0.5f);
    m_fColor.y = (0.f, m_fColor.y- fTimeDelta * 0.3f);
    m_fColor.z = (0.f, m_fColor.z- fTimeDelta * 0.1f);
}

CFlash_Effect* CFlash_Effect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CFlash_Effect* pInstance = new CFlash_Effect(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CFlash_Effect"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CFlash_Effect::Clone(void* pArg)
{
    CFlash_Effect* pInstance = new CFlash_Effect(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CFlash_Effect"));
        Safe_Release(pInstance);
    }

    return pInstance;
}


void CFlash_Effect::Free()
{
    __super::Free();
}