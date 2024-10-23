#include "stdafx.h"
#include "Rola.h"
#include "GameInstance.h"
#include "Shader.h"
#include "Model.h"
#include "State_Rola_Idle.h"
#include "State_Rola_Jump.h"
#include "State_Rola_Push.h"
#include "State_Rola_Demage.h"
#include "State_Rola_Dead.h"
#include "2DEffects.h"
#include "3D_Effects.h"
#include "Rola_Hand_Effect.h"
CRola::CRola(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CMonster{ pDevice, pContext }
{
}

CRola::CRola(const CRola& Prototype)
    : CMonster{ Prototype }
{
}

HRESULT CRola::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CRola::Initialize(void* pArg)
{
	/* 직교투영을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_State()))
		return E_FAIL;

	if (FAILED(Ready_Part_Effect()))
		return E_FAIL;

	MONSTER_DESC* pDesc = static_cast<MONSTER_DESC*>(pArg);
	_float3 vRot = { 0.f, -90.f, 0.f };
	m_pTransformCom->RotationThreeAxis(vRot);

	//m_pModelCom->SetUp_Animation(30, true);
	m_pFsmCom->Set_State(IDLE);
	m_eObjType = CGameObject::ANIM_MONSTER;

	m_isActive = false;

	m_iHp = 8;

	return S_OK;
}

void CRola::Priority_Update(_float fTimeDelta)
{
	if (m_isActive)
	{
		__super::Priority_Update(fTimeDelta);
		m_p3D_Effect->Priority_Update(fTimeDelta);

		for (auto& pPartObject : m_Parts)
			pPartObject->Priority_Update(fTimeDelta);

	}
	m_pEffect->Priority_Update(fTimeDelta);

	if (KEY_TAP(KEY::U))
		Change_State(DEAD);
}

void CRola::Update(_float fTimeDelta)
{
	if (m_isActive)
	{
		m_pFsmCom->Update(fTimeDelta);
		m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_Ptr());
		m_pMonsterSoundCom->Update(fTimeDelta);
		m_pModelCom->Play_Animation(fTimeDelta);

		
		//맞으면 두번 깜빡임
		if (m_bBlink == false)
		{
			if (m_iRedCount != 0)
			{
				if (m_fNonRedTimer > 0.2f)
				{
					m_fNonRedTimer = 0.f;
					m_iRedCount--;
					m_bBlink = true;
				}
			}

			if (m_iRedCount == 0)
			{
				m_iRedCount = 0;
				m_bBlink = false;
			}

			m_fNonRedTimer += fTimeDelta;
		}

		if (m_bBlink)
		{
			m_fTimer += fTimeDelta;

			if (m_fTimer > 0.25f)
			{
				m_fTimer = 0.f;
				m_bBlink = false;
				m_fNonRedTimer = 0.f;
			}
		}

		if (m_pFsmCom->Get_CurrentState() == DEMAGE)
			m_pColliderCom->Set_IsActive(false);
		else
			m_pColliderCom->Set_IsActive(true);
		__super::Update(fTimeDelta);


	
	}

	if (m_bActiveEffect )
	{
		if (m_fEffectTimer == 0.f)
		{
			m_p3D_Effect->SetActive(true);
		}
		else if (m_fEffectTimer > 0.2f)
		{
			m_p3D_Effect->SetActive(false);
			m_bActiveEffect = false;
			m_fEffectTimer = 0.f;
			return;
		}
		m_fEffectTimer += fTimeDelta;
	}
	m_p3D_Effect->Update(fTimeDelta);
	m_pEffect->Update(fTimeDelta);

	for (auto& pPartObject : m_Parts)
		pPartObject->Update(fTimeDelta);
}

void CRola::Late_Update(_float fTimeDelta)
{
	if (m_isActive)
	{
		__super::Late_Update(fTimeDelta);
		m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
		m_pGameInstance->Add_RenderObject(CRenderer::RG_SHADOWOBJ, this);
		m_pGameInstance->Add_ColliderList(m_pColliderCom);

#ifdef _DEBUG
		m_pGameInstance->Add_DebugObject(m_pColliderCom);
#endif

	
	}
	m_pEffect->Late_Update(fTimeDelta);
	m_p3D_Effect->Late_Update(fTimeDelta);

	for (auto& pPartObject : m_Parts)
		pPartObject->Late_Update(fTimeDelta);
}

HRESULT CRola::Render()
{
	if (m_isActive)
	{
		if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_bIsRed", &m_bBlink, sizeof(_bool))))
			return E_FAIL;

		_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

		for (size_t i = 0; i < iNumMeshes; i++)
		{
			m_pModelCom->Bind_MeshBoneMatrices(m_pShaderCom, "g_BoneMatrices", (_uint)i);

			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", TEXTURE_TYPE::DIFFUSE, (_uint)i)))
				return E_FAIL;


			if (FAILED(m_pShaderCom->Begin(1)))
				return E_FAIL;

			if (FAILED(m_pModelCom->Render((_uint)i)))
				return E_FAIL;
		}

		_bool bFalse = { false };
		if (FAILED(m_pShaderCom->Bind_RawValue("g_bIsRed", &bFalse, sizeof(_bool))))
			return E_FAIL;
	}

	return S_OK;
}

void CRola::OnCollisionEnter(CGameObject* pOther)
{
	if (m_pColliderCom->Get_IsColl())
	{
		if (pOther->Get_LayerTag() == TEXT("Layer_Sword") )
		{
			m_bActiveEffect = true;
			if(m_iHp > 0)
			{
				Change_State(DEMAGE);
			}
		}
	}
}

void CRola::OnCollisionStay(CGameObject* pOther)
{
}

void CRola::OnCollisionExit(CGameObject* pOther)
{
}


HRESULT CRola::Ready_Components()
{
	__super::Ready_Components();

	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_DUNGEON, TEXT("Prototype_Component_Model_Rola"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Navigation */
	CNavigation::NAVIGATION_DESC			NaviDesc{};

	NaviDesc.iCurrentIndex = m_iCellNum;
	NaviDesc.iOwnerType = CNavigation::NONPLAYER;

	if (FAILED(__super::Add_Component(LEVEL_DUNGEON, TEXT("Prototype_Component_Navigation"),
		TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &NaviDesc)))
		return E_FAIL;

	/* For.Com_Collider */
	CBounding_AABB::BOUNDING_AABB_DESC			ColliderDesc{};
	ColliderDesc.vExtents = _float3(1.f, 1.f, 1.f);
	ColliderDesc.vCenter = _float3(0.f, 0.5f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;
	m_pColliderCom->Set_Owner(this);

	/* FOR.Com_Sound */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sound"),
		TEXT("Com_Sound"), reinterpret_cast<CComponent**>(&m_pMonsterSoundCom))))
		return E_FAIL;
	m_pMonsterSoundCom->Set_Owner(this);


	CGameObject* pGameObj = m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_MonsterDied_Effect"));

	if (pGameObj != nullptr)
	{
		C2DEffects::EFFECT_DESC Desc{};
		Desc.iLevelIndex = LEVEL_DUNGEON;
		Desc.pParent = this;
		Desc.iEffectType = MONSTER_DIED_EFFECT;
		CGameObject* pEffect = dynamic_cast<CGameObject*>(pGameObj->Clone(&Desc));
		m_pEffect = pEffect;
	}
	pGameObj = m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_3D_Effects"));

	if (pGameObj != nullptr)
	{
		C3D_Effects::MODEL_EFFECT_DESC _Desc{};
		_Desc.iEffectType = ROLA_HIT_EFFECT;
		_Desc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
		_Desc.vScale = { 0.8f,0.8f,0.8f };
		_Desc.iLevelIndex = LEVEL_DUNGEON;
		CGameObject* p3DEffect = dynamic_cast<CGameObject*>(pGameObj->Clone(&_Desc));
		m_p3D_Effect = p3DEffect;
	}
	return S_OK;
}

