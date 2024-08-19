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
	
	return S_OK;
}

void CPawn::Priority_Update(_float fTimeDelta)
{
}

void CPawn::Update(_float fTimeDelta)
{
	m_pFsmCom->Update(fTimeDelta);

	m_pModelCom->Play_Animation(fTimeDelta);
}

void CPawn::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}
HRESULT CPawn::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_bIsDead", &m_isDead, sizeof(_bool))))
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

	//다른 모델한테 영향이 가면 안되서 dead처리를 풀어줘야 함
	_bool bFalse = false;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_bIsDead", &bFalse, sizeof(_bool))))
		return E_FAIL;

	return S_OK;
}

HRESULT CPawn::Ready_Components()
{
	__super::Ready_Components();

	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_FIELD, TEXT("Prototype_Component_Model_Pawn"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

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
}

