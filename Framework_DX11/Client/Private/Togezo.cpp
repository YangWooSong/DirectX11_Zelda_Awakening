#include "stdafx.h"
#include "Togezo.h"
#include "GameInstance.h"
#include "Shader.h"
#include "Model.h"
#include "State_Togezo_Idle.h"
#include "State_Togezo_Run.h"
#include "State_Togezo_Discover.h"
#include "State_Togezo_Rebound.h"
#include "State_Togezo_Damage.h"
#include "Detector.h"
#include "3D_Effects.h"
#include "2DEffects.h"

CTogezo::CTogezo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CMonster{ pDevice, pContext }
{
}

CTogezo::CTogezo(const CTogezo& Prototype)
    : CMonster{ Prototype }
{
}

HRESULT CTogezo::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CTogezo::Initialize(void* pArg)
{
	/* 직교투영을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_PartObjects()))
		return E_FAIL;

	if (FAILED(Ready_State()))
		return E_FAIL;

	//Read한 정보 세팅
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&pDesc->vPosition));
	//m_pTransformCom->Set_Scaled(pDesc->vScale.x, pDesc->vScale.y, pDesc->vScale.z);
	//m_pTransformCom->RotationThreeAxis(pDesc->vRotation);
	//m_vRot = pDesc->vRotation;

	//m_pModelCom->SetUp_Animation(30, true);
	_float3 vRot = { 0.f, 180.f, 0.f };
	m_pTransformCom->RotationThreeAxis(vRot);

	m_pFsmCom->Set_State(IDLE);
	m_eObjType = CGameObject::ANIM_MONSTER;

	m_iHp = 2;

	m_isActive = false;
	return S_OK;
}


void CTogezo::Priority_Update(_float fTimeDelta)
{
	if(m_isActive)
	{
		__super::Priority_Update(fTimeDelta);
		m_p3D_Effect->Priority_Update(fTimeDelta);
	}

	m_pEffect->Priority_Update(fTimeDelta);
}

void CTogezo::Update(_float fTimeDelta)
{
	if (m_isActive)
	{
		m_pFsmCom->Update(fTimeDelta);
		m_pModelCom->Play_Animation(fTimeDelta);
		m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_Ptr());
		m_pMonsterSoundCom->Update(fTimeDelta);

		if (m_bBodyRed)
		{
			m_fRedTimer += fTimeDelta;
		}

		if (m_fRedTimer > 0.3f)
		{
			m_fRedTimer = 0.f;
			m_bBodyRed = false;
		}

		
		__super::Update(fTimeDelta);
		m_p3D_Effect->Update(fTimeDelta);
	}

	if (m_isDead)
	{
		m_pColliderCom->Set_IsActive(false);
		m_pEffect->SetActive(true);
		for (auto& pPartObject : m_Parts)
			static_cast<CDetector*>(pPartObject)->Set_Active_Collider(false);
	}

	if (m_bActiveEffect)
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

	m_pEffect->Update(fTimeDelta);
}


void CTogezo::Late_Update(_float fTimeDelta)
{
	if (m_isActive)
	{
		__super::Late_Update(fTimeDelta);
		m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
		m_pGameInstance->Add_ColliderList(m_pColliderCom);
		m_p3D_Effect->Late_Update(fTimeDelta);
#ifdef _DEBUG
		m_pGameInstance->Add_DebugObject(m_pColliderCom);
#endif
	}
	m_pEffect->Late_Update(fTimeDelta);
}

HRESULT CTogezo::Render()
{
	if (m_isActive)
	{
		if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_bIsRed", &m_bBodyRed, sizeof(_bool))))
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

void CTogezo::OnCollisionEnter(CGameObject* pOther)
{
	if (m_pColliderCom->Get_IsColl())
	{
		if (pOther->Get_LayerTag() == TEXT("Layer_Shield") )
		{
			m_bActiveEffect = true;
			Change_State(REBOUND);
		}

		if (m_pFsmCom->Get_CurrentState() == REBOUND)
		{
			if (pOther->Get_LayerTag() == TEXT("Layer_Sword"))
			{
				m_bBodyRed = true;
				Change_State(DAMAGE);
			}
		}
	}
}

void CTogezo::OnCollisionStay(CGameObject* pOther)
{
}

void CTogezo::OnCollisionExit(CGameObject* pOther)
{
}

HRESULT CTogezo::Ready_Components()
{
	__super::Ready_Components();

	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_DUNGEON, TEXT("Prototype_Component_Model_Togezo"),
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
	ColliderDesc.vExtents = _float3(0.5f, 0.5f, 0.5f);
	ColliderDesc.vCenter = _float3(0.f, 0.3f, 0.f);

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
		_Desc.iEffectType = VEAGAS_HIT_EFFECT;
		_Desc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
		_Desc.vScale = { 0.3f,0.3f,0.3f };
		_Desc.iLevelIndex = LEVEL_DUNGEON;
		m_p3D_Effect = dynamic_cast<CGameObject*>(pGameObj->Clone(&_Desc));
	}

	return S_OK;
}

HRESULT CTogezo::Ready_State()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Fsm"),
		TEXT("Com_Fsm"), reinterpret_cast<CComponent**>(&m_pFsmCom), this)))
		return E_FAIL;

	m_pFsmCom->Add_State(CState_Togezo_Idle::Create(m_pFsmCom, this, IDLE));
	m_pFsmCom->Add_State(CState_Togezo_Run::Create(m_pFsmCom, this, RUN));
	m_pFsmCom->Add_State(CState_Togezo_Rebound::Create(m_pFsmCom, this, REBOUND));
	m_pFsmCom->Add_State(CState_Togezo_Discover::Create(m_pFsmCom, this, DISCOVER));
	m_pFsmCom->Add_State(CState_Togezo_Damage::Create(m_pFsmCom, this, DAMAGE));
	return S_OK;
}

HRESULT CTogezo::Ready_PartObjects()
{
	m_Parts.resize(PART_END);

	CDetector::DETECTOR_DESC DetectDesc{};
	DetectDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
	DetectDesc.pDetect = &m_bHoriDetect;
	DetectDesc.vOffset = _float3(0.f, 0.f, 0.f);
	DetectDesc.vSize = _float3(8.f, 0.5f, 0.3f);
	DetectDesc.pTransform = m_pTransformCom;
	DetectDesc.onlyPos = true;
	if (FAILED(__super::Add_PartObject(PART_DETECTOR_HORIZON, TEXT("Prototype_GameObject_Detector"), &DetectDesc)))
		return E_FAIL;

	DetectDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
	DetectDesc.pDetect = &m_bVerDetect;
	DetectDesc.vOffset = _float3(0.f, 0.f, 0.f);
	DetectDesc.vSize = _float3(0.3f, 0.5f, 8.f);
	DetectDesc.pTransform = m_pTransformCom;
	DetectDesc.onlyPos = true;
	if (FAILED(__super::Add_PartObject(PART_DETECTOR_VERTICAL, TEXT("Prototype_GameObject_Detector"), &DetectDesc)))
		return E_FAIL;


	return S_OK;
}

CTogezo* CTogezo::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTogezo* pInstance = new CTogezo(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CTogezo"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CTogezo::Clone(void* pArg)
{
	CTogezo* pInstance = new CTogezo(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CTogezo"));
		Safe_Release(pInstance);
	}

	return pInstance;
}
void CTogezo::Free()
{
	__super::Free();

	if (nullptr != m_pFsmCom)
		m_pFsmCom->Release_States();

	Safe_Release(m_pFsmCom);
	Safe_Release(m_pMonsterSoundCom);
}


