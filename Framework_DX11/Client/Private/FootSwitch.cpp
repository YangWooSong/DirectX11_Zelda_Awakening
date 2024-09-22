#include "stdafx.h"
#include "FootSwitch.h"
#include "GameInstance.h"

CFootSwitch::CFootSwitch(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CGameObject(pDevice, pContext)
{
}

CFootSwitch::CFootSwitch(const CFootSwitch& Prototype)
    :CGameObject(Prototype)
{
}

HRESULT CFootSwitch::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CFootSwitch::Initialize(void* pArg)
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

    m_iCurrentAnimIndex = m_pModelCom->Get_AnimationIndex("wait");
    m_iOnAnimIndex = m_pModelCom->Get_AnimationIndex("on");

    m_pModelCom->SetUp_Animation(m_iCurrentAnimIndex, true);
    m_pModelCom->Set_AnimationSpeed(m_iCurrentAnimIndex, 40);

    Set_LayerTag(TEXT("Layer_Monster"));
    return S_OK;
}

void CFootSwitch::Priority_Update(_float fTimeDelta)
{
}

void CFootSwitch::Update(_float fTimeDelta)
{
    if (m_isActive)
    {
        m_pModelCom->Play_Animation(fTimeDelta);
        m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_Ptr());
    }

    if (m_bOn)
        m_pColliderCom->Set_IsActive(false);
}

void CFootSwitch::Late_Update(_float fTimeDelta)
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

HRESULT CFootSwitch::Render()
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
        _bool bFalse = false;

        for (size_t i = 0; i < iNumMeshes; i++)
        {
            if(i == 0)
            {
                if (FAILED(m_pShaderCom->Bind_RawValue("g_bChangeColor", &m_bOn, sizeof(_bool))))
                    return E_FAIL;
            }


            m_pModelCom->Bind_MeshBoneMatrices(m_pShaderCom, "g_BoneMatrices", (_uint)i);

            if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", TEXTURE_TYPE::DIFFUSE, (_uint)i)))
                return E_FAIL;


            if (FAILED(m_pShaderCom->Begin(2)))
                return E_FAIL;

            if (FAILED(m_pModelCom->Render((_uint)i)))
                return E_FAIL;


            if (FAILED(m_pShaderCom->Bind_RawValue("g_bChangeColor", &bFalse, sizeof(_bool))))
                return E_FAIL;
        }
   
    }

    return S_OK;
}

void CFootSwitch::OnCollisionEnter(CGameObject* pOther)
{
    if (m_pColliderCom->Get_IsColl())
    {
        if (pOther->Get_LayerTag() == TEXT("Layer_Player") && m_bOn == false)
        {
            m_iCurrentAnimIndex = m_iOnAnimIndex;
            m_pModelCom->SetUp_Animation(m_iCurrentAnimIndex);
            m_pSoundCom->Play_Sound(TEXT("4_Obj_FootSwitch_On.wav"), 0.4f);

            m_bOn = true;
        }
    }
}

void CFootSwitch::OnCollisionStay(CGameObject* pOther)
{
}

void CFootSwitch::OnCollisionExit(CGameObject* pOther)
{
}

HRESULT CFootSwitch::Ready_Components()
{
    /* FOR.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* FOR.Com_Model */
    if (FAILED(__super::Add_Component(LEVEL_DUNGEON, TEXT("Prototype_Component_Model_FootSwitch"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    /* For.Com_Collider */
    CBounding_AABB::BOUNDING_AABB_DESC			ColliderDesc{};
    ColliderDesc.vExtents = _float3(0.65f, 0.4f, 0.65f);
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

CFootSwitch* CFootSwitch::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CFootSwitch* pInstance = new CFootSwitch(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CFootSwitch"));
        Safe_Release(pInstance);
    }

    return pInstance;
}
CGameObject* CFootSwitch::Clone(void* pArg)
{
    CFootSwitch* pInstance = new CFootSwitch(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CFootSwitch"));
        Safe_Release(pInstance);
    }

    return pInstance;
}
void CFootSwitch::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModelCom);
    Safe_Release(m_pColliderCom);
    Safe_Release(m_pSoundCom);
}
