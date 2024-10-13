#include "stdafx.h"
#include "DeguTail_00.h"
#include "GameInstance.h"
#include "Shader.h"
#include "Model.h"
#include "State_DeguTail_Idle.h"
#include "State_DeguTail_Walk.h"
#include "State_DeguTail_Appear.h"
#include "State_DeguTail_Hurt.h"
#include "State_DeguTail_Guard.h"
#include "State_DeguTail_Dead.h"
#include "DeguTail_01.h"
#include "DeguTail_04.h"
#include "2DEffects.h"

CDeguTail_00::CDeguTail_00(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CMonster{ pDevice, pContext }
{
}


CDeguTail_00::CDeguTail_00(const CDeguTail_00& Prototype)
    : CMonster{ Prototype }
{
}

HRESULT CDeguTail_00::Initialize_Prototype()
{
    return S_OK;
}


HRESULT CDeguTail_00::Initialize(void* pArg)
{

	/* 직교투영을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_State()))
		return E_FAIL;

	if (FAILED(Ready_PartObjects()))
		return E_FAIL;

	m_pFsmCom->Set_State(APPEAR);

	m_iDir = (int)m_pGameInstance->Get_Random(0, 4);
	m_pTransformCom->RotationThreeAxis(_float3(0.f, 180.f, 0.f));
	
	//벡터 사이즈 임의 지정
	m_MParentWorldMarix.reserve(100);

	m_iHp = 5;
	m_eObjType = CGameObject::ANIM_MONSTER;
	m_isActive = false;
	return S_OK;
}
void CDeguTail_00::Priority_Update(_float fTimeDelta)
{
	if (m_isActive == true)
	{
		__super::Priority_Update(fTimeDelta);
	}
	m_pEffect->Priority_Update(fTimeDelta);
}


void CDeguTail_00::Update(_float fTimeDelta)
{
	if (m_isActive == true)
	{
		m_pMonsterSoundCom->Update(fTimeDelta);

		if (m_pNavigationCom != nullptr)
			m_pNavigationCom->SetUp_OnCell(m_pTransformCom, 0.5f, fTimeDelta);

		m_pFsmCom->Update(fTimeDelta);
		m_pModelCom->Play_Animation(fTimeDelta);

		__super::Update(fTimeDelta);

		m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_Ptr());

		if (m_iHp <= 0 && m_bBodyRed == false)
		{
			m_iHp--;
			m_bBodyRed = true;
			m_pFsmCom->Change_State(DEAD);
		}

		if (m_isDead)
		{
			m_pEffect->SetActive(true);
			m_isActive = false;
		}
	}
	m_pEffect->Update(fTimeDelta);
}

void CDeguTail_00::Late_Update(_float fTimeDelta)
{
	if (m_isActive == true)
	{
		__super::Late_Update(fTimeDelta);

		m_pGameInstance->Add_ColliderList(m_pColliderCom);
		m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
	}
	m_pEffect->Late_Update(fTimeDelta);
}
HRESULT CDeguTail_00::Render()
{
	if(m_isActive == true)
	{
		if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
			return E_FAIL;

		_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();
		_bool bTrue = true;
		_bool bFalse = false;

		for (size_t i = 0; i < iNumMeshes; i++)
		{
			if( i == 2)
			{
				if (FAILED(m_pShaderCom->Bind_RawValue("g_bIsRed", &m_bBodyRed, sizeof(_bool))))
					return E_FAIL;
				if (FAILED(m_pShaderCom->Bind_RawValue("g_bOutBodyIsRed", &m_bOutBodyRed, sizeof(_bool))))
					return E_FAIL;
			}
			else
			{
				if (FAILED(m_pShaderCom->Bind_RawValue("g_bIsRed", &bFalse, sizeof(_bool))))
					return E_FAIL;
				if (FAILED(m_pShaderCom->Bind_RawValue("g_bOutBodyIsRed", &bFalse, sizeof(_bool))))
					return E_FAIL;
			}

			if (FAILED(m_pShaderCom->Bind_RawValue("g_bRedBlink", &m_bBlink, sizeof(_bool))))
				return E_FAIL;

			m_pModelCom->Bind_MeshBoneMatrices(m_pShaderCom, "g_BoneMatrices", (_uint)i);

			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", TEXTURE_TYPE::DIFFUSE, (_uint)i)))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Begin(1)))
				return E_FAIL;

			if (FAILED(m_pModelCom->Render((_uint)i)))
				return E_FAIL;
		}
		//다른 모델한테 영향이 가면 안되서 dead처리를 풀어줘야 함


		if (FAILED(m_pShaderCom->Bind_RawValue("g_bIsRed", &bFalse, sizeof(_bool))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_bOutBodyIsRed", &bFalse, sizeof(_bool))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_bRedBlink", &bFalse, sizeof(_bool))))
			return E_FAIL;

		

#ifdef _DEBUG
		m_pColliderCom->Render();
#endif	

		/*if (m_bRenderText)
			m_pGameInstance->Render_Text(TEXT("Font_BossName36"), TEXT("거대 꼬리리"), XMVectorSet(g_iWinSizeX / 2 - 20.f, g_iWinSizeY / 2 - 10, 0.f, 1.f));*/
	}
	return S_OK;
}

