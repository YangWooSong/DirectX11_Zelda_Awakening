#include "stdafx.h"
#include "Vegas.h"
#include "GameInstance.h"
#include "Shader.h"
#include "Model.h"
#include "State_Vegas_Idle.h"
#include "State_Vegas_Walk_f.h"
#include "State_Vegas_Walk_b.h"
#include "State_Vegas_Walk_l.h"
#include "State_Vegas_Walk_r.h"
#include "State_Vegas_Damage.h"
#include "State_Vegas_Dead.h"
#include "3D_Effects.h"
#include "2DEffects.h"
CVegas::CVegas(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMonster{ pDevice, pContext }
{
}


CVegas::CVegas(const CVegas& Prototype)
	: CMonster{ Prototype }
{
}

HRESULT CVegas::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CVegas::Initialize(void* pArg)
{
	/* 직교투영을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (m_iCellNum == 721)
		m_iCellNum = 719;
	if (m_iCellNum == 722)
		m_iCellNum = 720;
	if (m_iCellNum == 720)
		m_iCellNum = 718;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_State()))
		return E_FAIL;

	m_pFsmCom->Set_State(IDLE);
	m_pModelCom->Add_Texture_to_Material(TEXT("../Bin/ModelData/Anim/Monster/Vegas/MI_body_alb.1.dds"), TEXTURE_TYPE::DIFFUSE, 0);
	m_pModelCom->Add_Texture_to_Material(TEXT("../Bin/ModelData/Anim/Monster/Vegas/MI_body_alb.2.dds"), TEXTURE_TYPE::DIFFUSE, 0);
	m_pModelCom->Add_Texture_to_Material(TEXT("../Bin/ModelData/Anim/Monster/Vegas/MI_body_alb.3.dds"), TEXTURE_TYPE::DIFFUSE, 0);

	m_iTextureNum = 0;
	m_eObjType = CGameObject::ANIM_MONSTER;

	m_isActive = false;

	return S_OK;
}


void CVegas::Priority_Update(_float fTimeDelta)
{
	if (m_isActive)
	{
		__super::Priority_Update(fTimeDelta);
		m_p3D_Effect->Priority_Update(fTimeDelta);
	}
	m_pEffect->Priority_Update(fTimeDelta);
}

void CVegas::Update(_float fTimeDelta)
{
	if (m_isActive)
	{
		m_fTimer += fTimeDelta;

		if (m_pFsmCom->Get_CurrentState() != IDLE && m_pNavigationCom != nullptr)
			m_pNavigationCom->SetUp_OnCell(m_pTransformCom, 0.f, fTimeDelta);

		if (m_pFsmCom->Get_CurrentState() != DEAD && m_pFsmCom->Get_CurrentState() != DAMAGE && m_pFsmCom->Get_CurrentState() != IDLE && m_fTimer > 0.7f)
		{
			m_iTextureNum++;
			m_fTimer = 0;
			if (m_iTextureNum >= 4)
				m_iTextureNum = 0;
		}
		m_pFsmCom->Update(fTimeDelta);
		m_pModelCom->Play_Animation(fTimeDelta);
		m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_Ptr());
		m_pMonsterSoundCom->Update(fTimeDelta);
		m_p3D_Effect->Update(fTimeDelta);
		__super::Update(fTimeDelta);
	}
	else
	{
		if(m_bHide == true)
		{
			m_pEffect->SetActive(true);

			if (m_fDeadTimer > 1.3f)
			{
				m_pMonsterSoundCom->Stop();
				m_isDead = true;
			}
			else
				m_fDeadTimer += fTimeDelta;
		}
	}

	if (m_bActiveEffect && m_bHide == false)
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

void CVegas::Late_Update(_float fTimeDelta)
{
	if (m_isActive)
	{
		__super::Late_Update(fTimeDelta);
		m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
		m_pGameInstance->Add_RenderObject(CRenderer::RG_SHADOWOBJ, this);
		m_pGameInstance->Add_ColliderList(m_pColliderCom);
		m_p3D_Effect->Late_Update(fTimeDelta);
	
#ifdef _DEBUG
		m_pGameInstance->Add_DebugObject(m_pColliderCom);
#endif
	}
	m_pEffect->Late_Update(fTimeDelta);
}


HRESULT CVegas::Render()
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

			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", TEXTURE_TYPE::DIFFUSE, (_uint)i, m_iTextureNum)))
				return E_FAIL;


			if (FAILED(m_pShaderCom->Begin(0)))
				return E_FAIL;

			if (FAILED(m_pModelCom->Render((_uint)i)))
				return E_FAIL;
		}
	}

	return S_OK;
}

void CVegas::OnCollisionEnter(CGameObject* pOther)
{
	if (m_pColliderCom->Get_IsColl())
	{
		if (pOther->Get_LayerTag() == TEXT("Layer_Sword") )
		{
			m_bActiveEffect = true;
			m_pFsmCom->Change_State(DAMAGE);
		}
	}
}

void CVegas::OnCollisionStay(CGameObject* pOther)
{
	if (m_pColliderCom->Get_IsColl())
	{
		if (pOther->Get_LayerTag() == TEXT("Layer_Vegas") && pOther != this)
		{
			_vector newLook = pOther->Get_Transform()->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			newLook = XMVector3Normalize(newLook);

			_vector newBodyPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + -newLook * 0.02f;
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, newBodyPos);
		}
	}
}

void CVegas::OnCollisionExit(CGameObject* pOther)
{
}

HRESULT CVegas::Ready_Components()
{
	__super::Ready_Components();

	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_DUNGEON, TEXT("Prototype_Component_Model_Vegas0"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Collider */
	CBounding_AABB::BOUNDING_AABB_DESC			ColliderDesc{};
	ColliderDesc.vExtents = _float3(0.8f, 0.8f, 0.8f);
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

	/* For.Com_Navigation */
	CNavigation::NAVIGATION_DESC			NaviDesc{};

	NaviDesc.iCurrentIndex = m_iCellNum;
	NaviDesc.iOwnerType = CNavigation::NONPLAYER;

	if (FAILED(__super::Add_Component(LEVEL_DUNGEON, TEXT("Prototype_Component_Navigation"),
		TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &NaviDesc)))
		return E_FAIL;

	CGameObject* pGameObj = m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_3D_Effects"));

	if (pGameObj != nullptr)
	{
		C3D_Effects::MODEL_EFFECT_DESC _Desc{};
		_Desc.iEffectType = VEAGAS_HIT_EFFECT;
		_Desc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
		_Desc.vScale = { 0.3f,0.3f,0.3f };
		_Desc.iLevelIndex = LEVEL_DUNGEON;
		m_p3D_Effect = dynamic_cast<CGameObject*>(pGameObj->Clone(&_Desc));
	}

	pGameObj = m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_MonsterDied_Effect"));

	if (pGameObj != nullptr)
	{
		C2DEffects::EFFECT_DESC Desc{};
		Desc.iLevelIndex = LEVEL_DUNGEON;
		Desc.pParent = this;
		Desc.iEffectType = MONSTER_DIED_EFFECT;
		CGameObject* pEffect = dynamic_cast<CGameObject*>(pGameObj->Clone(&Desc));
		m_pEffect = pEffect;
	}
	return S_OK;
}


HRESULT CVegas::Ready_State()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Fsm"),
		TEXT("Com_Fsm"), reinterpret_cast<CComponent**>(&m_pFsmCom), this)))
		return E_FAIL;

	m_pFsmCom->Add_State(CState_Vegas_Idle::Create(m_pFsmCom, this, IDLE));
	m_pFsmCom->Add_State(CState_Vegas_Walk_f::Create(m_pFsmCom, this, WALK_F));
	m_pFsmCom->Add_State(CState_Vegas_Walk_b::Create(m_pFsmCom, this, WALK_B));
	m_pFsmCom->Add_State(CState_Vegas_Walk_l::Create(m_pFsmCom, this, WALK_L));
	m_pFsmCom->Add_State(CState_Vegas_Walk_r::Create(m_pFsmCom, this, WALK_R));
	m_pFsmCom->Add_State(CState_Vegas_Damage::Create(m_pFsmCom, this, DAMAGE));
	m_pFsmCom->Add_State(CState_Vegas_Dead::Create(m_pFsmCom, this, DEAD));
	return S_OK;
}


CVegas* CVegas::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{

	CVegas* pInstance = new CVegas(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CVegas"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CVegas::Clone(void* pArg)
{
	CVegas* pInstance = new CVegas(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CVegas"));
		Safe_Release(pInstance);
	}

	return pInstance;
}
void CVegas::Free()
{
	__super::Free();

	if (nullptr != m_pFsmCom)
		m_pFsmCom->Release_States();

	Safe_Release(m_pFsmCom);
	Safe_Release(m_pMonsterSoundCom);
}


