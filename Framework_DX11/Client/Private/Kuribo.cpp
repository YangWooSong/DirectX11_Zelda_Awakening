#include "stdafx.h"
#include "Kuribo.h"
#include "GameInstance.h"
#include "Shader.h"
#include "Model.h"
#include "State_Kuribo_Idle.h"
#include "State_Kuribo_Walk.h"
#include "State_Kuribo_Walk_Shake.h"
#include "State_Kuribo_Dead.h"

CKuribo::CKuribo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMonster{ pDevice, pContext }
{
}

CKuribo::CKuribo(const CKuribo& Prototype)
	: CMonster{ Prototype }
{
}

HRESULT CKuribo::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CKuribo::Initialize(void* pArg)
{
	/* 직교투영을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_State()))
		return E_FAIL;

	//m_pModelCom->SetUp_Animation(30, true);
	m_pFsmCom->Set_State(IDLE);
	m_eObjType = CGameObject::ANIM_MONSTER;
	return S_OK;
}

void CKuribo::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CKuribo::Update(_float fTimeDelta)
{
	m_pFsmCom->Update(fTimeDelta);

	m_pModelCom->Play_Animation(fTimeDelta);

	__super::Update(fTimeDelta);
}

void CKuribo::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CKuribo::Render()
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

HRESULT CKuribo::Ready_Components()
{
	__super::Ready_Components();

	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_FIELD, TEXT("Prototype_Component_Model_Kuribo"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CKuribo::Ready_State()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Fsm"),
		TEXT("Com_Fsm"), reinterpret_cast<CComponent**>(&m_pFsmCom), this)))
		return E_FAIL;

	m_pFsmCom->Add_State(CState_Kuribo_Idle::Create(m_pFsmCom, this, IDLE));
	m_pFsmCom->Add_State(CState_Kuribo_Walk::Create(m_pFsmCom, this, WALK));
	m_pFsmCom->Add_State(CState_Kuribo_Walk_Shake::Create(m_pFsmCom, this, WALK_SHAKE));
	m_pFsmCom->Add_State(CState_Kuribo_Dead::Create(m_pFsmCom, this, DEAD));
	return S_OK;
}

CKuribo* CKuribo::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CKuribo* pInstance = new CKuribo(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CKuribo"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CKuribo::Clone(void* pArg)
{
	CKuribo* pInstance = new CKuribo(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CKuribo"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CKuribo::Free()
{
	__super::Free();

	if (nullptr != m_pFsmCom)
		m_pFsmCom->Release_States();
	Safe_Release(m_pFsmCom);
}
