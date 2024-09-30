#include "stdafx.h"
#include "PurpleQuartz.h"
#include "GameInstance.h"
#include "Particle_Model.h"
CPurpleQuartz::CPurpleQuartz(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CGameObject(pDevice, pContext)
{
}

CPurpleQuartz::CPurpleQuartz(const CPurpleQuartz& Prototype)
    :CGameObject(Prototype)
{
}
HRESULT CPurpleQuartz::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CPurpleQuartz::Initialize(void* pArg)
{
    GAMEOBJECT_DESC* pDesc = static_cast<GAMEOBJECT_DESC*>(pArg);

    /* 직교퉁여을 위한 데이터들을 모두 셋. */
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    //Read한 정보 세팅
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&pDesc->vPosition));
    m_pTransformCom->Set_Scaled(pDesc->vScale.x, pDesc->vScale.y, pDesc->vScale.z);
    m_pTransformCom->RotationThreeAxis(pDesc->vRotation);
    m_vRot = pDesc->vRotation;
    m_iRoomNum = pDesc->iRoomNum;

    m_pGameInstance->AddScene_ColMesh(this, TEXT("PurpleQuartz"));

    m_isActive = false;


    CParticle_Model::MODEL_PARTICLE_DESC Desc = {};
    Desc.iParticleType = CParticle_Model::PURPLEQUARTZ;

    m_pParticle = m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Particle_Model"), &Desc);

    return S_OK;
}

void CPurpleQuartz::Priority_Update(_float fTimeDelta)
{
}

void CPurpleQuartz::Update(_float fTimeDelta)
{
    if (m_bBreak)
    {
        m_pGameInstance->Destroy_PhysXActor(this);
        m_pColliderCom->Set_IsActive(false);
        m_pParticle->Update(fTimeDelta);
    }

    if (m_isActive)
    {
        m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_Ptr());
        m_pParticle->Get_Transform()->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
    }
}

void CPurpleQuartz::Late_Update(_float fTimeDelta)
{
    if (m_isActive)
    {
        m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
        m_pGameInstance->Add_ColliderList(m_pColliderCom);

#ifdef _DEBUG
        m_pGameInstance->Add_DebugObject(m_pColliderCom);
#endif
    }

    if (m_bBreak)
        m_pParticle->Late_Update(fTimeDelta);
}

HRESULT CPurpleQuartz::Render()
{
    if (m_isActive)
    {
        if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
            return E_FAIL;
        if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
            return E_FAIL;


        _uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

        for (size_t i = 0; i < iNumMeshes; i++)
        {
            if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", TEXTURE_TYPE::DIFFUSE, i)))
                return E_FAIL;

            if (FAILED(m_pShaderCom->Begin(0)))
                return E_FAIL;

            if (FAILED(m_pModelCom->Render(i)))
                return E_FAIL;
        }
    }

    if (m_bBreak)
        m_pParticle->Render();

    return S_OK;
}

void CPurpleQuartz::OnCollisionEnter(CGameObject* pOther)
{
    if (m_pColliderCom->Get_IsColl())
    {
        if (pOther->Get_LayerTag() == TEXT("Layer_Sword"))
        {
            m_bBreak = true;
            m_isActive = false;
            m_pSoundCom->Play_Sound(TEXT("4_Obj_HousePot_Break.wav"), 1.f);
        }
    }
}

void CPurpleQuartz::OnCollisionStay(CGameObject* pOther)
{
}

void CPurpleQuartz::OnCollisionExit(CGameObject* pOther)
{
}

HRESULT CPurpleQuartz::Ready_Components()
{
    /* FOR.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxModel"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* FOR.Com_Model */
    if (FAILED(__super::Add_Component(LEVEL_DUNGEON, TEXT("Prototype_Component_Model_PurpleQuartz"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    /* For.Com_Collider */
    CBounding_AABB::BOUNDING_AABB_DESC			ColliderDesc{};
    ColliderDesc.vExtents = _float3(0.6f, 1.f, 0.6f);
    ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vExtents.y, 0.f);

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
        TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
        return E_FAIL;
    m_pColliderCom->Set_Owner(this);

    /* FOR.Com_Sound */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sound"),
        TEXT("Com_Sound"), reinterpret_cast<CComponent**>(&m_pSoundCom))))
        return E_FAIL;
    m_pSoundCom->Set_Owner(this);

    return S_OK;
}

CPurpleQuartz* CPurpleQuartz::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CPurpleQuartz* pInstance = new CPurpleQuartz(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CPurpleQuartz"));
        Safe_Release(pInstance);
    }

    return pInstance;
}
CGameObject* CPurpleQuartz::Clone(void* pArg)
{
    CPurpleQuartz* pInstance = new CPurpleQuartz(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CPurpleQuartz"));
        Safe_Release(pInstance);
    }

    return pInstance;
}
void CPurpleQuartz::Free()
{
    __super::Free();


    Safe_Release(m_pParticle);

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModelCom);
    Safe_Release(m_pColliderCom);
    Safe_Release(m_pSoundCom);
}
