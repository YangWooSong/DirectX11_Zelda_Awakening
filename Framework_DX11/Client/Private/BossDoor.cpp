#include "stdafx.h"
#include "BossDoor.h"
#include "GameInstance.h"
#include "Link.h"

CBossDoor::CBossDoor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CGameObject(pDevice, pContext)
{
}

CBossDoor::CBossDoor(const CBossDoor& Prototype)
    :CGameObject(Prototype)
{
}
HRESULT CBossDoor::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CBossDoor::Initialize(void* pArg)
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

    m_iRemoveKeyAnimIndex = m_pModelCom->Get_AnimationIndex("remove_key");
    m_iOpenAnimIndex = m_pModelCom->Get_AnimationIndex("open");

    m_iCurrentAnimIndex = m_iRemoveKeyAnimIndex;
    m_pModelCom->SetUp_Animation(m_iCurrentAnimIndex, false);
    m_pModelCom->Set_AnimationSpeed(m_iCurrentAnimIndex, 60);

    m_pGameInstance->AddScene_ColMesh(this, TEXT("BossDoor"));
    return S_OK;
}

void CBossDoor::Priority_Update(_float fTimeDelta)
{
}

void CBossDoor::Update(_float fTimeDelta)
{
    if (m_isActive)
    {
        if(m_fTimer == 0.f)
        {
            m_fTimer += fTimeDelta;
            m_pModelCom->Play_Animation(fTimeDelta);
        }

        if (m_bOpend == true )
        {
            if (m_iCurrentAnimIndex == m_iRemoveKeyAnimIndex && m_pModelCom->Get_IsEnd_CurrentAnimation())
            {
                if(m_bPlayedSound == false)
                {
                    m_bPlayedSound = true;
                    m_pSoundCom->Play_Sound(TEXT("4_Obj_Door_Open.wav"), 1.f);
                }

                m_iCurrentAnimIndex = m_iOpenAnimIndex;
                m_pModelCom->SetUp_NextAnimation(m_iCurrentAnimIndex,0.1f, false);
                m_pModelCom->Set_AnimationSpeed(m_iCurrentAnimIndex, 50);
            }
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

void CBossDoor::Late_Update(_float fTimeDelta)
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

HRESULT CBossDoor::Render()
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

            if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", TEXTURE_TYPE::NORMALS, i)))
                return E_FAIL;

            if (FAILED(m_pShaderCom->Begin(7)))
                return E_FAIL;

            if (FAILED(m_pModelCom->Render((_uint)i)))
                return E_FAIL;
        }
    }

    return S_OK;
}

void CBossDoor::OnCollisionEnter(CGameObject* pOther)
{
}

void CBossDoor::OnCollisionStay(CGameObject* pOther)
{
    if (m_pColliderCom->Get_IsColl())
    {
        if (pOther->Get_LayerTag() == TEXT("Layer_Player"))
        {
            if (static_cast<CLink*>(pOther)->Get_Fsm()->Get_PrevState() == CLink::KEY && static_cast<CLink*>(pOther)->Get_Fsm()->Get_CurrentState() == CLink::IDLE)
            {
                m_bOpend = true;
            }
        }
    }

}

void CBossDoor::OnCollisionExit(CGameObject* pOther)
{
}

HRESULT CBossDoor::Ready_Components()
{
    /* FOR.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* FOR.Com_Model */
    if (FAILED(__super::Add_Component(LEVEL_DUNGEON, TEXT("Prototype_Component_Model_BossDoor"),
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

CBossDoor* CBossDoor::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CBossDoor* pInstance = new CBossDoor(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CBossDoor"));
        Safe_Release(pInstance);
    }

    return pInstance;
}
CGameObject* CBossDoor::Clone(void* pArg)
{
    CBossDoor* pInstance = new CBossDoor(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CBossDoor"));
        Safe_Release(pInstance);
    }

    return pInstance;
}
void CBossDoor::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pSoundCom);
    Safe_Release(m_pModelCom);
    Safe_Release(m_pColliderCom);
}
