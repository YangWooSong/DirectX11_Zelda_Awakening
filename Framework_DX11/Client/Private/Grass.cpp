#include "stdafx.h"
#include "Grass.h"
#include "GameInstance.h"
#include "Particle_Model.h"
#include "Player.h"
#include "Lupee.h"


CGrass::CGrass(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CGameObject(pDevice, pContext)
{
}

CGrass::CGrass(const CGrass& Prototype)
    :CGameObject(Prototype)
{
}

HRESULT CGrass::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CGrass::Initialize(void* pArg)
{
    GAMEOBJECT_DESC* pDesc = static_cast<GAMEOBJECT_DESC*>(pArg);

    GRASS_DESC* pGrassDesc = static_cast<GRASS_DESC*>(pArg);
    m_iGrassType = pGrassDesc->iGrassType;

    /* 직교퉁여을 위한 데이터들을 모두 셋. */
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;
    
    if (FAILED(Ready_Particle()))
        return E_FAIL;

    //Read한 정보 세팅
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&pDesc->vPosition));
    m_pTransformCom->Set_Scaled(pDesc->vScale.x, pDesc->vScale.y, pDesc->vScale.z);
    m_pTransformCom->RotationThreeAxis(pDesc->vRotation);
    m_vRot = pDesc->vRotation;

    m_pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Player(LEVEL_FIELD));
    Safe_AddRef(m_pPlayer);

    return S_OK;
}

void CGrass::Priority_Update(_float fTimeDelta)
{
}

void CGrass::Update(_float fTimeDelta)
{
    if (m_bCut)
    {
        if (m_bPlaySound == false)
        {
            m_bPlaySound = true;
            m_pSoundCom->Play_Sound(TEXT("4_Obj_Cut_Grass.wav"), 0.6f);
        }
        m_pColliderCom->Set_IsActive(false);
        m_pParticle->Update(fTimeDelta);

        if(m_bCreate_Lupee == false)
            Drop_Lupee();
    }
    else if(m_pColliderCom->IsActive())
        m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_Ptr());

    Culculate_Distance_Player();
    m_pParticle->Get_Transform()->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
}

void CGrass::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);
    m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);

    if (m_bCut == false && m_bAddColliderList)
       m_pGameInstance->Add_ColliderList(m_pColliderCom);
    else if(m_bCut)
        m_pParticle->Late_Update(fTimeDelta);

#ifdef _DEBUG
    m_pGameInstance->Add_DebugObject(m_pColliderCom);
#endif
}

HRESULT CGrass::Render()
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
        if (m_bCut)
        {
            if (i == 0)
                continue;
        }
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", TEXTURE_TYPE::DIFFUSE, i)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(0)))
            return E_FAIL;

        if (FAILED(m_pModelCom->Render(i)))
            return E_FAIL;
    }


    if (m_bCut)
        m_pParticle->Render();

    return S_OK;
}

void CGrass::OnCollisionEnter(CGameObject* pOther)
{
    if (m_pColliderCom->Get_IsColl())
    {
        if (pOther->Get_LayerTag() == TEXT("Layer_Sword"))
        {
            m_bCut = true;
        }
    }
}

void CGrass::OnCollisionStay(CGameObject* pOther)
{
}

void CGrass::OnCollisionExit(CGameObject* pOther)
{
}

HRESULT CGrass::Ready_Components()
{
    /* FOR.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxModel"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* FOR.Com_Model */
    if (m_iGrassType == GRASS)
    {
        if (FAILED(__super::Add_Component(LEVEL_FIELD, TEXT("Prototype_Component_Model_Grass"),
            TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
            return E_FAIL;
    }
    else if (m_iGrassType == LAWN)
    {
        if (FAILED(__super::Add_Component(LEVEL_FIELD, TEXT("Prototype_Component_Model_Lawn"),
            TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
            return E_FAIL;
    }

    /* FOR.Com_Sound */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sound"),
        TEXT("Com_Sound"), reinterpret_cast<CComponent**>(&m_pSoundCom))))
        return E_FAIL;
    m_pSoundCom->Set_Owner(this);

    /* For.Com_Collider */
    CBounding_AABB::BOUNDING_AABB_DESC			ColliderDesc{};

    if(m_iGrassType == GRASS)
    {
        ColliderDesc.vExtents = _float3(0.5f, 0.5f, 0.5f);
        ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vExtents.y, 0.f);
    }
    else if (m_iGrassType == LAWN)
    {
        ColliderDesc.vExtents = _float3(1.f, 0.5f, 1.f);
        ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vExtents.y, 0.f);
    }

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
        TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
        return E_FAIL;
    m_pColliderCom->Set_Owner(this);

    return S_OK;
}

HRESULT CGrass::Ready_Particle()
{
    switch (m_iGrassType)
    {
    case GRASS:
    {

        CParticle_Model::MODEL_PARTICLE_DESC Desc = {};
        Desc.iParticleType = CParticle_Model::GRASS;

        m_pParticle = m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Particle_Model"), &Desc);
    }
        break;
    case LAWN:   
    {

        CParticle_Model::MODEL_PARTICLE_DESC Desc = {};
        Desc.iParticleType = CParticle_Model::LAWN;

        m_pParticle = m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Particle_Model"), &Desc);
    }
            break;
    default:
        break;
    }
    return S_OK;
}

void CGrass::Culculate_Distance_Player()
{
    _vector vPlayerPos = m_pPlayer->Get_Position();

    _float fDistance = XMVectorGetX(XMVector3Length(vPlayerPos - m_pTransformCom->Get_State(CTransform::STATE_POSITION)));

    if (fabs(fDistance) < 3.f)
        m_bAddColliderList = true;
    else
        m_bAddColliderList = false;
}

HRESULT CGrass::Drop_Lupee()
{
    m_bCreate_Lupee = true;
    _int iRandom = (int)m_pGameInstance->Get_Random(0, 10);

    if (iRandom == 3)
    {
        CGameObject::GAMEOBJECT_DESC Desc{};

        _float3 vPos;
        XMStoreFloat3(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
        vPos.y += 0.5f;
        Desc.vPosition = vPos;
        Desc.vScale = _float3(1.f,1.f,1.f);

        if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_FIELD, TEXT("Layer_Lupee"), TEXT("Prototype_GameObject_Lupee"), &Desc)))
            return E_FAIL;
    }
}

CGrass* CGrass::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CGrass* pInstance = new CGrass(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CGrass"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CGrass::Clone(void* pArg)
{
    CGrass* pInstance = new CGrass(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CGrass"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CGrass::Free()
{
    __super::Free();

    Safe_Release(m_pParticle);
    Safe_Release(m_pPlayer);

    Safe_Release(m_pSoundCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModelCom);
    Safe_Release(m_pColliderCom);
}
