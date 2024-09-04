#include "stdafx.h"
#include "DeguTail_00.h"
#include "GameInstance.h"
#include "Shader.h"
#include "Model.h"
#include "State_DeguTail_Idle.h"
#include "State_DeguTail_Walk.h"
#include "DeguTail_01.h"
#include "DeguTail_02.h"

CDeguTail_00::CDeguTail_00(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CMonster{ pDevice, pContext }
{
}


CDeguTail_00::CDeguTail_00(const CDeguTail_00& Prototype)
    : CMonster{ Prototype }
{
}

HRESULT CDeguTail_00::Initialize_Prototype()
{
    return S_OK;
}


HRESULT CDeguTail_00::Initialize(void* pArg)
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

	m_pFsmCom->Set_State(WALK);

	m_iDir = (int)m_pGameInstance->Get_Random(0, 4);
	m_pTransformCom->RotationThreeAxis(_float3(0.f, 180.f, 0.f));
	return S_OK;
}
void CDeguTail_00::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}


void CDeguTail_00::Update(_float fTimeDelta)
{
	if (m_pNavigationCom != nullptr)
		m_pNavigationCom->SetUp_OnCell(m_pTransformCom, 0.5f, fTimeDelta);

	m_pFsmCom->Update(fTimeDelta);
	m_pModelCom->Play_Animation(fTimeDelta);

	__super::Update(fTimeDelta);

	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_Ptr());
}

void CDeguTail_00::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}
HRESULT CDeguTail_00::Render()
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

#ifdef _DEBUG
	m_pColliderCom->Render();
#endif

	return S_OK;
}

HRESULT CDeguTail_00::Ready_Components()
{
	__super::Ready_Components();

	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_DUNGEON, TEXT("Prototype_Component_Model_DeguTail01"),
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
	ColliderDesc.vExtents = _float3(0.6f, 0.7f, 0.6f);
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vExtents.y, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CDeguTail_00::Ready_PartObjects()
{
	m_Parts.resize(PART_END);

	CDeguTail_01::DEGUBODY_DESC		BodyDesc{};
	BodyDesc.vSize = _float3(1.f, 1.f, 1.f);
	BodyDesc.iCellNum = m_pNavigationCom->Get_CurrentCellIndex();
	BodyDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
	BodyDesc.pParent = this;
	BodyDesc.iBodyNum = 0;

	if (FAILED(__super::Add_PartObject(PART_BODY0, TEXT("Prototype_GameObject_DeguTail_01"), &BodyDesc)))
		return E_FAIL;

	BodyDesc.vSize = _float3(0.7f, 0.7f, 0.7f);
	BodyDesc.iCellNum = m_pNavigationCom->Get_CurrentCellIndex();
	BodyDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
	BodyDesc.pParent = this;
	BodyDesc.iBodyNum = 1;

	if (FAILED(__super::Add_PartObject(PART_BODY1, TEXT("Prototype_GameObject_DeguTail_01"), &BodyDesc)))
		return E_FAIL;

	BodyDesc.vSize = _float3(0.5f, 0.5f, 0.5f);
	BodyDesc.iCellNum = m_pNavigationCom->Get_CurrentCellIndex();
	BodyDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
	BodyDesc.pParent = this;
	BodyDesc.iBodyNum = 2;

	if (FAILED(__super::Add_PartObject(PART_BODY2, TEXT("Prototype_GameObject_DeguTail_01"), &BodyDesc)))
		return E_FAIL;


	CDeguTail_02::DEGUTAIL_DESC		TailDesc{};
	TailDesc.vSize = _float3(0.5f, 0.5f, 0.5f);
	TailDesc.iCellNum = m_pNavigationCom->Get_CurrentCellIndex();
	TailDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
	TailDesc.pParent = this;

	if (FAILED(__super::Add_PartObject(PART_TAIL, TEXT("Prototype_GameObject_DeguTail_02"), &TailDesc)))
		return E_FAIL;
    return S_OK;
}

HRESULT CDeguTail_00::Ready_State()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Fsm"),
		TEXT("Com_Fsm"), reinterpret_cast<CComponent**>(&m_pFsmCom), this)))
		return E_FAIL;

	m_pFsmCom->Add_State(CState_DeguTail_Idle::Create(m_pFsmCom, this, IDLE));
	m_pFsmCom->Add_State(CState_DeguTail_Walk::Create(m_pFsmCom, this, WALK));
    return S_OK;
}

CDeguTail_00* CDeguTail_00::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDeguTail_00* pInstance = new CDeguTail_00(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CDeguTail_00"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDeguTail_00::Clone(void* pArg)
{
	CDeguTail_00* pInstance = new CDeguTail_00(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CDeguTail_00"));
		Safe_Release(pInstance);
	}

	return pInstance;
}
void CDeguTail_00::Free()
{
	__super::Free();

	if (nullptr != m_pFsmCom)
		m_pFsmCom->Release_States();
	Safe_Release(m_pFsmCom);
}

