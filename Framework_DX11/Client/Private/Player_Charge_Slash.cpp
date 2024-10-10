#include "stdafx.h"
#include "Player_Charge_Slash.h"
#include "GameInstance.h"
#include "Particle_Image.h"
#include "Flash_Effect.h"
#include "Halo_Effect.h"
CPlayer_Charge_Slash::CPlayer_Charge_Slash(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:C2DEffects(pDevice, pContext)
{
}


CPlayer_Charge_Slash::CPlayer_Charge_Slash(const CPlayer_Charge_Slash& Prototype)
	:C2DEffects(Prototype)
{
}


HRESULT CPlayer_Charge_Slash::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayer_Charge_Slash::Initialize(void* pArg)
{
	/* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Child()))
		return E_FAIL;

	m_isActive = false;

	return S_OK;
}

void CPlayer_Charge_Slash::Priority_Update(_float fTimeDelta)
{
	if (m_isActive)
	{
		for (auto& iter : m_Child_List)
			iter->SetActive(true);

		for (auto& iter : m_Child_List)
			iter->Priority_Update(fTimeDelta);
	}
}

void CPlayer_Charge_Slash::Update(_float fTimeDelta)
{
	_vector vParentPos = m_pParentObj->Get_Transform()->Get_State(CTransform::STATE_POSITION);
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

void CPlayer_Charge_Slash::Late_Update(_float fTimeDelta)
{
	if (m_isActive)
	{
		for (auto& iter : m_Child_List)
			iter->Late_Update(fTimeDelta);
	}
}

HRESULT CPlayer_Charge_Slash::Render()
{
	if (m_isActive)
	{
		for (auto& iter : m_Child_List)
			iter->Render();
	}
	return S_OK;
}
HRESULT CPlayer_Charge_Slash::Ready_Child()
{
	CGameObject* pGameObj = { nullptr };

	//pGameObj = m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Flash_Effect"));
	//if (pGameObj != nullptr)
	//{
	//	CFlash_Effect::FLASH_DESC _Desc{};
	//	_Desc.iEffectType = m_iEffectType;
	//	_Desc.fColor = { 1.f,1.f,0.8f,1.f };
	//	_Desc.pParent = this;
	//	_Desc.iLevelIndex = m_iLevelIndex;
	//	_Desc.iTextureNum = 1;
	//	_Desc.vScale = { 2.7f,2.7f,3.f };

	//	CGameObject* p2DEffect = dynamic_cast<CGameObject*>(pGameObj->Clone(&_Desc));
	//	m_Child_List.push_back(p2DEffect);
	//}

	pGameObj = m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Halo_Effect"));
	if (pGameObj != nullptr)
	{
		C3D_Effects::MODEL_EFFECT_DESC _Desc{};
		_Desc.iEffectType = m_iEffectType;
		_Desc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
		_Desc.vScale = { 10.f, 10.f, 0.1f };
		_Desc.iLevelIndex = m_iLevelIndex;
		CGameObject* p3DEffect = dynamic_cast<CGameObject*>(pGameObj->Clone(&_Desc));
		m_Child_List.push_back(p3DEffect);
	}

	return S_OK;
}

CPlayer_Charge_Slash* CPlayer_Charge_Slash::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPlayer_Charge_Slash* pInstance = new CPlayer_Charge_Slash(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CPlayer_Charge_Slash"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPlayer_Charge_Slash::Clone(void* pArg)
{
	CPlayer_Charge_Slash* pInstance = new CPlayer_Charge_Slash(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CPlayer_Charge_Slash"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CPlayer_Charge_Slash::Free()
{
	__super::Free();

}
