#include "stdafx.h"
#include "..\Public\Shield.h"

#include "Player.h"
#include "Link.h"
#include "GameInstance.h"

CShield::CShield(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{
}

CShield::CShield(const CShield& Prototype)
	: CPartObject{ Prototype }
{
}

HRESULT CShield::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CShield::Initialize(void* pArg)
{
	SHIELD_DESC* pDesc = static_cast<SHIELD_DESC*>(pArg);
	m_pSocketMatrix = pDesc->pSocketBoneMatrix;
	m_pPlayerFsm = pDesc->pPlayerFsm;
	m_pPlayerFsm->AddRef();

	/* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Scaled(1.f, 1.f, 1.f);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, -0.5f, 0.4f, 1.f));

	Set_LayerTag(TEXT("Layer_Shield"));

	return S_OK;
}

void CShield::Priority_Update(_float fTimeDelta)
{
}

void CShield::Update(_float fTimeDelta)
{
	//_matrix		SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

	for (size_t i = 0; i < 3; i++)
	{
	//	SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);
	}
	XMStoreFloat4x4(&m_WorldMatrix, XMLoadFloat4x4(m_pParentMatrix));

	//상태에 따라 활성화 결정
	if (m_pPlayerFsm->Get_CurrentState() == CLink::SHIELD || m_pPlayerFsm->Get_CurrentState() == CLink::SHIELD_WALK)
		m_pColliderCom->Set_IsActive(true);
	else
		m_pColliderCom->Set_IsActive(false);

	m_pColliderCom->Update(&m_WorldMatrix);
}

void CShield::Late_Update(_float fTimeDelta)
{
	/* 직교투영을 위한 월드행렬까지 셋팅하게 된다. */
	__super::Late_Update(fTimeDelta);

	m_pGameInstance->Add_ColliderList(m_pColliderCom);
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);

#ifdef _DEBUG
	m_pGameInstance->Add_DebugObject(m_pColliderCom);
#endif
}

HRESULT CShield::Render()
{
	return S_OK;
}

HRESULT CShield::Ready_Components()
{
	/* FOR.Com_Collider */
	CBounding_OBB::BOUNDING_OBB_DESC			ColliderDesc{};
	ColliderDesc.vExtents = _float3(0.5f, 1.f , 0.2f);
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vExtents.y, 0.5f);
	ColliderDesc.vAngles = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;
	m_pColliderCom->Set_Owner(this);
	m_pColliderCom->Set_IsActive(false);
	return S_OK;
}

CShield* CShield::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CShield* pInstance = new CShield(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CShield"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CShield::Clone(void* pArg)
{
	CShield* pInstance = new CShield(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CShield"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CShield::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pPlayerFsm);
}
