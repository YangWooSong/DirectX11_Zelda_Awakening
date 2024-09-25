#include "stdafx.h"
#include "CrackedWall.h"
#include "GameInstance.h"

CCrackedWall::CCrackedWall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CGameObject(pDevice, pContext)
{
}

CCrackedWall::CCrackedWall(const CCrackedWall& Prototype)
    :CGameObject(Prototype)
{
}
HRESULT CCrackedWall::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CCrackedWall::Initialize(void* pArg)
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

    m_pGameInstance->AddScene_ColMesh(this, TEXT("CrackedWall"));

    m_isActive = false;
    return S_OK;
}

void CCrackedWall::Priority_Update(_float fTimeDelta)
{
}

void CCrackedWall::Update(_float fTimeDelta)
{
    if (m_isActive && m_bCracked == false)
    {
        m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_Ptr());
    }

    if(m_bCracked)
    {
        m_pGameInstance->Destroy_PhysXActor(this);
        m_pColliderCom->Set_IsActive(false);
    }
}

void CCrackedWall::Late_Update(_float fTimeDelta)
{
    if (m_isActive && m_bCracked == false)
    {
        __super::Late_Update(fTimeDelta);
        m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
        m_pGameInstance->Add_ColliderList(m_pColliderCom);

#ifdef _DEBUG
        m_pGameInstance->Add_DebugObject(m_pColliderCom);
#endif
    }
}

HRESULT CCrackedWall::Render()
{
    if (m_isActive && m_bCracked == false)
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
    return S_OK;
}

void CCrackedWall::OnCollisionEnter(CGameObject* pOther)
{
}

void CCrackedWall::OnCollisionStay(CGameObject* pOther)
{
}

void CCrackedWall::OnCollisionExit(CGameObject* pOther)
{
    if (pOther->Get_LayerTag() == TEXT("Layer_Bomb"))
    {
        m_isActive = false;
        m_bCracked = true;
    }
}

HRESULT CCrackedWall::Ready_Components()
{
    /* FOR.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxModel"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* FOR.Com_Model */
    if (FAILED(__super::Add_Component(LEVEL_DUNGEON, TEXT("Prototype_Component_Model_DungeonCrackedWallLv01"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    /* For.Com_Collider */
    CBounding_AABB::BOUNDING_AABB_DESC			ColliderDesc{};
    ColliderDesc.vExtents = _float3(1.f, 1.f, 1.f);
    ColliderDesc.vCenter = _float3(0.5f, ColliderDesc.vExtents.y, 0.f);

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
        TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
        return E_FAIL;
    m_pColliderCom->Set_Owner(this);

    return S_OK;
}

CCrackedWall* CCrackedWall::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CCrackedWall* pInstance = new CCrackedWall(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CCrackedWall"));
        Safe_Release(pInstance);
    }

    return pInstance;
}
CGameObject* CCrackedWall::Clone(void* pArg)
{
    CCrackedWall* pInstance = new CCrackedWall(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CCrackedWall"));
        Safe_Release(pInstance);
    }

    return pInstance;
}
void CCrackedWall::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModelCom);
    Safe_Release(m_pColliderCom);
}
