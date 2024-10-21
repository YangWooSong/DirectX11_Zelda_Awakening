#include "stdafx.h"
#include "FootDust.h"
#include "GameInstance.h"
#include "Player.h"
CFootDust::CFootDust(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :C2DEffects(pDevice, pContext)
{
}

CFootDust::CFootDust(const CFootDust& Prototype)
    :C2DEffects(Prototype)
{
}


HRESULT CFootDust::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CFootDust::Initialize(void* pArg)
{
    /* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_isActive = false;

    m_iDepth = 1;

    m_fFrame = m_pGameInstance->Get_Random(0.f, 8.f);
    return S_OK;
}

void CFootDust::Priority_Update(_float fTimeDelta)
{
}

void CFootDust::Update(_float fTimeDelta)
{
    if (m_isActive)
    {
        if (m_iFrameIndex >= 31)
        {
            m_fFrame = 0.f;
        }
        m_fFrame += fTimeDelta * 10.f;


        m_iFrameIndex = (_uint)floor(m_fFrame);

        m_iTexCoordCol = m_iFrameIndex % 8;
        m_iTexCoordRow = m_iFrameIndex / 8;
  
        m_vOffset = { 0.f,0.3f,0.f };

        if(static_cast<CPlayer*>(m_pParentObj)->Get_Player_PrePos(&m_vNewPos))
        {
            m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&m_vNewPos) + XMLoadFloat3(&m_vOffset));
            m_fColor.w = 1.f;
        }
        else
        {   
            m_fColor.w = max(0.f, m_fColor.w - fTimeDelta * 3.f);
        }

        if (static_cast<CPlayer*>(m_pParentObj)->Get_Player_Dir() == CPlayer::RIGHT)
            m_pTransformCom->Go_World_Left(fTimeDelta, 1.5f);
        else  if (static_cast<CPlayer*>(m_pParentObj)->Get_Player_Dir() == CPlayer::LEFT)
            m_pTransformCom->Go_World_Right(fTimeDelta, 1.5f);
        else  if (static_cast<CPlayer*>(m_pParentObj)->Get_Player_Dir() == CPlayer::FRONT)
            m_pTransformCom->Go_World_Backward(fTimeDelta, 1.5f);
        else  if (static_cast<CPlayer*>(m_pParentObj)->Get_Player_Dir() == CPlayer::BACK)
            m_pTransformCom->Go_World_Straight(fTimeDelta, 1.5f);

    }
    else
    {
        if (static_cast<CPlayer*>(m_pParentObj)->Get_Player_PrePos(&m_vNewPos))
        {
            m_isActive = true;
            m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&m_vNewPos) + XMLoadFloat3(&m_vOffset));
            m_fColor.w = 1.f;
        }
    }
}

void CFootDust::Late_Update(_float fTimeDelta)
{
    if (m_isActive)
    {
        __super::Late_Update(fTimeDelta);
    }
}

HRESULT CFootDust::Render()
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

        if (FAILED(m_pShaderCom->Bind_RawValue("g_iCol", &m_iTexCoordCol, sizeof(_int))))
            return E_FAIL;
        if (FAILED(m_pShaderCom->Bind_RawValue("g_iRow", &m_iTexCoordRow, sizeof(_int))))
            return E_FAIL;

        if (FAILED(m_pTextureCom->Bind_ShadeResource(m_pShaderCom, "g_Texture", 0)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(15)))
            return E_FAIL;

        if (FAILED(m_pVIBufferCom->Bind_Buffers()))
            return E_FAIL;

        if (FAILED(m_pVIBufferCom->Render()))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CFootDust::Ready_Components()
{
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_smoke_pattern_02"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    return S_OK;
}

CFootDust* CFootDust::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CFootDust* pInstance = new CFootDust(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CFootDust"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CFootDust::Clone(void* pArg)
{
    CFootDust* pInstance = new CFootDust(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CFootDust"));
        Safe_Release(pInstance);
    }

    return pInstance;
}


void CFootDust::Free()
{
    __super::Free();
}