void CDeguTail_00::OnCollisionEnter(CGameObject* pOther)
{
	if (m_pColliderCom->Get_IsColl())
	{
		if (pOther->Get_LayerTag() == TEXT("Layer_Sword"))
		{
			Change_State(GUARD);
		}
	}
}

void CDeguTail_00::OnCollisionStay(CGameObject* pOther)
{
}

void CDeguTail_00::OnCollisionExit(CGameObject* pOther)
{
}

HRESULT CDeguTail_00::Ready_Components()
{
	__super::Ready_Components();

	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_DUNGEON, TEXT("Prototype_Component_Model_DeguTail01"),
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
	ColliderDesc.vExtents = m_pTransformCom->Get_Scaled();
	ColliderDesc.vCenter = _float3(0.f, 0.f, 0.f);

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
	  
	return S_OK;
}

HRESULT CDeguTail_00::Ready_PartObjects()
{
	m_Parts.resize(PART_END);

	CDeguTail_01::DEGUBODY_DESC		BodyDesc{};
	BodyDesc.vSize = _float3(0.9f, 0.9f, 0.9f);
	BodyDesc.iCellNum = m_pNavigationCom->Get_CurrentCellIndex();
	BodyDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
	BodyDesc.pParent = this;
	BodyDesc.fWaitTime = 0.2f;
	BodyDesc.pFsm = m_pFsmCom;
	BodyDesc.pBodyRed = &m_bBodyRed;
	BodyDesc.pOutBodyRed = &m_bOutBodyRed;
	BodyDesc.pBlink = &m_bBlink;
	if (FAILED(__super::Add_PartObject(PART_BODY1, TEXT("Prototype_GameObject_DeguTail_01"), &BodyDesc)))
		return E_FAIL;

	BodyDesc.vSize = _float3(0.8f, 0.8f, 0.8f);
	BodyDesc.iCellNum = m_pNavigationCom->Get_CurrentCellIndex();
	BodyDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
	BodyDesc.pParent = dynamic_cast<CGameObject*>(m_Parts[PART_BODY1]);
	BodyDesc.fWaitTime = 0.4f;
	BodyDesc.pFsm = m_pFsmCom;
	BodyDesc.pBodyRed = &m_bBodyRed;
	BodyDesc.pOutBodyRed = &m_bOutBodyRed;
	BodyDesc.pBlink = &m_bBlink;
	if (FAILED(__super::Add_PartObject(PART_BODY2, TEXT("Prototype_GameObject_DeguTail_01"), &BodyDesc)))
		return E_FAIL;

	BodyDesc.vSize = _float3(0.7f, 0.7f, 0.7f);
	BodyDesc.iCellNum = m_pNavigationCom->Get_CurrentCellIndex();
	BodyDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
	BodyDesc.pParent = dynamic_cast<CGameObject*>(m_Parts[PART_BODY2]);
	BodyDesc.fWaitTime = 0.6f;
	BodyDesc.pFsm = m_pFsmCom;
	BodyDesc.pBodyRed = &m_bBodyRed;
	BodyDesc.pOutBodyRed = &m_bOutBodyRed;
	BodyDesc.pBlink = &m_bBlink;
	if (FAILED(__super::Add_PartObject(PART_BODY3, TEXT("Prototype_GameObject_DeguTail_01"), &BodyDesc)))
		return E_FAIL;

	CDeguTail_04::DEGUTAIL_DESC		TailDesc{};
	TailDesc.vSize = _float3(1.f,1.f,1.f);
	TailDesc.iCellNum = m_pNavigationCom->Get_CurrentCellIndex();
	TailDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
	TailDesc.pParent = dynamic_cast<CGameObject*>(m_Parts[PART_BODY3]);
	TailDesc.fWaitTime = 0.8f;
	TailDesc.pFsm = m_pFsmCom;
	if (FAILED(__super::Add_PartObject(PART_TAIL, TEXT("Prototype_GameObject_DeguTail_04"), &TailDesc)))
		return E_FAIL;

    return S_OK;
}

