#include "stdafx.h"
#include "Teleport.h"
#include "GameInstance.h"
#include "Level_Loading.h"

CTeleport::CTeleport(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}

CTeleport::CTeleport(const CTeleport& Prototype)
    : CGameObject{ Prototype }
{
}

HRESULT CTeleport::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CTeleport::Initialize(void* pArg)
{
	TELEPORT_DESC* pDesc = static_cast<TELEPORT_DESC*>(pArg);
	m_iNextLevelIndex = pDesc->iNextLevelIndex;

	/* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&pDesc->vPosition));
	m_pTransformCom->Set_Scaled(pDesc->vScale.x, pDesc->vScale.y, pDesc->vScale.z);

	return S_OK;
}

void CTeleport::Priority_Update(_float fTimeDelta)
{
}

void CTeleport::Update(_float fTimeDelta)
{

	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_Ptr());

}

void CTeleport::Late_Update(_float fTimeDelta)
{

	m_pGameInstance->Add_ColliderList(m_pColliderCom);
	
#ifdef _DEBUG
	m_pGameInstance->Add_DebugObject(m_pColliderCom);
#endif
}

HRESULT CTeleport::Render()
{
    return S_OK;
}

void CTeleport::OnCollisionEnter(CGameObject* pOther)
{

}

void CTeleport::OnCollisionStay(CGameObject* pOther)
{
	if (m_pColliderCom->Get_IsColl())
	{
		if (pOther->Get_LayerTag() == TEXT("Layer_Player"))
		{
			m_bChangeScene = true;
		}
	}
}

void CTeleport::OnCollisionExit(CGameObject* pOther)
{
	if (pOther->Get_LayerTag() == TEXT("Layer_Player"))
	{
		m_bChangeScene = false;
	}
}

HRESULT CTeleport::Ready_Components()
{
	/* For.Com_Collider */
	CBounding_AABB::BOUNDING_AABB_DESC			ColliderDesc{};
	ColliderDesc.vExtents = m_pTransformCom->Get_Scaled();
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vExtents.y, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;
	m_pColliderCom->Set_Owner(this);

	return S_OK;
}

CTeleport* CTeleport::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTeleport* pInstance = new CTeleport(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CTeleport"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTeleport::Clone(void* pArg)
{
	CTeleport* pInstance = new CTeleport(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CTeleport"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTeleport::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
}

