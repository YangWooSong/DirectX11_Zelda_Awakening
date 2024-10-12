#include "stdafx.h"
#include "Fire_Plane_Effect.h"
#include "GameInstance.h"
CFire_Plane_Effect::CFire_Plane_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :C3D_Effects(pDevice, pContext)
{
}
CFire_Plane_Effect::CFire_Plane_Effect(const CFire_Plane_Effect& Prototype)
    :C3D_Effects(Prototype)
{
}

HRESULT CFire_Plane_Effect::Initialize_Prototype()
{
    __super::Initialize_Prototype();
    return S_OK;
}

HRESULT CFire_Plane_Effect::Initialize(void* pArg)
{
    __super::Initialize(pArg);

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_pModelCom->Add_Texture_to_Material(TEXT("../Bin/Resources/Zelda/Effect/fire_forme_01.png"), TEXTURE_TYPE::DIFFUSE, 0);
    m_pModelCom->Add_Texture_to_Material(TEXT("../Bin/Resources/Zelda/Effect/wave_02.png"), TEXTURE_TYPE::DIFFUSE, 0);

    FIRE_EFFECT_DESC* pDesc = static_cast<FIRE_EFFECT_DESC*>(pArg);
    m_fOriSize = pDesc->vScale;
    m_iFireTypeNum = pDesc->iFireTypeNum;
    return S_OK;
}

void CFire_Plane_Effect::Priority_Update(_float fTimeDelta)
{
    if (m_isActive)
    {

        __super::Priority_Update(fTimeDelta);
    }
}

void CFire_Plane_Effect::Update(_float fTimeDelta)
{
    _vector vOffeset = {};

    m_fTexMove += fTimeDelta * 0.8f;
    m_bMoveAlpha = false;
    if(m_iFireTypeNum == 0)
        vOffeset = { 0.f, 0.7f,0.f };
    else
        vOffeset = { 0.f, 0.2f,0.f };
   
    if (m_fTexMove >= 0.1f)
        m_fTexMove = 0.f;

    m_fColor = { 1.f, 0.7f, 0.f, 1.f };

    m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(m_pParentMatrix));
    m_pTransformCom->Set_Scaled(m_fOriSize.x, m_fOriSize.y, m_fOriSize.z);
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_State(CTransform::STATE_POSITION) + vOffeset);
    m_pTransformCom->Turn_Lerp_Angle(m_pTransformCom->Get_Rot(), _float3(10.f, 0.f, 0.f), 10.f);
}

void CFire_Plane_Effect::Late_Update(_float fTimeDelta)
{

   // m_pTransformCom->BillBoard(m_iLevelIndex);
    __super::Late_Update(fTimeDelta);
}

HRESULT CFire_Plane_Effect::Render()
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
        if (FAILED(m_pShaderCom->Bind_RawValue("g_fColor", &m_fColor, sizeof(_float4))))
            return E_FAIL;  

        _uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

        for (size_t i = 0; i < iNumMeshes; i++)
        {

            if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", TEXTURE_TYPE::DIFFUSE, 0, 1)))
                return E_FAIL;
            if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_AlphaTexture", TEXTURE_TYPE::DIFFUSE, 0, 2)))
                return E_FAIL;

            if (FAILED(m_pShaderCom->Begin(11)))
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

HRESULT CFire_Plane_Effect::Ready_Components()
{
    /* FOR.Com_Model */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_fireplane_00"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    return S_OK;
}

C3D_Effects* CFire_Plane_Effect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CFire_Plane_Effect* pInstance = new CFire_Plane_Effect(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CFire_Plane_Effect"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CFire_Plane_Effect::Clone(void* pArg)
{
    CFire_Plane_Effect* pInstance = new CFire_Plane_Effect(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CFire_Plane_Effect"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CFire_Plane_Effect::Free()
{
    __super::Free();
}

