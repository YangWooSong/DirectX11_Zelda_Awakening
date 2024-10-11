#include "stdafx.h"
#include "Spark_Effect.h"
#include "GameInstance.h"
#include "Particle_Model.h"
#include "3D_Effects.h"

CSpark_Effect::CSpark_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:C2DEffects(pDevice, pContext)
{
}


CSpark_Effect::CSpark_Effect(const CSpark_Effect& Prototype)
	:C2DEffects(Prototype)
{
}


HRESULT CSpark_Effect::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSpark_Effect::Initialize(void* pArg)
{
	/* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Child()))
		return E_FAIL;

	m_isActive = false;

	return S_OK;
}

void CSpark_Effect::Priority_Update(_float fTimeDelta)
{
	if (m_isActive)
	{
		for (auto& iter : m_Child_List)
			iter->SetActive(true);

		for (auto& iter : m_Child_List)
			iter->Priority_Update(fTimeDelta);
	}
}

void CSpark_Effect::Update(_float fTimeDelta)
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

void CSpark_Effect::Late_Update(_float fTimeDelta)
{
	if (m_isActive)
	{
		for (auto& iter : m_Child_List)
			iter->Late_Update(fTimeDelta);
	}
}

HRESULT CSpark_Effect::Render()
{
	return S_OK;
}
HRESULT CSpark_Effect::Ready_Child()
{
	CGameObject* pGameObj = m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_CubeHalfDome_Effect"));
	if (pGameObj != nullptr)
	{
		C3D_Effects::MODEL_EFFECT_DESC _Desc{};
		_Desc.iEffectType = m_iEffectType;
		_Desc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
		_Desc.vScale = { 0.25f, 0.25f, 0.1f };
		_Desc.iLevelIndex = m_iLevelIndex;
		CGameObject* p3DEffect = dynamic_cast<CGameObject*>(pGameObj->Clone(&_Desc));
		m_Child_List.push_back(p3DEffect);
	}

	return S_OK;
}
CSpark_Effect* CSpark_Effect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSpark_Effect* pInstance = new CSpark_Effect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CSpark_Effect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSpark_Effect::Clone(void* pArg)
{
	CSpark_Effect* pInstance = new CSpark_Effect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CSpark_Effect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CSpark_Effect::Free()
{
	__super::Free();

}
