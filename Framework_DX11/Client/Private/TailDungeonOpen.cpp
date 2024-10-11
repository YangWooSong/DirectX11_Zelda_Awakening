#include "stdafx.h"
#include "TailDungeonOpen.h"
#include "GameInstance.h"
#include "Particle_Image.h"
#include "Flash_Effect.h"
#include "Glow_Effect.h"

CTailDungeonOpen::CTailDungeonOpen(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:C2DEffects(pDevice, pContext)
{
}


CTailDungeonOpen::CTailDungeonOpen(const CTailDungeonOpen& Prototype)
	:C2DEffects(Prototype)
{
}


HRESULT CTailDungeonOpen::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTailDungeonOpen::Initialize(void* pArg)
{
	/* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Child()))
		return E_FAIL;

	m_isActive = false;

	return S_OK;
}

void CTailDungeonOpen::Priority_Update(_float fTimeDelta)
{
	if (m_isActive)
	{
		for (auto& iter : m_Child_List)
			iter->SetActive(true);

		for (auto& iter : m_Child_List)
			iter->Priority_Update(fTimeDelta);
	}
}

void CTailDungeonOpen::Update(_float fTimeDelta)
{
	_vector vParentPos = m_pParentObj->Get_Transform()->Get_State(CTransform::STATE_POSITION);
	//vParentPos = XMVectorSetZ(vParentPos, (XMVectorGetZ(vParentPos) + 1.f));
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

void CTailDungeonOpen::Late_Update(_float fTimeDelta)
{
	if (m_isActive)
	{
		for (auto& iter : m_Child_List)
			iter->Late_Update(fTimeDelta);
	}
}

HRESULT CTailDungeonOpen::Render()
{
	if (m_isActive)
	{
		for (auto& iter : m_Child_List)
			iter->Render();
	}
	return S_OK;
}
HRESULT CTailDungeonOpen::Ready_Child()
{
	CGameObject* pGameObj = m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Glow_Effect"));
	C2DEffects::EFFECT_DESC pDesc{};
	pDesc.iLevelIndex = m_iLevelIndex;
	pDesc.pParent = this;
	pDesc.iEffectType = m_iEffectType;

	if (pGameObj != nullptr)
	{
		pDesc.fColor = { 0.7f, 0.8f, 1.f, 0.5f };
		pDesc.vScale = { 1.f,1.f,0.1f };
		CGameObject* m_pEffect = dynamic_cast<CGameObject*>(pGameObj->Clone(&pDesc));
		m_Child_List.push_back(m_pEffect);
	}

	pGameObj = m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Flash_Effect"));
	if (pGameObj != nullptr)
	{
		CFlash_Effect::FLASH_DESC _Desc{};
		_Desc.iEffectType = m_iEffectType;
		_Desc.fColor = { 0.9f,0.9f,1.f,1.f };
		_Desc.pParent = this;
		_Desc.iLevelIndex = m_iLevelIndex;
		_Desc.iTextureNum = 3;
		_Desc.vScale = { 0.7f,0.7f,0.1f };

		CGameObject* p2DEffect = dynamic_cast<CGameObject*>(pGameObj->Clone(&_Desc));
		m_Child_List.push_back(p2DEffect);
	}

	pGameObj = m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Particle_Image"));
	if (pGameObj != nullptr)
	{
		CParticle_Image::IMAGE_PARTICLE_DESC pImageDesc = {};
		pImageDesc.iParticleType = CParticle_Image::GLOW;
		pImageDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
		CGameObject* m_pEffect = dynamic_cast<CGameObject*>(pGameObj->Clone(&pImageDesc));
		m_Child_List.push_back(m_pEffect);
	}

	return S_OK;
}
CTailDungeonOpen* CTailDungeonOpen::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTailDungeonOpen* pInstance = new CTailDungeonOpen(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CTailDungeonOpen"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTailDungeonOpen::Clone(void* pArg)
{
	CTailDungeonOpen* pInstance = new CTailDungeonOpen(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CTailDungeonOpen"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CTailDungeonOpen::Free()
{
	__super::Free();

}
