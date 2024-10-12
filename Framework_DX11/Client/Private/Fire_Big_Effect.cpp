#include "stdafx.h"
#include "Fire_Big_Effect.h"
#include "GameInstance.h"
#include "Fire_Plane_Effect.h"
#include "Particle_Image.h"
CFire_Big_Effect::CFire_Big_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:C2DEffects(pDevice, pContext)
{
}


CFire_Big_Effect::CFire_Big_Effect(const CFire_Big_Effect& Prototype)
	:C2DEffects(Prototype)
{
}


HRESULT CFire_Big_Effect::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CFire_Big_Effect::Initialize(void* pArg)
{
	/* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Child()))
		return E_FAIL;

	m_isActive = true;

	GAMEOBJECT_DESC* pDesc = static_cast<GAMEOBJECT_DESC*>(pArg);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&pDesc->vPosition));

	return S_OK;
}

void CFire_Big_Effect::Priority_Update(_float fTimeDelta)
{
	if (m_isActive)
	{
		for (auto& iter : m_Child_List)
			iter->SetActive(true);

		for (auto& iter : m_Child_List)
			iter->Priority_Update(fTimeDelta);
	}

}

void CFire_Big_Effect::Update(_float fTimeDelta)
{
	if (m_isActive)
	{
		for (auto& iter : m_Child_List)
			iter->Update(fTimeDelta);
	}
	else
	{
		if (m_bActiveChild)
		{
			m_bActiveChild = false;

			for (auto& iter : m_Child_List)
				iter->SetActive(false);
		}
	}
}

void CFire_Big_Effect::Late_Update(_float fTimeDelta)
{
	if (m_isActive)
	{
		for (auto& iter : m_Child_List)
			iter->Late_Update(fTimeDelta);
	}
}

HRESULT CFire_Big_Effect::Render()
{
	if (m_isActive)
	{
		for (auto& iter : m_Child_List)
			iter->Render();
	}
	return S_OK;
}
HRESULT CFire_Big_Effect::Ready_Child()
{
	CGameObject* pGameObj = { nullptr };

	pGameObj = m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Fire_Plane_Effect"));
	if (pGameObj != nullptr)
	{
		CFire_Plane_Effect::FIRE_EFFECT_DESC _Desc{};
		_Desc.iEffectType = m_iEffectType;
		_Desc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
		_Desc.vScale = { 0.7f, 0.9f, 0.1f };
		_Desc.iLevelIndex = m_iLevelIndex;
		_Desc.iDifuseNum = 0;
		CGameObject* p3DEffect = dynamic_cast<CGameObject*>(pGameObj->Clone(&_Desc));
		m_Child_List.push_back(p3DEffect);

		_Desc.iDifuseNum = 1;
		p3DEffect = dynamic_cast<CGameObject*>(pGameObj->Clone(&_Desc));
		m_Child_List.push_back(p3DEffect);
	}
	
	pGameObj = m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Glow_Effect"));
	
	if (pGameObj != nullptr)
	{
		C2DEffects::EFFECT_DESC pDesc{};
		pDesc.iLevelIndex = m_iLevelIndex;
		pDesc.pParent = this;
		pDesc.iEffectType = m_iEffectType;
		pDesc.fColor = { 1.f, 0.3f, 0.f, 1.f };
		pDesc.vScale = { 1.f,1.f,0.1f };
		CGameObject* m_pEffect = dynamic_cast<CGameObject*>(pGameObj->Clone(&pDesc));
		m_Child_List.push_back(m_pEffect);
	}

	pGameObj = m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Fire_Small_Effect"));

	if (pGameObj != nullptr)
	{
		C2DEffects::EFFECT_DESC pDesc{};
		pDesc.iLevelIndex = m_iLevelIndex;
		pDesc.pParent = this;
		pDesc.iEffectType = m_iEffectType;
		pDesc.fColor = { 1.f, 0.8f, 0.f, 1.f };
		pDesc.vScale = { 0.5f,0.5f,0.1f };
		CGameObject* m_pEffect = dynamic_cast<CGameObject*>(pGameObj->Clone(&pDesc));
		m_Child_List.push_back(m_pEffect);
	}

	pGameObj = m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Particle_Image"));
	if (pGameObj != nullptr)
	{
		CParticle_Image::IMAGE_PARTICLE_DESC pImageDesc = {};
		pImageDesc.iParticleType = m_iEffectType;
		pImageDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
		CGameObject* m_pEffect = dynamic_cast<CGameObject*>(pGameObj->Clone(&pImageDesc));
		m_Child_List.push_back(m_pEffect);
	}
	return S_OK;
}

CFire_Big_Effect* CFire_Big_Effect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CFire_Big_Effect* pInstance = new CFire_Big_Effect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CFire_Big_Effect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CFire_Big_Effect::Clone(void* pArg)
{
	CFire_Big_Effect* pInstance = new CFire_Big_Effect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CFire_Big_Effect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CFire_Big_Effect::Free()
{
	__super::Free();

}
