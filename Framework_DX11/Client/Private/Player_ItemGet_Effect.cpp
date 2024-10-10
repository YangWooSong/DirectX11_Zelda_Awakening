#include "stdafx.h"
#include "Player_ItemGet_Effect.h"
#include "GameInstance.h"
#include "Particle_Image.h"
#include "Flash_Effect.h"
#include "Ripple_Effect.h"
#include "Halo_Effect.h"
CPlayer_ItemGet::CPlayer_ItemGet(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:C2DEffects(pDevice, pContext)
{
}


CPlayer_ItemGet::CPlayer_ItemGet(const CPlayer_ItemGet& Prototype)
	:C2DEffects(Prototype)
{
}


HRESULT CPlayer_ItemGet::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayer_ItemGet::Initialize(void* pArg)
{
	/* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Child()))
		return E_FAIL;

	m_isActive = false;

	return S_OK;
}

void CPlayer_ItemGet::Priority_Update(_float fTimeDelta)
{
	if (m_isActive)
	{
		for (auto& iter : m_Child_List)
			iter->SetActive(true);

		for (auto& iter : m_Child_List)
			iter->Priority_Update(fTimeDelta);
	}
}

void CPlayer_ItemGet::Update(_float fTimeDelta)
{
	_vector vParentPos = m_pParentObj->Get_Transform()->Get_State(CTransform::STATE_POSITION);
	vParentPos = XMVectorSetZ(vParentPos, (XMVectorGetZ(vParentPos) + 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vParentPos);

	if (m_isActive)
	{
		m_bActiveChild = true;

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

void CPlayer_ItemGet::Late_Update(_float fTimeDelta)
{
	if (m_isActive)
	{
		for (auto& iter : m_Child_List)
			iter->Late_Update(fTimeDelta);
	}
}

HRESULT CPlayer_ItemGet::Render()
{
	if (m_isActive)
	{
		for (auto& iter : m_Child_List)
			iter->Render();
	}
	return S_OK;
}
HRESULT CPlayer_ItemGet::Ready_Child()
{
	CGameObject* pGameObj = m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Glow_Effect"));
	C2DEffects::EFFECT_DESC pDesc{};
	pDesc.iLevelIndex = m_iLevelIndex;
	pDesc.pParent = this;
	pDesc.iEffectType = m_iEffectType;

	if (pGameObj != nullptr)
	{
		pDesc.fColor = { 1.f, 1.f, 0.8f, 1.f };
		pDesc.vScale = { 2.5f,2.5f,0.1f };
		CGameObject* m_pEffect = dynamic_cast<CGameObject*>(pGameObj->Clone(&pDesc));
		m_Child_List.push_back(m_pEffect);
	}

	pGameObj = m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Flash_Effect"));
	if (pGameObj != nullptr)
	{
		CFlash_Effect::FLASH_DESC _Desc{};
		_Desc.iEffectType = m_iEffectType;
		_Desc.fColor = { 1.f,1.f,0.8f,1.f };
		_Desc.pParent = this;
		_Desc.iLevelIndex = m_iLevelIndex;
		_Desc.iTextureNum = 1;
		_Desc.vScale = { 2.7f,2.7f,3.f };
		
		CGameObject* p2DEffect = dynamic_cast<CGameObject*>(pGameObj->Clone(&_Desc));
		m_Child_List.push_back(p2DEffect);
	}
	
	pGameObj = m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Ripple_Effect"));
	if (pGameObj != nullptr)
	{
		CRipple_Effect::RIPPLE_DESC _Desc{};
		_Desc.iEffectType = m_iEffectType;
		_Desc.fColor = { 1.f,1.f,0.8f,1.f };
		_Desc.pParent = this;
		_Desc.iLevelIndex = m_iLevelIndex;
		_Desc.iTextureNum = 0;
		_Desc.vScale = { 1.f,1.f,1.f };
		
		CGameObject* p2DEffect = dynamic_cast<CGameObject*>(pGameObj->Clone(&_Desc));
		m_Child_List.push_back(p2DEffect);
	}

	pGameObj = m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Halo_Effect"));
	if (pGameObj != nullptr)
	{
		C3D_Effects::MODEL_EFFECT_DESC _Desc{};
		_Desc.iEffectType = m_iEffectType;
		_Desc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
		_Desc.vScale = { 3.f, 3.f, 0.1f };
		_Desc.iLevelIndex = m_iLevelIndex;
		CGameObject* p3DEffect = dynamic_cast<CGameObject*>(pGameObj->Clone(&_Desc));
		m_Child_List.push_back(p3DEffect);
	}

	pGameObj = m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Particle_Image"));
	if (pGameObj != nullptr)
	{
		CParticle_Image::IMAGE_PARTICLE_DESC pImageDesc = {};
		pImageDesc.iParticleType = CParticle_Image::PLAYER_ITEM_GET;
		pImageDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
		CGameObject* m_pEffect = dynamic_cast<CGameObject*>(pGameObj->Clone(&pImageDesc));
		m_Child_List.push_back(m_pEffect);
	}

	return S_OK;
}
CPlayer_ItemGet* CPlayer_ItemGet::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPlayer_ItemGet* pInstance = new CPlayer_ItemGet(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CPlayer_ItemGet"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPlayer_ItemGet::Clone(void* pArg)
{
	CPlayer_ItemGet* pInstance = new CPlayer_ItemGet(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CPlayer_ItemGet"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CPlayer_ItemGet::Free()
{
	__super::Free();

}
