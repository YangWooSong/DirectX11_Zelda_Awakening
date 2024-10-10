#include "stdafx.h"
#include "Octorok.h"
#include "GameInstance.h"
#include "Shader.h"
#include "Model.h"
#include "State_Octorok_Idle.h"
#include "State_Octorok_Dead.h"
#include "State_Octorok_Attack.h"
#include "State_Octorok_Walk.h"
#include "OctorokRock.h"
#include "Detector.h"
#include "2DEffects.h"
#include "3D_Effects.h"
#include "Client_Defines.h"

COctorok::COctorok(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMonster{ pDevice, pContext }
{
}

COctorok::COctorok(const COctorok& Prototype)
	: CMonster{ Prototype }
{
}
HRESULT COctorok::Initialize_Prototype()
{
	return S_OK;
}

HRESULT COctorok::Initialize(void* pArg)
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

	//m_pModelCom->SetUp_Animation(30, true);
	m_pFsmCom->Set_State(IDLE);
	
	m_iDir = (int)m_pGameInstance->Get_Random(0, 4);
	m_pTransformCom->RotationThreeAxis(_float3(0.f, 180.f, 0.f));

	m_eObjType = CGameObject::ANIM_MONSTER;
	return S_OK;
}

void COctorok::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
	m_pEffect->Priority_Update(fTimeDelta);
	m_p3D_Effect->Priority_Update(fTimeDelta);
}

void COctorok::Update(_float fTimeDelta)
{
	if (m_isDead)
	{
		m_bRender = false;
		m_bBodyRed = false;
		m_pColliderCom->Set_IsActive(false);
		m_pEffect->SetActive(true);
	}
	else
	{
		if (m_bDetect == true && m_pFsmCom->Get_PrevState() != ATTACK)
			m_pFsmCom->Change_State(ATTACK);
		else if(m_bDetect == false && m_pFsmCom->Get_PrevState() == ATTACK)
			m_pFsmCom->Change_State(IDLE);

		if(m_bRender == false)
		{
			m_pColliderCom->Set_IsActive(true);
			m_bRender = true;
		}

		if (m_pNavigationCom != nullptr)
			m_pNavigationCom->SetUp_OnCell(m_pTransformCom, 0.f, fTimeDelta);

		m_pFsmCom->Update(fTimeDelta);
		m_pModelCom->Play_Animation(fTimeDelta);

		__super::Update(fTimeDelta);

		m_pMonsterSoundCom->Update(fTimeDelta);

		m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_Ptr());
	}

	m_pEffect->Update(fTimeDelta);
	m_p3D_Effect->Update(fTimeDelta);
}

void COctorok::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	m_pGameInstance->Add_ColliderList(m_pColliderCom);
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
	m_pEffect->Late_Update(fTimeDelta);
	m_p3D_Effect->Late_Update(fTimeDelta);
#ifdef _DEBUG
	m_pGameInstance->Add_DebugObject(m_pColliderCom);
#endif
}

HRESULT COctorok::Render()
{
	if(m_bRender)
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

void COctorok::OnCollisionEnter(CGameObject* pOther)
{
	if (m_pColliderCom->Get_IsColl())
	{
		if (pOther->Get_LayerTag() == TEXT("Layer_Sword"))
		{
			Change_State(DEAD);
			//m_p3D_Effect->Get_Transform()->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
			m_p3D_Effect->SetActive(true);
		}
	}
}

void COctorok::OnCollisionStay(CGameObject* pOther)
{
	if (m_pColliderCom->Get_IsColl())
	{
		if (pOther->Get_LayerTag() == TEXT("Layer_Monster"))
		{
			_vector newLook = pOther->Get_Transform()->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			newLook = XMVector3Normalize(newLook);

			_vector newBodyPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + -newLook * 0.02f;
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, newBodyPos);
		}
	}
}

void COctorok::OnCollisionExit(CGameObject* pOther)
{
}


