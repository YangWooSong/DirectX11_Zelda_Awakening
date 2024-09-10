#include "stdafx.h"
#include "Detector.h"
#include "GameInstance.h"
#include "Monster.h"

CDetector::CDetector(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CPartObject{ pDevice, pContext }
{
}

CDetector::CDetector(const CDetector& Prototype)
    : CPartObject{ Prototype }
{
}

HRESULT CDetector::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CDetector::Initialize(void* pArg)
{
	//m_pSocketMatrix = pDesc->pSocketBoneMatrix;

	DETECTOR_DESC* pDesc = static_cast<DETECTOR_DESC*>(pArg);
	m_pDetect = pDesc->pDetect;
	m_vSize = pDesc->vSize;
	m_vOffset = pDesc->vOffset;

	/* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Set_LayerTag(TEXT("Layer_Detector"));


	return S_OK;
}

void CDetector::Priority_Update(_float fTimeDelta)
{
}

void CDetector::Update(_float fTimeDelta)
{
	m_pColliderCom->Update(m_pParentMatrix);
}

void CDetector::Late_Update(_float fTimeDelta)
{
	/* 직교투영을 위한 월드행렬까지 셋팅하게 된다. */
	__super::Late_Update(fTimeDelta);

	m_pGameInstance->Add_ColliderList(m_pColliderCom);
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CDetector::Render()
{
#ifdef _DEBUG
	m_pColliderCom->Render();
#endif
	return S_OK;
}

void CDetector::OnCollisionEnter(CGameObject* pOther)
{
	if (m_pColliderCom->Get_IsColl())
	{
		if (pOther->Get_LayerTag() == TEXT("Layer_Player"))
		{
			*m_pDetect = true;
		}
	}
}

void CDetector::OnCollisionStay(CGameObject* pOther)
{
}

void CDetector::OnCollisionExit(CGameObject* pOther)
{
	if (m_pColliderCom->Get_IsColl())
	{
		if (pOther->Get_LayerTag() == TEXT("Layer_Player"))
		{
			*m_pDetect = false;
		}
	}
}

void CDetector::Set_Active_Collider(_bool bActive)
{
	m_pColliderCom->Set_IsActive(bActive);
}

HRESULT CDetector::Ready_Components()
{
	/* FOR.Com_Collider */
	CBounding_OBB::BOUNDING_OBB_DESC			ColliderDesc{};
	ColliderDesc.vExtents = _float3(m_vSize.x, m_vSize.y, m_vSize.z);
	ColliderDesc.vCenter = _float3(0.f + m_vOffset.x, ColliderDesc.vExtents.y + m_vOffset.y, 0.f + m_vOffset.z);
	ColliderDesc.vAngles = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;
	m_pColliderCom->Set_Owner(this);

	return S_OK;
}

CDetector* CDetector::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDetector* pInstance = new CDetector(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CDetector"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDetector::Clone(void* pArg)
{
	CDetector* pInstance = new CDetector(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CDetector"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDetector::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);

}
