#include "stdafx.h"
#include "LockDoor.h"
#include "GameInstance.h"
#include "Link.h"

CLockDoor::CLockDoor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CGameObject(pDevice, pContext)
{
}

CLockDoor::CLockDoor(const CLockDoor& Prototype)
    :CGameObject(Prototype)
{
}
HRESULT CLockDoor::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CLockDoor::Initialize(void* pArg)
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

    m_isActive = false;

    m_iCurrentAnimIndex = m_pModelCom->Get_AnimationIndex("open");
    m_pModelCom->SetUp_Animation(m_iCurrentAnimIndex, false);
    m_pModelCom->Set_AnimationSpeed(m_iCurrentAnimIndex, 40);

    m_pGameInstance->AddScene_ColMesh(this, TEXT("Lockdoor"));
    return S_OK;
}

void CLockDoor::Priority_Update(_float fTimeDelta)
{
}

void CLockDoor::Update(_float fTimeDelta)
{
    if (m_isActive)
    {
        if(m_bOpend == true || m_fTimer == 0.f)
        {
            m_fTimer += fTimeDelta;
            m_pModelCom->Play_Animation(fTimeDelta);
        }

        m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_Ptr());
    }

    if (m_bOpend)
    {
        m_pGameInstance->Destroy_PhysXActor(this);
        m_pColliderCom->Set_IsActive(false);
    }
}

void CLockDoor::Late_Update(_float fTimeDelta)
{
    if (m_isActive)
    {
        __super::Late_Update(fTimeDelta);
        m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
        m_pGameInstance->Add_ColliderList(m_pColliderCom);

#ifdef _DEBUG
        m_pGameInstance->Add_DebugObject(m_pColliderCom);
#endif
    }
}

HRESULT CLockDoor::Render()
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
            m_pModelCom->Bind_MeshBoneMatrices(m_pShaderCom, "g_BoneMatrices", (_uint)i);

            if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", TEXTURE_TYPE::DIFFUSE, (_uint)i)))
                return E_FAIL;

            if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", TEXTURE_TYPE::NORMALS, (_uint)i)))
                return E_FAIL;

            if (FAILED(m_pShaderCom->Begin(7)))
                return E_FAIL;

            if (FAILED(m_pModelCom->Render((_uint)i)))
                return E_FAIL;
        }
    }

    return S_OK;
}

void CLockDoor::OnCollisionEnter(CGameObject* pOther)
{
}

void CLockDoor::OnCollisionStay(CGameObject* pOther)
{
    if (m_pColliderCom->Get_IsColl())
    {
        if (pOther->Get_LayerTag() == TEXT("Layer_Player"))
        {
            if (static_cast<CLink*>(pOther)->Get_Fsm()->Get_PrevState() == CLink::KEY && static_cast<CLink*>(pOther)->Get_Fsm()->Get_CurrentState() == CLink::IDLE)
            {
                m_pSoundCom->Play_Sound(TEXT("4_Obj_Door_Open.wav"), 1.f);
                m_bOpend = true;
            }
        }
    }
  
}

void CLockDoor::OnCollisionExit(CGameObject* pOther)
{
}

HRESULT CLockDoor::Ready_Components()
{
    /* FOR.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* FOR.Com_Model */
    if (FAILED(__super::Add_Component(LEVEL_DUNGEON, TEXT("Prototype_Component_Model_LockDoor"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    /* For.Com_Collider */
    CBounding_AABB::BOUNDING_AABB_DESC			ColliderDesc{};
    ColliderDesc.vExtents = _float3(1.f, 0.4f, 1.f);
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

CLockDoor* CLockDoor::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CLockDoor* pInstance = new CLockDoor(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CLockDoor"));
        Safe_Release(pInstance);
    }

    return pInstance;
}
CGameObject* CLockDoor::Clone(void* pArg)
{
    CLockDoor* pInstance = new CLockDoor(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CLockDoor"));
        Safe_Release(pInstance);
    }

    return pInstance;
}
void CLockDoor::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pSoundCom);
    Safe_Release(m_pModelCom);
    Safe_Release(m_pColliderCom);
}
