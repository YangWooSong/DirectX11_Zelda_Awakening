#include "stdafx.h"
#include "Lupee.h"
#include "GameInstance.h"

CLupee::CLupee(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CGameObject(pDevice, pContext)
{
}

CLupee::CLupee(const CLupee& Prototype)
    :CGameObject(Prototype)
{
}

HRESULT CLupee::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CLupee::Initialize(void* pArg)
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
    m_pTransformCom->RotationThreeAxis(_float3(0.f, 0.f, 0.f));
    m_vRot = pDesc->vRotation;

    // m_pGameInstance->AddScene_ColMesh(this, TEXT("Land"));
    m_isActive = true;

    return S_OK;
}

void CLupee::Priority_Update(_float fTimeDelta)
{
}

void CLupee::Update(_float fTimeDelta)
{
    if (m_isActive)
    {
        m_pSoundCom->Update(fTimeDelta);
        m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_Ptr());

        if (m_bShow)
            Show_Move(fTimeDelta);
    }
}

void CLupee::Late_Update(_float fTimeDelta)
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

HRESULT CLupee::Render()
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

    return S_OK;
}

void CLupee::OnCollisionEnter(CGameObject* pOther)
{
    if (m_pColliderCom->Get_IsColl())
    {
        if (pOther->Get_LayerTag() == TEXT("Layer_Player"))
        {
            m_isActive = false;
            m_pSoundCom->Play_Sound(TEXT("4_Obj_Get_Lupee.mp3"), 1.f);
        }
    }
}

void CLupee::OnCollisionStay(CGameObject* pOther)
{
}

void CLupee::OnCollisionExit(CGameObject* pOther)
{
}

HRESULT CLupee::Ready_Components()
{
    /* FOR.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxModel"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* FOR.Com_Model */
    if (FAILED(__super::Add_Component(LEVEL_FIELD, TEXT("Prototype_Component_Model_Lupee"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    /* FOR.Com_Sound */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sound"),
        TEXT("Com_Sound"), reinterpret_cast<CComponent**>(&m_pSoundCom))))
        return E_FAIL;
    m_pSoundCom->Set_Owner(this);

    /* For.Com_Collider */
    CBounding_AABB::BOUNDING_AABB_DESC			ColliderDesc{};
    ColliderDesc.vExtents = _float3(0.3f, 0.5f, 0.3f);
    ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vExtents.y, 0.f);

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
        TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
        return E_FAIL;
    m_pColliderCom->Set_Owner(this);

    return S_OK;
}

void CLupee::Show_Move(_float fTimeDelta)
{
    m_fTimer += fTimeDelta;

    if (m_fTimer <  0.4f)
        m_pTransformCom->Go_World_Up(fTimeDelta, 1.8f);
    else  if (m_fTimer < 0.8f)
        m_pTransformCom->Go_World_Up(fTimeDelta, -1.9f);
    else
        m_bShow = false;
}

CLupee* CLupee::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CLupee* pInstance = new CLupee(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CLupee"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CLupee::Clone(void* pArg)
{
    CLupee* pInstance = new CLupee(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CLupee"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLupee::Free()
{
    __super::Free();

    Safe_Release(m_pSoundCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModelCom);
    Safe_Release(m_pColliderCom);
}
