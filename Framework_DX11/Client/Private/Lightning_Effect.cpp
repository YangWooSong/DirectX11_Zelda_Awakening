#include "stdafx.h"
#include "Lightning_Effect.h"
#include "GameInstance.h"

CLightning_Effect::CLightning_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :C2DEffects(pDevice, pContext)
{
}

CLightning_Effect::CLightning_Effect(const CLightning_Effect& Prototype)
    :C2DEffects(Prototype)
{
}


HRESULT CLightning_Effect::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CLightning_Effect::Initialize(void* pArg)
{
    /* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_isActive = false;

    m_iDepth = 1;

    m_vOriSize = m_pTransformCom->Get_Scaled();

    m_fFrame = m_pGameInstance->Get_Random(0.f, 8.f);
    return S_OK;
}

void CLightning_Effect::Priority_Update(_float fTimeDelta)
{
}

void CLightning_Effect::Update(_float fTimeDelta)
{
    if (m_isActive)
    {
       if (m_iFrameIndex >= 8)
        {
            m_fFrame = 0.f;
        }
        m_fFrame += fTimeDelta * 5.f;


        m_iFrameIndex = (_uint)floor(m_fFrame);

        m_iTexCoordCol = m_iFrameIndex % 4;
        m_iTexCoordRow = m_iFrameIndex / 4;

       

      //      m_iFrameIndex = 0;
        // m_bReset = false;
        // Lerp_Size(fTimeDelta);
        m_vOffset = { 0.f,0.5f,0.f };
        m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pParentObj->Get_Transform()->Get_State(CTransform::STATE_POSITION) + XMLoadFloat3(&m_vOffset));
       // m_pTransformCom->Turn_Lerp_Angle(m_pTransformCom->Get_Rot(), _float3(5.f, 0.f, 0.f), 10.f);
    }
    else
    {
        if (m_bReset == false)
        {
            m_bReset = true;
            m_bSizeUp = true;
            m_fColor.w = 1.f;
            m_pTransformCom->Set_Scaled(m_vOriSize.x, m_vOriSize.y, m_vOriSize.z);
        }
    }
}

void CLightning_Effect::Late_Update(_float fTimeDelta)
{
    if (m_isActive)
    {
        __super::Late_Update(fTimeDelta);
        m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);
    }
}

HRESULT CLightning_Effect::Render()
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

        if (FAILED(m_pShaderCom->Begin(11)))
            return E_FAIL;

        if (FAILED(m_pVIBufferCom->Bind_Buffers()))
            return E_FAIL;

        if (FAILED(m_pVIBufferCom->Render()))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CLightning_Effect::Ready_Components()
{
    if (FAILED(__super::Add_Component(LEVEL_DUNGEON, TEXT("Prototype_Component_Texture_Lightning"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    return S_OK;
}

void CLightning_Effect::Lerp_Size(_float fTimeDelta)
{
    _float3 fCurSize = m_pTransformCom->Get_Scaled();

    _float fAmout = fTimeDelta * 1.f;

    if (fAmout + fCurSize.x <= m_fMaxSize && m_bSizeDown == false)
        m_pTransformCom->Set_Scaled(fCurSize.x + fAmout, fCurSize.y + fAmout, fCurSize.z + fAmout);
    else
        m_bSizeDown = true;

    if (m_bSizeDown)
    {
        if (fCurSize.x - fAmout >= m_vOriSize.x)
            m_pTransformCom->Set_Scaled(fCurSize.x - fAmout, fCurSize.y - fAmout, fCurSize.z - fAmout);
        else
        {
            m_isActive = false;
            m_bSizeDown = false;
        }
    }
}

CLightning_Effect* CLightning_Effect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CLightning_Effect* pInstance = new CLightning_Effect(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CLightning_Effect"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CLightning_Effect::Clone(void* pArg)
{
    CLightning_Effect* pInstance = new CLightning_Effect(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CLightning_Effect"));
        Safe_Release(pInstance);
    }

    return pInstance;
}


void CLightning_Effect::Free()
{
    __super::Free();
}