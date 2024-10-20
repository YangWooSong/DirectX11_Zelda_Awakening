#include "stdafx.h"
#include "SquareBlock.h"
#include "GameInstance.h"
#include "Link.h"

CSquareBlock::CSquareBlock(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CGameObject(pDevice, pContext)
{
}

CSquareBlock::CSquareBlock(const CSquareBlock& Prototype)
    :CGameObject(Prototype)
{
}
HRESULT CSquareBlock::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CSquareBlock::Initialize(void* pArg)
{
    SQUARE_DESC* pSquareDesc = static_cast<SQUARE_DESC*>(pArg);
    m_bColliderActive = pSquareDesc->bActiveCollider;

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

    if(m_bColliderActive)
        m_pGameInstance->AddScene_ColMesh(this, TEXT("SquareBlock"));
    
    m_pPlayer = static_cast<CLink*>(m_pGameInstance->Find_Player(LEVEL_DUNGEON));

    m_isActive = false;
    return S_OK;
}

void CSquareBlock::Priority_Update(_float fTimeDelta)
{
}

void CSquareBlock::Update(_float fTimeDelta)
{
    if (m_isActive && m_bColliderActive)
    {
        if (m_pPlayer->Get_Fsm()->Get_CurrentState() == CLink::PUSH)
        {
            m_iDir = m_pPlayer->Get_Player_Dir();
            if(m_iDir == CLink::LEFT)
                m_bPushed = true;
        }

        if(m_bPushed && m_bStopMove == false)
            Move(fTimeDelta);

        m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_Ptr());
    }
}

void CSquareBlock::Late_Update(_float fTimeDelta)
{
    if (m_isActive)
    {
        m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
        m_pGameInstance->Add_RenderObject(CRenderer::RG_SHADOWOBJ, this);
        if(m_pColliderCom != nullptr && m_bPushed == false)
            m_pGameInstance->Add_ColliderList(m_pColliderCom);

#ifdef _DEBUG
        if (m_pColliderCom != nullptr)
            m_pGameInstance->Add_DebugObject(m_pColliderCom);
#endif
       
    }

}

HRESULT CSquareBlock::Render()
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
    return S_OK;
}

HRESULT CSquareBlock::Render_LightDepth()
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

HRESULT CSquareBlock::Ready_Components()
{
    /* FOR.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxModel"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* FOR.Com_Model */
    if (FAILED(__super::Add_Component(LEVEL_DUNGEON, TEXT("Prototype_Component_Model_SquareBlock"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    if(m_bColliderActive)
    {
        /* For.Com_Collider */
        CBounding_AABB::BOUNDING_AABB_DESC			ColliderDesc{};
        ColliderDesc.vExtents = _float3(0.9f, 0.9f, 0.9f);
        ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vExtents.y, 0.f);

        if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
            TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
            return E_FAIL;
        m_pColliderCom->Set_Owner(this);
    }

    return S_OK;
}

void CSquareBlock::Move(_float fTimeDelta)
{
    if (m_bMoveActor == false)
    {
        m_bMoveActor = true;
        m_pGameInstance->Destroy_PhysXActor(this);
        m_pPlayer->Get_EffectSound()->Play_Sound(TEXT("4_Obj_Block_Push.wav"), 0.8f);
    }

    _float3 curPos = {};
    XMStoreFloat3(&curPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

    if (fabs(curPos.x - m_vTarget.x) < 0.1f)
        m_bStopMove = true;
    else
        m_pTransformCom->Go_Lerp(curPos, m_vTarget, 0.02f);
}

CSquareBlock* CSquareBlock::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CSquareBlock* pInstance = new CSquareBlock(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CSquareBlock"));
        Safe_Release(pInstance);
    }

    return pInstance;
}
CGameObject* CSquareBlock::Clone(void* pArg)
{
    CSquareBlock* pInstance = new CSquareBlock(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CSquareBlock"));
        Safe_Release(pInstance);
    }

    return pInstance;
}
void CSquareBlock::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModelCom);
    if (m_bColliderActive)
    {
        Safe_Release(m_pColliderCom);
    }
}
