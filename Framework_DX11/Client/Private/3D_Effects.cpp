#include "stdafx.h"
#include "3D_Effects.h"
#include "GameInstance.h"
#include "Client_Defines.h"

C3D_Effects::C3D_Effects(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CGameObject(pDevice, pContext)
{
}

C3D_Effects::C3D_Effects(const C3D_Effects& Prototype)
    :CGameObject(Prototype)
{
}
HRESULT C3D_Effects::Initialize_Prototype()
{
    return S_OK;
}

HRESULT C3D_Effects::Initialize(void* pArg)
{
    GAMEOBJECT_DESC* pDesc = static_cast<GAMEOBJECT_DESC*>(pArg);

    /* 직교퉁여을 위한 데이터들을 모두 셋. */
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;


    //Read한 정보 세팅
  //  m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&pDesc->vPosition));
    m_pTransformCom->Set_Scaled(pDesc->vScale.x, pDesc->vScale.y, pDesc->vScale.z);
    m_pTransformCom->RotationThreeAxis(pDesc->vRotation);
    m_vRot = pDesc->vRotation;
    m_iRoomNum = pDesc->iRoomNum;


    MODEL_EFFECT_DESC* pModelDesc = static_cast<MODEL_EFFECT_DESC*>(pArg);
    if (pModelDesc != nullptr)
    {
        m_iEffectType = pModelDesc->iEffectType;
        m_pParentMatrix = pModelDesc->pParentWorldMatrix;
        m_iLevelIndex = pModelDesc->iLevelIndex;
    }

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_isActive = false;
    return S_OK;
}

void C3D_Effects::Priority_Update(_float fTimeDelta)
{
    if (m_isActive)
    {
        __super::Priority_Update(fTimeDelta);
    }
}

void C3D_Effects::Update(_float fTimeDelta)
{
    if (m_isActive)
    {
        __super::Update(fTimeDelta);

        if (m_iEffectType == MONSTER_HIT_EFFECT)
        {
            m_fAlpha = 0.6f;
            m_fBright = 1.5f;
            Monster_HIt_SizeUp(fTimeDelta);
        }
    }
    else
    {
        m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(m_pParentMatrix));
        _vector vOffeset = { 0.f, 1.f,0.f };
        m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_State(CTransform::STATE_POSITION) + vOffeset);
    }
}

void C3D_Effects::Late_Update(_float fTimeDelta)
{
    if (m_isActive)
    {
        __super::Late_Update(fTimeDelta);
        m_pGameInstance->Add_RenderObject(CRenderer::RG_NONLIGHT, this);
    }
  
}

HRESULT C3D_Effects::Render()
{
    if(m_isActive)
    {
        if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
            return E_FAIL;
        if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
            return E_FAIL;
        if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float))))
            return E_FAIL;   
        
        if (FAILED(m_pShaderCom->Bind_RawValue("g_fBright", &m_fBright, sizeof(_float))))
            return E_FAIL;

        _uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

        for (size_t i = 0; i < iNumMeshes; i++)
        {
            if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", TEXTURE_TYPE::DIFFUSE,(_uint) i)))
                return E_FAIL;

            if (FAILED(m_pShaderCom->Begin(6)))
                return E_FAIL;

            if (FAILED(m_pModelCom->Render((_uint)i)))
                return E_FAIL;
        }

        _float fOne = 1.f;
        if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &fOne, sizeof(_float))))
            return E_FAIL;
        
        if (FAILED(m_pShaderCom->Bind_RawValue("g_fBright", &fOne, sizeof(_float))))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT C3D_Effects::Ready_Components()
{
    /* FOR.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxModel"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    if(m_iEffectType == MONSTER_HIT_EFFECT)
    {
        /* FOR.Com_Model */
        if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_hitflash_00"),
            TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
            return E_FAIL;
    }

    return S_OK;
}

void C3D_Effects::Monster_HIt_SizeUp(_float fTimeDelta)
{
    if (m_fSize == 0.8f)
        m_isActive = false;

    m_pTransformCom->Set_Scaled(m_fSize, m_fSize, m_fSize);

    m_fSize = min(0.8f, m_fSize += fTimeDelta * 1.f);

}

C3D_Effects* C3D_Effects::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    C3D_Effects* pInstance = new C3D_Effects(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : C3D_Effects"));
        Safe_Release(pInstance);
    }

    return pInstance;
}
CGameObject* C3D_Effects::Clone(void* pArg)
{
    C3D_Effects* pInstance = new C3D_Effects(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : C3D_Effects"));
        Safe_Release(pInstance);
    }

    return pInstance;
}
void C3D_Effects::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModelCom);
}
