#include "stdafx.h"
#include "..\Public\Sword.h"

#include "Player.h"
#include "Link.h"
#include "GameInstance.h"

CSword::CSword(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{
}

CSword::CSword(const CSword& Prototype)
	: CPartObject{ Prototype }
{
}

HRESULT CSword::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSword::Initialize(void* pArg)
{
	SWORD_DESC* pDesc = static_cast<SWORD_DESC*>(pArg);
	m_pSocketMatrix = pDesc->pSocketBoneMatrix;
	m_pPlayerFsm = pDesc->pPlayerFsm;
	m_pPlayerFsm->AddRef();

	/* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Scaled(1.f, 1.f, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, -0.4f, 1.f));

	return S_OK;
}

void CSword::Priority_Update(_float fTimeDelta)
{
}

void CSword::Update(_float fTimeDelta)
{
	_matrix		SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

	for (size_t i = 0; i < 3; i++)
	{
		SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);
	}
	XMStoreFloat4x4(&m_WorldMatrix, XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()) * SocketMatrix * XMLoadFloat4x4(m_pParentMatrix));

	
	m_pColliderCom->Update(&m_WorldMatrix);

	//상태에 따라 활성화 결정
}

void CSword::Late_Update(_float fTimeDelta)
{
	/* 직교투영을 위한 월드행렬까지 셋팅하게 된다. */
	__super::Late_Update(fTimeDelta);
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CSword::Render()
{
#ifdef _DEBUG
	if (m_pPlayerFsm->Get_CurrentState() == CLink::SLASH || m_pPlayerFsm->Get_CurrentState() == CLink::SLASH_HOLD)
		m_pColliderCom->Render();
#endif
	return S_OK;
}

HRESULT CSword::Ready_Components()
{
	/* FOR.Com_Collider */
	CBounding_OBB::BOUNDING_OBB_DESC			ColliderDesc{};
	ColliderDesc.vExtents = _float3(0.1f, 0.1f, 0.4f);
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vExtents.y, 0.f);
	ColliderDesc.vAngles = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

CSword* CSword::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSword* pInstance = new CSword(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CSword"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CSword::Clone(void* pArg)
{
	CSword* pInstance = new CSword(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CSword"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSword::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pPlayerFsm);
}
