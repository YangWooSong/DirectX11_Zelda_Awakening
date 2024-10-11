#include "stdafx.h"
#include "Ring_Effect.h"
#include "GameInstance.h"
CRing_Effect::CRing_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :C3D_Effects(pDevice, pContext)
{
}
CRing_Effect::CRing_Effect(const CRing_Effect& Prototype)
    :C3D_Effects(Prototype)
{
}

HRESULT CRing_Effect::Initialize_Prototype()
{
    __super::Initialize_Prototype();
    return S_OK;
}

HRESULT CRing_Effect::Initialize(void* pArg)
{
    __super::Initialize(pArg);

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_pModelCom->Add_Texture_to_Material(TEXT("../Bin/Resources/Zelda/Effect/grad_02.dds"), TEXTURE_TYPE::DIFFUSE, 0);

    GAMEOBJECT_DESC* pDesc = static_cast<GAMEOBJECT_DESC*>(pArg);
    m_fOriSize = pDesc->vScale;
  
    if (m_iEffectType == PLAYER_CHARGE_SLASH_EFFECT || BOX_APPEAR)
    {
        m_fTexMove = 0.7f;
    }
    return S_OK;
}

void CRing_Effect::Priority_Update(_float fTimeDelta)
{
    if (m_isActive)
    {

        __super::Priority_Update(fTimeDelta);
    }
}

void CRing_Effect::Update(_float fTimeDelta)
{
    if(m_isActive)
    {
        _vector vOffeset = {};

        if (m_iEffectType == PLAYER_CHARGE_SLASH_EFFECT)
        {
            vOffeset = { 0.f,0.5f,0.f };
            m_fColor = { 1.f,1.f,0.8f,1.f };
            m_fTexMove += fTimeDelta * 1.2f;
            m_bMoveAlpha = true;
        } 
        
        else if (m_iEffectType == BOX_APPEAR)
        {
            vOffeset = { 0.f,0.5f,0.f };
            m_fColor = { 1.f,0.8f,0.8f,1.f };
            m_fTexMove += fTimeDelta * 1.2f;
            m_bMoveAlpha = true;
        }

        m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(m_pParentMatrix));
        m_pTransformCom->RotationThreeAxis({ 90.f,0.f,0.f });
        m_pTransformCom->Set_Scaled(m_fOriSize.x, m_fOriSize.y, m_fOriSize.z);
        m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_State(CTransform::STATE_POSITION) + vOffeset);
    }
    else
    {
        if (m_iEffectType == PLAYER_CHARGE_SLASH_EFFECT || BOX_APPEAR)
        {
            m_fTexMove = 0.7f;
        }
    }
}

void CRing_Effect::Late_Update(_float fTimeDelta)
{
   // m_pTransformCom->BillBoard(m_iLevelIndex);
    __super::Late_Update(fTimeDelta);
}

HRESULT CRing_Effect::Render()
{
    if (m_isActive)
    {
        if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
            return E_FAIL;
        if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
            return E_FAIL;
        if (FAILED(m_pShaderCom->Bind_RawValue("g_MoveTexCoord", &m_fTexMove, sizeof(_float))))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_RawValue("g_fColor", &m_fColor, sizeof(_float3))))
            return E_FAIL;

        _uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

        for (size_t i = 0; i < iNumMeshes; i++)
        {
            if (m_iEffectType == PLAYER_CHARGE_SLASH_EFFECT || BOX_APPEAR)
            {
                if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", TEXTURE_TYPE::DIFFUSE, 0, 0)))
                    return E_FAIL;
                if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_AlphaTexture", TEXTURE_TYPE::DIFFUSE, 0, 1)))
                    return E_FAIL;
            }

            if (FAILED(m_pShaderCom->Begin(8)))
                return E_FAIL;

            if (FAILED(m_pModelCom->Render((_uint)i)))
                return E_FAIL;
        }

        _float fZero = 0.f;
        if (FAILED(m_pShaderCom->Bind_RawValue("g_MoveTexCoord", &fZero, sizeof(_float))))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CRing_Effect::Ready_Components()
{
    /* FOR.Com_Model */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_ring_00"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;
    return S_OK;
}

C3D_Effects* CRing_Effect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CRing_Effect* pInstance = new CRing_Effect(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CRing_Effect"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CRing_Effect::Clone(void* pArg)
{
    CRing_Effect* pInstance = new CRing_Effect(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CRing_Effect"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CRing_Effect::Free()
{
    __super::Free();

}

