#include "stdafx.h"
#include "MonsterDied_Effect.h"
#include "GameInstance.h"
#include "Particle_Image.h"

CMonsterDied_Effect::CMonsterDied_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:C2DEffects(pDevice, pContext)
{
}


CMonsterDied_Effect::CMonsterDied_Effect(const CMonsterDied_Effect& Prototype)
	:C2DEffects(Prototype)
{
}


HRESULT CMonsterDied_Effect::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMonsterDied_Effect::Initialize(void* pArg)
{
	/* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Child()))
		return E_FAIL;

	m_isActive = false;

	return S_OK;
}

void CMonsterDied_Effect::Priority_Update(_float fTimeDelta)
{
	if (m_isActive)
	{
		for (auto& iter : m_Child_List)
			iter->SetActive(true);

		for (auto& iter : m_Child_List)
			iter->Priority_Update(fTimeDelta);
	}
}

void CMonsterDied_Effect::Update(_float fTimeDelta)
{
	_vector vParentPos = m_pParentObj->Get_Transform()->Get_State(CTransform::STATE_POSITION);
	vParentPos = XMVectorSetZ(vParentPos, (XMVectorGetZ(vParentPos) + 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vParentPos);

	if (m_isActive)
	{
		for (auto& iter : m_Child_List)
			iter->Update(fTimeDelta);

		m_fActiveTimer += fTimeDelta;

		if (m_fActiveTimer > 2.f)
			m_isActive = false;
	}
}

void CMonsterDied_Effect::Late_Update(_float fTimeDelta)
{
	if (m_isActive)
	{
		for (auto& iter : m_Child_List)
			iter->Late_Update(fTimeDelta);
	}
}

HRESULT CMonsterDied_Effect::Render()
{
	return S_OK;
}
HRESULT CMonsterDied_Effect::Ready_Child()
{
	CGameObject* pGameObj = m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Glow_Effect"));
	C2DEffects::EFFECT_DESC pDesc{};
	pDesc.iLevelIndex = m_iLevelIndex;
	pDesc.pParent = this;
	pDesc.iEffectType = m_iEffectType;

	if (pGameObj != nullptr)
	{
		pDesc.fColor = { 0.f, 0.f, 0.f, 2.f };
		pDesc.vScale = {0.1f,0.1f,0.1f};
		CGameObject* m_pEffect = dynamic_cast<CGameObject*>(pGameObj->Clone(&pDesc));
		m_Child_List.push_back(m_pEffect);
	}

	pGameObj = m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Cross_Effect"));
	if (pGameObj != nullptr)
	{
		pDesc.fColor = { 0.78f, 0.f, 0.78f, 1.f };
		CGameObject* m_pEffect = dynamic_cast<CGameObject*>(pGameObj->Clone(&pDesc));
		m_Child_List.push_back(m_pEffect);
	}


	pGameObj = m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Smoke_Effect"));
	if (pGameObj != nullptr)
	{
		pDesc.fColor = { 0.78f, 0.f, 0.78f, 1.f };
		pDesc.vScale = { 3.f,3.f,1.f };
		CGameObject* m_pEffect = dynamic_cast<CGameObject*>(pGameObj->Clone(&pDesc));
		m_Child_List.push_back(m_pEffect);

	}


	pGameObj = m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Particle_Image"));
	if (pGameObj != nullptr)
	{
		CParticle_Image::IMAGE_PARTICLE_DESC pImageDesc = {};
		pImageDesc.iParticleType = CParticle_Image::CROSS_MINI;
		pImageDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
		CGameObject* m_pEffect = dynamic_cast<CGameObject*>(pGameObj->Clone(&pImageDesc));
		m_Child_List.push_back(m_pEffect);
	}

	return S_OK;
}
CMonsterDied_Effect* CMonsterDied_Effect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMonsterDied_Effect* pInstance = new CMonsterDied_Effect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CMonsterDied_Effect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMonsterDied_Effect::Clone(void* pArg)
{
	CMonsterDied_Effect* pInstance = new CMonsterDied_Effect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CMonsterDied_Effect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CMonsterDied_Effect::Free()
{
	__super::Free();

	Safe_Release(m_pParticle);
}
