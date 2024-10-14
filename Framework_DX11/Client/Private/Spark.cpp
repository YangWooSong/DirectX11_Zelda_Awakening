#include "stdafx.h"
#include "Spark.h"
#include "GameInstance.h"
#include "Shader.h"
#include "Model.h"
#include "State_Spark_Idle.h"
#include "Glow_Effect.h"

CSpark::CSpark(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CMonster{ pDevice, pContext }
{
}

CSpark::CSpark(const CSpark& Prototype)
    : CMonster{ Prototype }
{
}

HRESULT CSpark::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CSpark::Initialize(void* pArg)
{
	/* 직교투영을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Set_MonsterNum(pArg);

	if (FAILED(Ready_State()))
		return E_FAIL;

	m_pFsmCom->Set_State(IDLE);
	m_eObjType = CGameObject::ANIM_MONSTER;

	m_isActive = false;

	CGameObject* pGameObj = m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Glow_Effect"));

	if (pGameObj != nullptr)
	{
		C2DEffects::EFFECT_DESC Desc{};
		Desc.iLevelIndex = LEVEL_DUNGEON;
		Desc.pParent = this;
		Desc.iEffectType = SPARK_EFFECT;
		Desc.fColor = { 1.f, 1.f, 0.0f, 0.2f };
		Desc.vScale = { 2.f,2.f,0.1f };
		m_pEffect[0] = dynamic_cast<CGameObject*>(pGameObj->Clone(&Desc));
	}

	pGameObj = m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Lightning_Effect"));
	C2DEffects::EFFECT_DESC pDesc{};
	pDesc.iLevelIndex = LEVEL_DUNGEON;
	pDesc.pParent = this;
	pDesc.iEffectType = SPARK_EFFECT;

	if (pGameObj != nullptr)
	{
		pDesc.fColor = { 1.f, 1.f, 0.0f, 0.5f };
		pDesc.vScale = { 1.5f,1.5f,0.1f };
		m_pEffect[1] = dynamic_cast<CGameObject*>(pGameObj->Clone(&pDesc));
	}
	return S_OK;
}

void CSpark::Priority_Update(_float fTimeDelta)
{
	if (m_isActive)
	{
		m_pEffect[0]->Priority_Update(fTimeDelta);
		m_pEffect[1]->Priority_Update(fTimeDelta);
		__super::Priority_Update(fTimeDelta);
	}
}

void CSpark::Update(_float fTimeDelta)
{
	if (m_isActive)
	{
		m_pEffect[0] ->SetActive(true);
		m_pEffect[1] ->SetActive(true);
		m_pEffect[0]->Update(fTimeDelta);
		m_pEffect[1]->Update(fTimeDelta);
		m_pFsmCom->Update(fTimeDelta);
		m_pModelCom->Play_Animation(fTimeDelta);
		m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_Ptr());
		//m_pEffect->Get_Transform()->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		__super::Update(fTimeDelta);
	}
	else
	{
		m_pMonsterSoundCom->Stop();
		m_pEffect[0]->SetActive(false);
		m_pEffect[1]->SetActive(false);
	}
}

void CSpark::Late_Update(_float fTimeDelta)
{
	if (m_isActive)
	{
		m_pEffect[0]->Late_Update(fTimeDelta);
		m_pEffect[1]->Late_Update(fTimeDelta);
		__super::Late_Update(fTimeDelta);
		m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
		m_pGameInstance->Add_ColliderList(m_pColliderCom);

#ifdef _DEBUG
		m_pGameInstance->Add_DebugObject(m_pColliderCom);
#endif
	}
}

HRESULT CSpark::Render()
{
	if (m_isActive)
	{
		if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
			return E_FAIL;

		_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

		for (size_t i = 0; i < iNumMeshes; i++)
		{
			m_pModelCom->Bind_MeshBoneMatrices(m_pShaderCom, "g_BoneMatrices", (_uint)i);

			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", TEXTURE_TYPE::DIFFUSE, (_uint)i)))
				return E_FAIL;


			if (FAILED(m_pShaderCom->Begin(0)))
				return E_FAIL;

			if (FAILED(m_pModelCom->Render((_uint)i)))
				return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CSpark::Ready_Components()
{
	__super::Ready_Components();

	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_DUNGEON, TEXT("Prototype_Component_Model_Spark"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Collider */
	CBounding_AABB::BOUNDING_AABB_DESC			ColliderDesc{};
	ColliderDesc.vExtents = _float3(0.5f, 0.5f, 0.5f);
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vExtents.y, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;
	m_pColliderCom->Set_Owner(this);


	/* FOR.Com_Sound */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sound"),
		TEXT("Com_Sound"), reinterpret_cast<CComponent**>(&m_pMonsterSoundCom))))
		return E_FAIL;
	m_pMonsterSoundCom->Set_Owner(this);

	return S_OK;
}

HRESULT CSpark::Ready_State()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Fsm"),
		TEXT("Com_Fsm"), reinterpret_cast<CComponent**>(&m_pFsmCom), this)))
		return E_FAIL;

	m_pFsmCom->Add_State(CState_Spark_Idle::Create(m_pFsmCom, this, IDLE));
	return S_OK;
}

void CSpark::Set_MonsterNum(void* pArg)
{
	MONSTER_DESC* pDesc = static_cast<MONSTER_DESC*>(pArg);

	switch (m_iRoomNum)
	{
	case 14:
	{
		{
			if (pDesc->vPosition.x < -10.f)
				m_iMonsterNum = 1;
			else if (pDesc->vPosition.x < -2.f)
				m_iMonsterNum = 2;
			else
				m_iMonsterNum = 3;
		}
	}
		break;
	case 7:
	{
		if (pDesc->vPosition.x < 0.f)
			m_iMonsterNum = 4;
		else
			m_iMonsterNum = 5;
	}
		break;
	case 8:
	{
		if (pDesc->vPosition.x < 30.f)
			m_iMonsterNum = 6;
		else
			m_iMonsterNum = 7;
	}
		break;
	default:
		break;
	}

}


CSpark* CSpark::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSpark* pInstance = new CSpark(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CSpark"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSpark::Clone(void* pArg)
{
	CSpark* pInstance = new CSpark(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CSpark"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSpark::Free()
{
	__super::Free();

	if (nullptr != m_pFsmCom)
		m_pFsmCom->Release_States();

	Safe_Release(m_pEffect[1]);
	Safe_Release(m_pEffect[0]);
	Safe_Release(m_pFsmCom);
	Safe_Release(m_pMonsterSoundCom);
}


