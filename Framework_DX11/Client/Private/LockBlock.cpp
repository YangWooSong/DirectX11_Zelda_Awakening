#include "stdafx.h"
#include "LockBlock.h"
#include "GameInstance.h"
#include "Link.h"
#include "2DEffects.h"
CLockBlock::CLockBlock(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CGameObject(pDevice, pContext)
{
}

CLockBlock::CLockBlock(const CLockBlock& Prototype)
    :CGameObject(Prototype)
{
}
HRESULT CLockBlock::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CLockBlock::Initialize(void* pArg)
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

    m_pGameInstance->AddScene_ColMesh(this, TEXT("LockBlock"));

    m_isActive = false;
    return S_OK;
}

void CLockBlock::Priority_Update(_float fTimeDelta)
{
    m_pEffect->Priority_Update(fTimeDelta);
}

void CLockBlock::Update(_float fTimeDelta)
{
    if (m_isActive && m_bOpened == false)
    {
        m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_Ptr());

    }

    if (m_bOpened)
    {
        m_pColliderCom->Set_IsActive(false);
        m_pGameInstance->Destroy_PhysXActor(this);
        if(m_bActiveEffect==false)
        {
            m_bActiveEffect = true;
            m_pEffect->SetActive(true);
        }
        m_fAlpha = max(0.f, m_fAlpha - fTimeDelta * 2.f);
    }
    m_pEffect->Update(fTimeDelta);
}

void CLockBlock::Late_Update(_float fTimeDelta)
{
    if (m_isActive)
    {
        m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
        m_pGameInstance->Add_ColliderList(m_pColliderCom);

        if (m_bOpened == false)
        {
            m_pGameInstance->Add_RenderObject(CRenderer::RG_SHADOWOBJ, this);
        }

#ifdef _DEBUG
        m_pGameInstance->Add_DebugObject(m_pColliderCom);
#endif
    }

    m_pEffect->Late_Update(fTimeDelta);
}

HRESULT CLockBlock::Render()
{
    if (m_isActive && m_fAlpha != 0.f)
    {
        if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
            return E_FAIL;
        if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float))))
            return E_FAIL;

        _uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

        for (size_t i = 0; i < iNumMeshes; i++)
        {
            if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", TEXTURE_TYPE::DIFFUSE, (_uint)i)))
                return E_FAIL;

            if (FAILED(m_pShaderCom->Begin(6)))
                return E_FAIL;

            if (FAILED(m_pModelCom->Render((_uint)i)))
                return E_FAIL;
        }

        _float fOne = 1.f;
        if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &fOne, sizeof(_float))))
            return E_FAIL;
    }
    return S_OK;
}

HRESULT CLockBlock::Render_LightDepth()
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

void CLockBlock::OnCollisionEnter(CGameObject* pOther)
{
  
}

void CLockBlock::OnCollisionStay(CGameObject* pOther)
{
    if (m_pColliderCom->Get_IsColl())
    {
        if (pOther->Get_LayerTag() == TEXT("Layer_Player"))
        {
            if(static_cast<CLink*>(pOther)->Get_Fsm()->Get_PrevState() == CLink::KEY && static_cast<CLink*>(pOther)->Get_Fsm()->Get_CurrentState() == CLink::IDLE)
                m_bOpened = true;
        }
    }
}

void CLockBlock::OnCollisionExit(CGameObject* pOther)
{
}

HRESULT CLockBlock::Ready_Components()
{
    /* FOR.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxModel"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* FOR.Com_Model */
    if (FAILED(__super::Add_Component(LEVEL_DUNGEON, TEXT("Prototype_Component_Model_LockBlock"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    /* For.Com_Collider */
    CBounding_AABB::BOUNDING_AABB_DESC			ColliderDesc{};
    ColliderDesc.vExtents = _float3(1.f, 1.f, 1.f);
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

    CGameObject* pGameObj = m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_LockBlockEffect"));

    if (pGameObj != nullptr)
    {
        C2DEffects::EFFECT_DESC Desc{};
        Desc.iLevelIndex = LEVEL_DUNGEON;
        Desc.pParent = this;
        Desc.iEffectType = LOCKBLOCK_OPEN;
        m_pEffect = dynamic_cast<CGameObject*>(pGameObj->Clone(&Desc));
    }

    return S_OK;
}

CLockBlock* CLockBlock::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CLockBlock* pInstance = new CLockBlock(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CLockBlock"));
        Safe_Release(pInstance);
    }

    return pInstance;
}
CGameObject* CLockBlock::Clone(void* pArg)
{
    CLockBlock* pInstance = new CLockBlock(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CLockBlock"));
        Safe_Release(pInstance);
    }

    return pInstance;
}
void CLockBlock::Free()
{
    __super::Free();

    Safe_Release(m_pEffect);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModelCom);
    Safe_Release(m_pColliderCom);
    Safe_Release(m_pSoundCom);
}
