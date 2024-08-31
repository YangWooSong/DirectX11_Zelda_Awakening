#include "stdafx.h"
#include "Octorok.h"
#include "GameInstance.h"
#include "Shader.h"
#include "Model.h"
#include "State_Octorok_Idle.h"
#include "State_Octorok_Dead.h"
#include "State_Octorok_Attack.h"
#include "State_Octorok_Walk.h"
#include "OctorokRock.h"

COctorok::COctorok(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMonster{ pDevice, pContext }
{
}

COctorok::COctorok(const COctorok& Prototype)
	: CMonster{ Prototype }
{
}
HRESULT COctorok::Initialize_Prototype()
{
	return S_OK;
}

HRESULT COctorok::Initialize(void* pArg)
{

	/* 직교투영을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;
	
	if (FAILED(Ready_PartObjects()))
		return E_FAIL;

	if (FAILED(Ready_State()))
		return E_FAIL;

	//m_pModelCom->SetUp_Animation(30, true);
	m_pFsmCom->Set_State(IDLE);
	
	m_iDir = (int)m_pGameInstance->Get_Random(0, 4);
	m_pTransformCom->RotationThreeAxis(_float3(0.f, 180.f, 0.f));

	return S_OK;
}

void COctorok::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void COctorok::Update(_float fTimeDelta)
{
	if (m_pNavigationCom != nullptr)
		m_pNavigationCom->SetUp_OnCell(m_pTransformCom, 0.f, fTimeDelta);

	m_pFsmCom->Update(fTimeDelta);
	m_pModelCom->Play_Animation(fTimeDelta);

	__super::Update(fTimeDelta);
}

void COctorok::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT COctorok::Render()
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


HRESULT COctorok::Ready_Components()
{
	__super::Ready_Components();

	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_FIELD, TEXT("Prototype_Component_Model_Octorok"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;


	/* For.Com_Navigation */
	CNavigation::NAVIGATION_DESC			NaviDesc{};

	NaviDesc.iCurrentIndex = m_iCellNum;
	NaviDesc.iOwnerType = CNavigation::NONPLAYER;

	if (FAILED(__super::Add_Component(LEVEL_FIELD, TEXT("Prototype_Component_Navigation"),
		TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &NaviDesc)))
		return E_FAIL;
	
	return S_OK;
}

HRESULT COctorok::Ready_PartObjects()
{
	/* 실제 추가하고 싶은 파트오브젝트의 갯수만큼 밸류를 셋팅해놓자. */
	m_Parts.resize(PART_END);

	COctorokRock::OCTOROKROCK_DESC		RockDesc{};
	RockDesc.iDir = m_iDir;
	RockDesc.iCellNum = m_pNavigationCom->Get_CurrentCellIndex();
	RockDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
	RockDesc.pParent = this;

	if (FAILED(__super::Add_PartObject(PART_BULLET, TEXT("Prototype_GameObject_OctorokRock"), &RockDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT COctorok::Ready_State()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Fsm"),
		TEXT("Com_Fsm"), reinterpret_cast<CComponent**>(&m_pFsmCom), this)))
		return E_FAIL;

	m_pFsmCom->Add_State(CState_Octorok_Idle::Create(m_pFsmCom, this, IDLE));
	m_pFsmCom->Add_State(CState_Octorok_Walk::Create(m_pFsmCom, this, WALK));
	m_pFsmCom->Add_State(CState_Octorok_Attack::Create(m_pFsmCom, this, ATTACK));
	m_pFsmCom->Add_State(CState_Octorok_Dead::Create(m_pFsmCom, this, DEAD));

	return S_OK;
}

COctorok* COctorok::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	COctorok* pInstance = new COctorok(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : COctorok"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* COctorok::Clone(void* pArg)
{
	COctorok* pInstance = new COctorok(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : COctorok"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void COctorok::Free()
{
	__super::Free();

	if (nullptr != m_pFsmCom)
		m_pFsmCom->Release_States();
	Safe_Release(m_pFsmCom);
}

