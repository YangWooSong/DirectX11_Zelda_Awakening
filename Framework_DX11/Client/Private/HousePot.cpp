#include "stdafx.h"
#include "HousePot.h"
#include "GameInstance.h"
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

    return S_OK;
}

void CHousePot::Priority_Update(_float fTimeDelta)
{
}

void CHousePot::Update(_float fTimeDelta)
{
    m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_Ptr());
}

void CHousePot::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);

    m_pGameInstance->Add_ColliderList(m_pColliderCom);
    m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
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
            if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", TEXTURE_TYPE::DIFFUSE, i)))
                return E_FAIL;

            if (FAILED(m_pShaderCom->Begin(0)))
                return E_FAIL;

            if (FAILED(m_pModelCom->Render(i)))
                return E_FAIL;
        }

#ifdef _DEBUG
        m_pColliderCom->Render();
#endif
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
       
            //Change_State(HIT);
        }
    }
}

void CHousePot::OnCollisionExit(CGameObject* pOther)
{
}


HRESULT CHousePot::Ready_Components()
{
    /* FOR.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxModel"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    if(m_iRoomNum == 0)
    {
        /* FOR.Com_Model */
        if (FAILED(__super::Add_Component(LEVEL_MARINHOUSE, TEXT("Prototype_Component_Model_Obj_HousePot"),
            TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
            return E_FAIL;
    }
    else
    {
        /* FOR.Com_Model */
        if (FAILED(__super::Add_Component(LEVEL_DUNGEON, TEXT("Prototype_Component_Model_Obj_HousePot"),
            TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
            return E_FAIL;
    }

    /* For.Com_Collider */
    CBounding_AABB::BOUNDING_AABB_DESC			ColliderDesc{};
    ColliderDesc.vExtents = _float3(0.6f, 0.6f, 0.6f);
    ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vExtents.y, 0.f);

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
        TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
        return E_FAIL;
    m_pColliderCom->Set_Owner(this);

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

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModelCom);
    Safe_Release(m_pColliderCom);
}