HRESULT CRola::Ready_State()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Fsm"),
		TEXT("Com_Fsm"), reinterpret_cast<CComponent**>(&m_pFsmCom), this)))
		return E_FAIL;

	m_pFsmCom->Add_State(CState_Rola_Idle::Create(m_pFsmCom, this, IDLE));
	m_pFsmCom->Add_State(CState_Rola_Jump::Create(m_pFsmCom, this, JUMP));
	m_pFsmCom->Add_State(CState_Rola_Push::Create(m_pFsmCom, this, PUSH));
	m_pFsmCom->Add_State(CState_Rola_Demage::Create(m_pFsmCom, this, DEMAGE));
	m_pFsmCom->Add_State(CState_Rola_Dead::Create(m_pFsmCom, this, DEAD));
	return S_OK;
}

HRESULT CRola::Ready_Part_Effect()
{
	m_Parts.resize(PART_END);

	CRola_Hand_Effect::ROLA_HAND_DESC		Desc{};
	Desc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
	Desc.pSocketBoneMatrix = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr("attach_L");
	Desc.m_pParent = this;
	if (FAILED(__super::Add_PartObject(DUST, TEXT("Prototype_GameObject_Rola_Hand_Effect"), &Desc)))
		return E_FAIL;
 
	return S_OK;
}

CRola* CRola::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRola* pInstance = new CRola(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CRola"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CRola::Clone(void* pArg)
{
	CRola* pInstance = new CRola(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CRola"));
		Safe_Release(pInstance);
	}

	return pInstance;
}
void CRola::Free()
{
	__super::Free();

	if (nullptr != m_pFsmCom)
		m_pFsmCom->Release_States();

	Safe_Release(m_pFsmCom);
	Safe_Release(m_pMonsterSoundCom);
}

