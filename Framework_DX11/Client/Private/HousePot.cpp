#include "stdafx.h"
#include "HousePot.h"
#include "GameInstance.h"
#include "Link.h"
#include"Particle_Model.h"

CHousePot::CHousePot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CGameObject(pDevice, pContext)
{
}

CHousePot::CHousePot(const CHousePot& Prototype)
    :CGameObject(Prototype)
{
}

HRESULT CHousePot::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CHousePot::Initialize(void* pArg)
{
    GAMEOBJECT_DESC* pDesc = static_cast<GAMEOBJECT_DESC*>(pArg);

    /* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&pDesc->vPosition));
    m_pTransformCom->Set_Scaled(pDesc->vScale.x, pDesc->vScale.y, pDesc->vScale.z);
    m_pTransformCom->RotationThreeAxis(pDesc->vRotation);
    m_vRot = pDesc->vRotation;

    m_pGameInstance->AddScene_ColMesh(this, TEXT("HousePot"));

    Set_LayerTag(TEXT("Layer_HousePot"));

    if (m_iRoomNum != 0)
        m_isActive = false;
    else
        m_isActive = true;

    CParticle_Model::MODEL_PARTICLE_DESC Desc = {};
    Desc.iParticleType = CParticle_Model::HOUSEPOT;

    m_pParticle = m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Particle_Model"), &Desc);

    return S_OK;
}

void CHousePot::Priority_Update(_float fTimeDelta)
{
}

void CHousePot::Update(_float fTimeDelta)
{
    if (m_bBreak)
    {
        m_bCarried = false;
        m_isActive = false;
        m_pParticle->Update(fTimeDelta);
        m_pColliderCom->Set_IsActive(false);
    }

    if(m_isActive)
    {
     
        if (m_bBreak)
        {
            m_bCarried = false;
            m_isActive = false;
            m_pParticle->Update(fTimeDelta);
        }

        if (m_bCarried)
        {
            m_pGameInstance->Destroy_PhysXActor(this);

            if (m_fTimer > 0.5f || m_pPlayerFsmCom->Get_CurrentState() != CLink::CARRY)
            {
                _matrix		SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);
                m_pTransformCom->Set_WorldMatrix(SocketMatrix * m_pPlayer->Get_Transform()->Get_WorldMatrix());
            }
            m_fTimer += fTimeDelta;
        }           

        m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_Ptr());
        m_pParticle->Get_Transform()->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
    }

}

void CHousePot::Late_Update(_float fTimeDelta)
{
    if(m_isActive)
    {
        __super::Late_Update(fTimeDelta);

        m_pGameInstance->Add_ColliderList(m_pColliderCom);
        m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
        m_pGameInstance->Add_RenderObject(CRenderer::RG_SHADOWOBJ, this);
#ifdef _DEBUG
        m_pGameInstance->Add_DebugObject(m_pColliderCom);
#endif

    }

    if (m_bBreak)
        m_pParticle->Late_Update(fTimeDelta);
}

HRESULT CHousePot::Render()
{
    if(m_isActive)
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
            if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", TEXTURE_TYPE::DIFFUSE, (_uint)i)))
                return E_FAIL;

            if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", TEXTURE_TYPE::NORMALS, (_uint)i)))
                return E_FAIL;

            if (FAILED(m_pShaderCom->Begin(5)))
                return E_FAIL;

            if (FAILED(m_pModelCom->Render((_uint)i)))
                return E_FAIL;
        }
    }

    if (m_bBreak)
        m_pParticle->Render();

    return S_OK;
}

HRESULT CHousePot::Render_LightDepth()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldMatrix_Ptr())))
        return E_FAIL;


    _float4x4		ViewMatrix;
    XMStoreFloat4x4(&ViewMatrix, XMMatrixLookAtLH(m_pGameInstance->Get_ShadowLightPos_Vector(), m_pGameInstance->Get_LightLook_Vector(), XMVectorSet(0.f, 1.f, 0.f, 0.f)));

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

    _uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", TEXTURE_TYPE::DIFFUSE, (_uint)i)))
            return E_FAIL;

        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", TEXTURE_TYPE::NORMALS, (_uint)i)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(13)))
            return E_FAIL;

        if (FAILED(m_pModelCom->Render((_uint)i)))
            return E_FAIL;
    }

    return S_OK;
}

void CHousePot::OnCollisionEnter(CGameObject* pOther)
{
 
}

void CHousePot::OnCollisionStay(CGameObject* pOther)
{
    if (m_pColliderCom->Get_IsColl())
    {
        if (pOther->Get_LayerTag() == TEXT("Layer_Player"))
        {
            if (KEY_TAP(E))
                m_bCarried = true;
        }
    }
}

void CHousePot::OnCollisionExit(CGameObject* pOther)
{
}

void CHousePot::Set_Break(_bool bBreak)
{
    if (bBreak)
    {
        if(m_bBreak == false)
            m_pSoundCom->Play_Sound(TEXT("4_Obj_HousePot_Break.wav"), 1.f);
        m_bBreak = bBreak;
    }
}


HRESULT CHousePot::Ready_Components()
{
    /* FOR.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxModel"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;


    /* FOR.Com_Model */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Obj_HousePot"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;


    /* For.Com_Collider */
    CBounding_AABB::BOUNDING_AABB_DESC			ColliderDesc{};
    ColliderDesc.vExtents = _float3(0.6f, 0.6f, 0.6f);
    ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vExtents.y, 0.f);

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
        TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
        return E_FAIL;
    m_pColliderCom->Set_Owner(this);

    if (m_pGameInstance->Find_Player(LEVEL_MARINHOUSE) != nullptr)
    {
        m_pPlayer = m_pGameInstance->Find_Player(LEVEL_MARINHOUSE);
    }
    else
    {
        m_pPlayer = m_pGameInstance->Find_Player(LEVEL_DUNGEON);
    }

    if(m_pPlayer != nullptr)
    {
       
        m_pPlayerFsmCom = static_cast<CLink*>(m_pPlayer)->Get_Fsm();
        m_pPlayerModelCom = static_cast<CLink*>(m_pPlayer)->Get_Model();
        m_pSocketMatrix = m_pPlayerModelCom->Get_BoneCombindTransformationMatrix_Ptr("itemA_L");
        Safe_AddRef(m_pPlayerModelCom);
        Safe_AddRef(m_pPlayerFsmCom);
    }

    /* FOR.Com_Sound */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sound"),
        TEXT("Com_Sound"), reinterpret_cast<CComponent**>(&m_pSoundCom))))
        return E_FAIL;
    m_pSoundCom->Set_Owner(this);

    return S_OK;
}

CHousePot* CHousePot::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CHousePot* pInstance = new CHousePot(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CHousePot"));
        Safe_Release(pInstance);
    }

    return pInstance;
}


CGameObject* CHousePot::Clone(void* pArg)
{
    CHousePot* pInstance = new CHousePot(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CHousePot"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CHousePot::Free()
{
    __super::Free();

    Safe_Release(m_pParticle);

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModelCom);
    Safe_Release(m_pColliderCom);
    Safe_Release(m_pPlayerModelCom);
    Safe_Release(m_pPlayerFsmCom);
    Safe_Release(m_pSoundCom);
}

