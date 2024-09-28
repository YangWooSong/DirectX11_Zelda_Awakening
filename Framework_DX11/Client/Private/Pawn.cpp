#include "stdafx.h"
#include "Pawn.h"
#include "GameInstance.h"
#include "Shader.h"
#include "Model.h"
#include "State_Pawn_Idle.h"
#include "State_Pawn_Walk.h"
#include "State_Pawn_Jump.h"
#include "State_Pawn_Damage.h"
#include "State_Pawn_DeadFall.h"

CPawn::CPawn(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CMonster{ pDevice, pContext }
{
}


CPawn::CPawn(const CPawn& Prototype)
    : CMonster{ Prototype }
{
}

HRESULT CPawn::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CPawn::Initialize(void* pArg)
{
	/* 직교투영을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_State()))
		return E_FAIL;

	//Read한 정보 세팅
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&pDesc->vPosition));
	//m_pTransformCom->Set_Scaled(pDesc->vScale.x, pDesc->vScale.y, pDesc->vScale.z);
	m_pTransformCom->RotationThreeAxis(_float3(0.f, 180.f, 0.f));
	//m_vRot = _float3(0.f, 180.f, 0.f);

	//m_pModelCom->SetUp_Animation(30, true);
	m_pFsmCom->Set_State(IDLE);
	m_isActive = false;

	return S_OK;
}

void CPawn::Priority_Update(_float fTimeDelta)
{
	if (m_isActive)
	{
		__super::Priority_Update(fTimeDelta);
	}
}

void CPawn::Update(_float fTimeDelta)
{
	if (m_isActive)
	{
		if (m_pNavigationCom != nullptr && m_pFsmCom->Get_CurrentState() != JUMP && m_pFsmCom->Get_CurrentState() != DEADFALL)
			m_pNavigationCom->SetUp_OnCell(m_pTransformCom, 0.0f, fTimeDelta);

		m_pFsmCom->Update(fTimeDelta);
		m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_Ptr());
		m_pModelCom->Play_Animation(fTimeDelta);
		m_pMonsterSoundCom->Update(fTimeDelta);

		__super::Update(fTimeDelta);
	}
}

void CPawn::Late_Update(_float fTimeDelta)
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

HRESULT CPawn::Render()
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
			m_pModelCom->Bind_MeshBoneMatrices(m_pShaderCom, "g_BoneMatrices", (_uint)i);

			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", TEXTURE_TYPE::DIFFUSE, (_uint)i)))
				return E_FAIL;


			if (FAILED(m_pShaderCom->Begin(0)))
				return E_FAIL;

			if (FAILED(m_pModelCom->Render((_uint)i)))
				return E_FAIL;
		}
	}

	return S_OK;
}

void CPawn::OnCollisionEnter(CGameObject* pOther)
{
	if (m_pColliderCom->Get_IsColl())
	{
		if (pOther->Get_LayerTag() == TEXT("Layer_Sword"))
		{
			Change_State(DAMAGE);
		}
	}
}

void CPawn::OnCollisionStay(CGameObject* pOther)
{
	if (m_pColliderCom->Get_IsColl())
	{
		if (pOther->Get_LayerTag() == TEXT("Layer_Pawn"))
		{
			_vector newLook = pOther->Get_Transform()->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			newLook = XMVector3Normalize(newLook);

			_vector newBodyPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + -newLook * 0.02f;
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, newBodyPos);
		}
	}
}

void CPawn::OnCollisionExit(CGameObject* pOther)
{
}

HRESULT CPawn::Ready_Components()
{
	__super::Ready_Components();

	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_DUNGEON, TEXT("Prototype_Component_Model_Pawn"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Navigation */
	CNavigation::NAVIGATION_DESC			NaviDesc{};

	NaviDesc.iCurrentIndex = m_iCellNum;
	NaviDesc.iOwnerType = CNavigation::NONPLAYER;

	if (FAILED(__super::Add_Component(LEVEL_DUNGEON, TEXT("Prototype_Component_Navigation"),
		TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &NaviDesc)))
		return E_FAIL;

	/* For.Com_Collider */
	CBounding_AABB::BOUNDING_AABB_DESC			ColliderDesc{};
	ColliderDesc.vExtents = _float3(0.5f, 0.5f, 0.5f);
	ColliderDesc.vCenter = _float3(0.f, 0.3f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;
	m_pColliderCom->Set_Owner(this);

	/* FOR.Com_Sound */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sound"),
		TEXT("Com_Sound"), reinterpret_cast<CComponent**>(&m_pMonsterSoundCom))))
		return E_FAIL;
	m_pMonsterSoundCom->Set_Owner(this);

	return S_OK;
}

HRESULT CPawn::Ready_State()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Fsm"),
		TEXT("Com_Fsm"), reinterpret_cast<CComponent**>(&m_pFsmCom), this)))
		return E_FAIL;

	m_pFsmCom->Add_State(CState_Pawn_Idle::Create(m_pFsmCom, this, IDLE));
	m_pFsmCom->Add_State(CState_Pawn_Walk::Create(m_pFsmCom, this, WALK));
	m_pFsmCom->Add_State(CState_Pawn_Jump::Create(m_pFsmCom, this, JUMP));
	m_pFsmCom->Add_State(CState_Pawn_Damage::Create(m_pFsmCom, this, DAMAGE));
	m_pFsmCom->Add_State(CState_Pawn_DeadFall::Create(m_pFsmCom, this, DEADFALL));
	return S_OK;
}


CPawn* CPawn::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPawn* pInstance = new CPawn(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CPawn"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CPawn::Clone(void* pArg)
{
	CPawn* pInstance = new CPawn(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CPawn"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPawn::Free()
{
	__super::Free();

	if (nullptr != m_pFsmCom)
		m_pFsmCom->Release_States();
	Safe_Release(m_pFsmCom);
	Safe_Release(m_pMonsterSoundCom);
}

