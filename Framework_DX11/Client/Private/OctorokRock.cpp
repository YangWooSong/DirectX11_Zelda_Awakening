#include "stdafx.h"
#include "OctorokRock.h"
#include "GameInstance.h"
#include "Monster.h"

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

    m_fSpeed = 7.f;
  //  m_isDead = true;
    return S_OK;
}

void COctorokRock::Priority_Update(_float fTimeDelta)
{
 
}

void COctorokRock::Update(_float fTimeDelta)
{
    if (m_bShoot)
    {
        //시작 설정
        Set_StartState();
        m_bShoot = false;
        m_bFollowParent = false;
    }

    if (m_pNavigationCom != nullptr)
        m_pNavigationCom->SetUp_OnCell(m_pTransformCom, m_fOffset.y, fTimeDelta);

    if(m_bFollowParent == true)
    {
        _vector vPosition = m_pParent->Get_Transform()->Get_State(CTransform::STATE_POSITION);
        XMVectorSetY(vPosition, XMVectorGetY(vPosition) + m_fOffset.y);
        m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
    }
    else
    {
        switch (m_iRockDir)
        {
        case FRONT:
            m_pTransformCom->Go_World_Straight(fTimeDelta, m_fSpeed, m_pNavigationCom, &m_bIsMove);
            break;
        case BACK:
            m_pTransformCom->Go_World_Backward(fTimeDelta, m_fSpeed, m_pNavigationCom, &m_bIsMove);
            break;
        case LEFT:
            m_pTransformCom->Go_World_Left(fTimeDelta, m_fSpeed, m_pNavigationCom, &m_bIsMove);
            break;
        case RIGHT:
            m_pTransformCom->Go_World_Right(fTimeDelta, m_fSpeed, m_pNavigationCom, &m_bIsMove);
            break;
        default:
            break;
        }
    }
  
    if (m_bIsMove == false )
    {
        m_bFollowParent = true;
        m_bIsMove = true;
    }

    m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_Ptr());
}
void COctorokRock::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);

    m_pGameInstance->Add_ColliderList(m_pColliderCom);
    m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
    }

HRESULT COctorokRock::Render()
{
  if(m_bFollowParent == false)
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
#ifdef _DEBUG
  m_pColliderCom->Render();
#endif
    return S_OK;
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

    return S_OK;
}

void COctorokRock::Set_Parent(CGameObject* pParent)
{
    m_pParent = static_cast<CMonster*>(pParent);
}

void COctorokRock::Set_StartState()
{
    m_iRockDir = m_pParent->Get_Monster_Dir();
    m_iCellNum = m_pParent->Get_CurrentCellNum();
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

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModelCom);
    Safe_Release(m_pNavigationCom);
    Safe_Release(m_pColliderCom);
}
