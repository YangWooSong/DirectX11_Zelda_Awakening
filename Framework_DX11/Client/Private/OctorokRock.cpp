#include "stdafx.h"
#include "OctorokRock.h"
#include "GameInstance.h"
#include "Monster.h"
#include "Octorok.h"
#include "Detector.h"
#include"Particle_Model.h"

COctorokRock::COctorokRock(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CPartObject(pDevice, pContext)
{
}

COctorokRock::COctorokRock(const COctorokRock& Prototype)
    :CPartObject(Prototype)
{
}

HRESULT COctorokRock::Initialize_Prototype()
{
    return S_OK;
}

HRESULT COctorokRock::Initialize(void* pArg)
{
    OCTOROKROCK_DESC* pDesc = static_cast<OCTOROKROCK_DESC*>(pArg);

    /* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    m_fOffset = { 0.f, 0.5f, 0.f };
    m_pTransformCom->Set_Scaled(0.8f, 0.8f, 0.8f);
    m_iRockDir = pDesc->iDir;
    m_iCellNum = pDesc->iCellNum;
    m_pParent = pDesc->pParent;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_eObjType = CGameObject::ANIM_MONSTER;

    Set_LayerTag(TEXT("Layer_OctorokRock"));

    m_fSpeed = 7.f;
  //  m_isDead = true;



    return S_OK;
}

void COctorokRock::Priority_Update(_float fTimeDelta)
{
    if (m_bActiveParticle)
    {
        if (m_pParticle != nullptr)
            m_pParticle->Priority_Update(fTimeDelta);
    }
}

void COctorokRock::Update(_float fTimeDelta)
{
   
    if (m_bShoot)
    {
        //시작 설정
        Set_StartState();
        m_bShoot = false;
        m_bRender = true;
        m_isDead = false;
    }

    if (m_pNavigationCom != nullptr)
        m_pNavigationCom->SetUp_OnCell(m_pTransformCom, m_fOffset.y, fTimeDelta);

    //문어 속에 있을 때, 문어 죽을때 
    if(m_bRender == false || m_pParent->Get_Dead() == true)
    {
        _vector vPosition = m_pParent->Get_Transform()->Get_State(CTransform::STATE_POSITION);
        XMVectorSetY(vPosition, XMVectorGetY(vPosition) + m_fOffset.y);
        m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
        m_pColliderCom->Set_IsActive(false);
    }
    else
    {
        m_pColliderCom->Set_IsActive(true);
    

        switch (m_iRockDir)
        {
        case FRONT:
            m_pTransformCom->Go_World_Straight_Slide(fTimeDelta, m_fSpeed, m_pNavigationCom, &m_bIsMove);
            break;
        case BACK:
            m_pTransformCom->Go_World_Backward_Slide(fTimeDelta, m_fSpeed, m_pNavigationCom, &m_bIsMove);
            break;
        case LEFT:
            m_pTransformCom->Go_World_Left_Slide(fTimeDelta, m_fSpeed, m_pNavigationCom, &m_bIsMove);
            break;
        case RIGHT:
            m_pTransformCom->Go_World_Right_Slide(fTimeDelta, m_fSpeed, m_pNavigationCom, &m_bIsMove);
            break;
        default:
            break;
        }
    }

    if (m_bIsMove == false)
    {
        Break();
    }


    if (m_bActiveParticle)
    {
        m_fParticleTimer += fTimeDelta;

        if (m_fParticleTimer > 1.f)
        {
            m_fParticleTimer = 0.f;
            m_fParticleTimer = false;
        }

        if (m_pParticle != nullptr)
           m_pParticle->Update(fTimeDelta);
    }

    m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_Ptr());
}
void COctorokRock::Late_Update(_float fTimeDelta)
{
    if (true == m_pGameInstance->isIn_Frustum_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 3.f))
    {
        __super::Late_Update(fTimeDelta);

        if(m_bRender)
        {
            m_pGameInstance->Add_ColliderList(m_pColliderCom);
            m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
            m_pGameInstance->Add_RenderObject(CRenderer::RG_SHADOWOBJ, this);
        }

        if (m_bActiveParticle)
        {
            if (m_pParticle != nullptr)
                m_pParticle->Late_Update(fTimeDelta);
        }
#ifdef _DEBUG
        m_pGameInstance->Add_DebugObject(m_pColliderCom);
#endif
    }
}

HRESULT COctorokRock::Render()
{
  if(m_bRender == true)
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

          if (FAILED(m_pShaderCom->Begin(0)))
              return E_FAIL;

          if (FAILED(m_pModelCom->Render((_uint)i)))
              return E_FAIL;
      }

  }

  if (m_bActiveParticle)
  {
      if(m_pParticle != nullptr)
         m_pParticle->Render();
  }

    return S_OK;
}

HRESULT COctorokRock::Render_LightDepth()
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

        if (FAILED(m_pShaderCom->Begin(13)))
            return E_FAIL;

        if (FAILED(m_pModelCom->Render((_uint)i)))
            return E_FAIL;
    }

    return S_OK;
}

void COctorokRock::OnCollisionEnter(CGameObject* pOther)
{
    if (m_pColliderCom->Get_IsColl())
    {
        if (pOther->Get_LayerTag() != TEXT("Layer_Octorok") && pOther->Get_LayerTag() != TEXT("Layer_Detector"))
            Break();
    }
}

void COctorokRock::OnCollisionStay(CGameObject* pOther)
{
}

void COctorokRock::OnCollisionExit(CGameObject* pOther)
{
}

HRESULT COctorokRock::Ready_Components()
{
    /* FOR.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxModel"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* FOR.Com_Model */
    if (FAILED(__super::Add_Component(LEVEL_FIELD, TEXT("Prototype_Component_Model_OctorokRock"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    /* For.Com_Navigation */
    CNavigation::NAVIGATION_DESC			NaviDesc{};

    NaviDesc.iCurrentIndex = m_iCellNum;
    NaviDesc.iOwnerType = CNavigation::NONPLAYER;

    if (FAILED(__super::Add_Component(LEVEL_FIELD, TEXT("Prototype_Component_Navigation"),
        TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &NaviDesc)))
        return E_FAIL;

    /* For.Com_Collider */
    CBounding_AABB::BOUNDING_AABB_DESC			ColliderDesc{};
    ColliderDesc.vExtents = _float3(0.2f, 0.2f, 0.2f);
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

void COctorokRock::Set_Parent(CGameObject* pParent)
{
    m_pParent = static_cast<CMonster*>(pParent);
}

void COctorokRock::Break()
{
    _float fVolume = m_pSoundCom->Culculate_Volume_Distance(m_pTransformCom->Get_State(CTransform::STATE_POSITION), m_pGameInstance->Find_Player(LEVEL_FIELD)->Get_Transform()->Get_State(CTransform::STATE_POSITION), 5.f, 0.4f);
    m_pSoundCom->Play_Sound(TEXT("3_Octarock_RockBreak.wav"), fVolume);
    m_bRender = false;
    m_bIsMove = true;
    m_isDead = false;

    m_bActiveParticle = true;

    if (m_pParticle != nullptr)
        Safe_Release(m_pParticle);

    CParticle_Model::MODEL_PARTICLE_DESC Desc = {};
    Desc.iParticleType = CParticle_Model::ROCK;

    m_pParticle = m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Particle_Model"), &Desc);
    m_pParticle->Get_Transform()->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
}

void COctorokRock::Set_StartState()
{
    m_iRockDir = m_pParent->Get_Monster_Dir();
    m_iCellNum = m_pParent->Get_CurrentCellNum();

    _vector vPosition = m_pParent->Get_Transform()->Get_State(CTransform::STATE_POSITION);
    XMVectorSetY(vPosition, XMVectorGetY(vPosition) + m_fOffset.y);
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
}

COctorokRock* COctorokRock::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    COctorokRock* pInstance = new COctorokRock(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : COctorokRock"));
        Safe_Release(pInstance);
    }

    return pInstance;
}
CGameObject* COctorokRock::Clone(void* pArg)
{
    COctorokRock* pInstance = new COctorokRock(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : COctorokRock"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void COctorokRock::Free()
{
    __super::Free();

    Safe_Release(m_pParticle);

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModelCom);
    Safe_Release(m_pNavigationCom);
    Safe_Release(m_pColliderCom);
    Safe_Release(m_pSoundCom);
}
