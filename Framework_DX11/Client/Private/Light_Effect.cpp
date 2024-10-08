#include "stdafx.h"
#include "Light_Effect.h"
#include "GameInstance.h"

CLight_Effect::CLight_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :C2DEffects(pDevice, pContext)
{
}

CLight_Effect::CLight_Effect(const CLight_Effect& Prototype)
    :C2DEffects(Prototype)
{
}


HRESULT CLight_Effect::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CLight_Effect::Initialize(void* pArg)
{
    /* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_pTransformCom->Set_Scaled(0.1f, 0.1f, 0.1f);
    m_isActive = false;

    return S_OK;
}

void CLight_Effect::Priority_Update(_float fTimeDelta)
{
}

void CLight_Effect::Update(_float fTimeDelta)
{
    if (m_isActive)
    {
       
        if (m_iEffectType == BOMB_FUSE)
        {
            m_fOffset = { 0.f, 0.8f,0.f };
            Lerp_Size(fTimeDelta);
            m_fColor = { 1.f, 1.f,0.278f, 1.f };
            m_pTransformCom->Set_RotationSpeed(50.f);
            m_pTransformCom->Turn(_vector{ 0.f, 1.f, 0.f }, fTimeDelta);
            m_fAngle = min(360.f, m_fAngle + fTimeDelta * 100.f);

            if (m_fAngle == 360.f)
                m_fAngle = 0.f;
        }

        m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pParentObj->Get_Transform()->Get_State(CTransform::STATE_POSITION) + XMLoadFloat3(&m_fOffset));

    }
}

void CLight_Effect::Late_Update(_float fTimeDelta)
{
    if (m_isActive)
    {
        m_pTransformCom->BillBoard_RotZ(m_iLevelIndex, m_fAngle);
        m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);
    }
}

HRESULT CLight_Effect::Render()
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

        if (FAILED(m_pTextureCom->Bind_ShadeResource(m_pShaderCom, "g_Texture", 0)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(8)))
            return E_FAIL;

        if (FAILED(m_pVIBufferCom->Bind_Buffers()))
            return E_FAIL;

        if (FAILED(m_pVIBufferCom->Render()))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CLight_Effect::Ready_Components()
{
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Light"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    return S_OK;
}

void CLight_Effect::Lerp_Size(_float fTimeDelta)
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

CLight_Effect* CLight_Effect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CLight_Effect* pInstance = new CLight_Effect(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CLight_Effect"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CLight_Effect::Clone(void* pArg)
{
    CLight_Effect* pInstance = new CLight_Effect(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CLight_Effect"));
        Safe_Release(pInstance);
    }

    return pInstance;
}


void CLight_Effect::Free()
{
    __super::Free();
}