HRESULT CDeguTail_00::Ready_State()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Fsm"),
		TEXT("Com_Fsm"), reinterpret_cast<CComponent**>(&m_pFsmCom), this)))
		return E_FAIL;

	m_pFsmCom->Add_State(CState_DeguTail_Idle::Create(m_pFsmCom, this, IDLE));
	m_pFsmCom->Add_State(CState_DeguTail_Walk::Create(m_pFsmCom, this, WALK));
	m_pFsmCom->Add_State(CState_DeguTail_Appear::Create(m_pFsmCom, this, APPEAR));
	m_pFsmCom->Add_State(CState_DeguTail_Hurt::Create(m_pFsmCom, this, HURT));
	m_pFsmCom->Add_State(CState_DeguTail_Guard::Create(m_pFsmCom, this, GUARD));
	m_pFsmCom->Add_State(CState_DeguTail_Dead::Create(m_pFsmCom, this, DEAD));
    return S_OK;
}

void CDeguTail_00::Add_Vec_Matrix()
{
	m_MParentWorldMarix.insert(m_MParentWorldMarix.begin(), m_pTransformCom->Get_WorldMatrix());
}

void CDeguTail_00::Kill_Parts(_float fTimeDelta)
{
	fTimer += fTimeDelta;

	if(fTimer > 0.5f)
	{
		if (m_iPartIndex == 0)
		{
			m_isDead = true;
			m_pColliderCom->Set_IsActive(false);
		}

		--m_iPartIndex;

		if (m_iPartIndex < 0)
			m_iPartIndex = 0;

		m_pMonsterSoundCom->Play_Sound(TEXT("3_Monster_Explosion.wav"), 1.f);
		m_Parts[m_iPartIndex]->Set_Dead(true);
		fTimer = 0.f;
	}

}

CDeguTail_00* CDeguTail_00::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDeguTail_00* pInstance = new CDeguTail_00(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CDeguTail_00"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDeguTail_00::Clone(void* pArg)
{
	CDeguTail_00* pInstance = new CDeguTail_00(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CDeguTail_00"));
		Safe_Release(pInstance);
	}

	return pInstance;
}
void CDeguTail_00::Free()
{
	__super::Free();

	if (nullptr != m_pFsmCom)
		m_pFsmCom->Release_States();

	m_MParentWorldMarix.clear();

	Safe_Release(m_pFsmCom);
	Safe_Release(m_pMonsterSoundCom);
}