HRESULT COctorok::Ready_Components()
{
	__super::Ready_Components();

	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_FIELD, TEXT("Prototype_Component_Model_Octorok"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;


	/* For.Com_Navigation */
	CNavigation::NAVIGATION_DESC			NaviDesc{};

	NaviDesc.iCurrentIndex = m_iCellNum;
	NaviDesc.iOwnerType = CNavigation::NONPLAYER;

	if (FAILED(__super::Add_Component(LEVEL_FIELD, TEXT("Prototype_Component_Navigation"),
		TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &NaviDesc)))
		return E_FAIL;
	
	/* For.Com_Collider */
	CBounding_AABB::BOUNDING_AABB_DESC			ColliderDesc{};
	ColliderDesc.vExtents = _float3(0.6f, 0.7f, 0.6f);
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vExtents.y, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider0"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;
	m_pColliderCom->Set_Owner(this);

	/* FOR.Com_Sound */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sound"),
		TEXT("Com_Sound"), reinterpret_cast<CComponent**>(&m_pMonsterSoundCom))))
		return E_FAIL;
	m_pMonsterSoundCom->Set_Owner(this);

	return S_OK;
}

HRESULT COctorok::Ready_PartObjects()
{
	/* 실제 추가하고 싶은 파트오브젝트의 갯수만큼 밸류를 셋팅해놓자. */
	m_Parts.resize(PART_END);

	COctorokRock::OCTOROKROCK_DESC		RockDesc{};
	RockDesc.iDir = m_iDir;
	RockDesc.iCellNum = m_pNavigationCom->Get_CurrentCellIndex();
	RockDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
	RockDesc.pParent = this;

	if (FAILED(__super::Add_PartObject(PART_BULLET, TEXT("Prototype_GameObject_OctorokRock"), &RockDesc)))
		return E_FAIL;

	CDetector::DETECTOR_DESC DetectDesc{};
	DetectDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
	DetectDesc.pDetect = &m_bDetect;
	DetectDesc.vOffset = _float3(0.f, 0.f, 3.5f);
	DetectDesc.vSize = _float3(1.5f, 0.5f, 3.f);
	if (FAILED(__super::Add_PartObject(PART_DETECTOR, TEXT("Prototype_GameObject_Detector"), &DetectDesc)))
		return E_FAIL;

	CGameObject* pGameObj = m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_MonsterDied_Effect"));

	if (pGameObj != nullptr)
	{
		C2DEffects::EFFECT_DESC Desc{};
		Desc.iLevelIndex = LEVEL_FIELD;
		Desc.pParent = this;
		Desc.iEffectType = MONSTER_DIED_EFFECT;
		CGameObject* pEffect = dynamic_cast<CGameObject*>(pGameObj->Clone(&Desc));
		m_pEffect = pEffect;
	}
	
	pGameObj = m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_3D_Effects"));

	if (pGameObj != nullptr)
	{
		C3D_Effects::MODEL_EFFECT_DESC _Desc{};
		_Desc.iEffectType = MONSTER_HIT_EFFECT;
		_Desc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
		CGameObject* p3DEffect = dynamic_cast<CGameObject*>(pGameObj->Clone(&_Desc));
		m_p3D_Effect = p3DEffect;
	}
	return S_OK;
}

HRESULT COctorok::Ready_State()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Fsm"),
		TEXT("Com_Fsm"), reinterpret_cast<CComponent**>(&m_pFsmCom), this)))
		return E_FAIL;

	m_pFsmCom->Add_State(CState_Octorok_Idle::Create(m_pFsmCom, this, IDLE));
	m_pFsmCom->Add_State(CState_Octorok_Walk::Create(m_pFsmCom, this, WALK));
	m_pFsmCom->Add_State(CState_Octorok_Attack::Create(m_pFsmCom, this, ATTACK));
	m_pFsmCom->Add_State(CState_Octorok_Dead::Create(m_pFsmCom, this, DEAD));

	return S_OK;
}

COctorok* COctorok::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	COctorok* pInstance = new COctorok(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : COctorok"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* COctorok::Clone(void* pArg)
{
	COctorok* pInstance = new COctorok(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : COctorok"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void COctorok::Free()
{
	__super::Free();

	if (nullptr != m_pFsmCom)
		m_pFsmCom->Release_States();
	Safe_Release(m_pFsmCom);
	Safe_Release(m_pMonsterSoundCom);
}

