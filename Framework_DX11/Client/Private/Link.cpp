#include "stdafx.h"
#include "Link.h"
#include "GameInstance.h"
#include "Shader.h"
#include "Model.h"
#include "PartObject.h"

#include "State_Link_Idle.h"
#include "State_Link_Walk.h"
#include "State_Link_ItemA.h"
#include "State_Link_ItemB.h"
#include "State_Link_Jump.h"
#include "State_Link_Slash.h"
#include "State_Link_Slash_Hold.h"
#include "State_Link_Shield.h"
#include "State_Link_Shield_Walk.h"
#include "State_Link_Stair_Up.h"
#include "State_Link_Stair_Down.h"
#include "State_Link_Fall.h"
#include "State_Link_Damage_Front.h"

#include "Cell.h"
#include "Sword.h"
#include "Shield.h"

CLink::CLink(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CPlayer{ pDevice, pContext }
{
}

CLink::CLink(const CLink& Prototype)
    : CPlayer{ Prototype }
{
}

HRESULT CLink::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CLink::Initialize(void* pArg)
{
	PLAYER_DESC* pDesc = static_cast<PLAYER_DESC*>(pArg);

	/* 직교투영을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_State()))
		return E_FAIL;

	if (FAILED(Ready_PartObjects()))
		return E_FAIL;

	m_pModelCom->SetUp_Animation(30, true);
	m_pFsmCom->Set_State(IDLE);

	m_ePlayer_Dir = FRONT;
	m_iLevelIndex = pDesc->LevelIndex;
	m_pGameInstance->SetUp_Player(this);

	return S_OK;
}

void CLink::Priority_Update(_float fTimeDelta)
{
	for (auto& pPartObject : m_Parts)
		pPartObject->Priority_Update(fTimeDelta);
}

void CLink::Update(_float fTimeDelta)
{
	//점프일때는 자동으로 땅 타지 않도록
	if (m_pFsmCom->Get_CurrentState() != JUMP && m_pNavigationCom != nullptr && m_bFall == false)
		m_pNavigationCom->SetUp_OnCell(m_pTransformCom, 0.f, fTimeDelta);

	m_pFsmCom->Update(fTimeDelta);
	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_Ptr());
	m_pModelCom->Play_Animation(fTimeDelta);
	m_pPlayerSoundCom->Update(fTimeDelta);
	m_pEffectSoundCom->Update(fTimeDelta);

//	int a = m_pNavigationCom->Get_PreCellIndex();

	for (auto& pPartObject : m_Parts)
		pPartObject->Update(fTimeDelta);

	//떨어지기
	if (m_pNavigationCom != nullptr && m_pNavigationCom->Get_CurrentCellType() == CCell::CELL_CLIFF && m_pFsmCom->Get_CurrentState() != JUMP)
	{
		if(m_pFsmCom->Get_CurrentState() != FALL)
			m_pFsmCom->Change_State(FALL);
	}

	//깜빡이기
	if (m_bBlink )
	{
		if(m_iBlinkCount < 6)
		{
			m_fBlinkTimer += fTimeDelta;

			if (m_fBlinkTimer > 0.3f)
			{
				m_bRender = !m_bRender;
				m_fBlinkTimer = 0.f;
				m_iBlinkCount++;
			}
		}
		else
		{
			m_bBlink = false;
			m_iBlinkCount = 0;
			m_fBlinkTimer = 0.f;
			m_pColliderCom->Set_IsActive(true);
		}
	}

}

void CLink::Late_Update(_float fTimeDelta)
{
	for (auto& pPartObject : m_Parts)
		pPartObject->Late_Update(fTimeDelta);

	m_pGameInstance->Add_ColliderList(m_pColliderCom);
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CLink::Render()
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
			if ( i == 1 || i == 3 || i == 5 || i == 9 || i == 11 || i == 12 || i == 16 || i == 18 || i == 17)
				continue;

			if (m_bActiveSheild == false)
			{
				if ( i == 7)
					continue;
			}

			if (m_bActiveSword == false)
			{
				if (i == 2 || i == 6)
					continue;
			}

			m_pModelCom->Bind_MeshBoneMatrices(m_pShaderCom, "g_BoneMatrices", (_uint)i);

			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", TEXTURE_TYPE::DIFFUSE, (_uint)i)))
				return E_FAIL;


			if (FAILED(m_pShaderCom->Begin(1)))
				return E_FAIL;

			if (FAILED(m_pModelCom->Render((_uint)i)))
				return E_FAIL;
		}

		_bool bFalse = false;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_bIsRed", &bFalse, sizeof(_bool))))
			return E_FAIL;
	}

#ifdef _DEBUG
	m_pColliderCom->Render();
#endif

	return S_OK;
}

void CLink::OnCollisionEnter(CGameObject* pOther)
{
	if (m_pColliderCom->Get_IsColl())
	{
		if (pOther->Get_LayerTag() == TEXT("Layer_Monster"))
		{
			Change_State(DAMAGE_FRONT);
		}
	}
}

void CLink::OnCollisionStay(CGameObject* pOther)
{
}

void CLink::OnCollisionExit(CGameObject* pOther)
{
}

HRESULT CLink::Ready_Components()
{
	__super::Ready_Components();

	if(m_iLevelIndex != LEVEL_TEST )

	{
		/* For.Com_Navigation */
		CNavigation::NAVIGATION_DESC			NaviDesc{};

		NaviDesc.iCurrentIndex = 0;
		NaviDesc.iOwnerType = CNavigation::PLAYER;

		if (FAILED(__super::Add_Component(m_iLevelIndex, TEXT("Prototype_Component_Navigation"),
			TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &NaviDesc)))
			return E_FAIL;
	}

	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Link"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Collider */
	CBounding_AABB::BOUNDING_AABB_DESC			ColliderDesc{};
	ColliderDesc.vExtents = _float3(0.4f, 0.7f, 0.4f);
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vExtents.y, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;
	m_pColliderCom->Set_Owner(this);

	/* FOR.Com_Sound */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sound"),
		TEXT("Com_Sound0"), reinterpret_cast<CComponent**>(&m_pPlayerSoundCom))))
		return E_FAIL;
	m_pPlayerSoundCom->Set_Owner(this);

	/* FOR.Com_Sound */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sound"),
		TEXT("Com_Sound1"), reinterpret_cast<CComponent**>(&m_pEffectSoundCom))))
		return E_FAIL;
	m_pEffectSoundCom->Set_Owner(this);
	return S_OK;
}

HRESULT CLink::Ready_PartObjects()
{
	/* 실제 추가하고 싶은 파트오브젝트의 갯수만큼 밸류를 셋팅해놓자. */
	m_Parts.resize(PART_END);

	CSword::SWORD_DESC		SwordDesc{};
	SwordDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
	SwordDesc.pSocketBoneMatrix =m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr("itemA_L");
	SwordDesc.pPlayerFsm = m_pFsmCom;

	if (FAILED(__super::Add_PartObject(CPlayer::PART_SWORD, TEXT("Prototype_GameObject_Sword"), &SwordDesc)))
		return E_FAIL;
	

	CShield::SHIELD_DESC		ShieldDesc{};
	ShieldDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
	ShieldDesc.pSocketBoneMatrix = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr("itemA_R");
	ShieldDesc.pPlayerFsm = m_pFsmCom;

	if (FAILED(__super::Add_PartObject(CPlayer::PART_SHIELD, TEXT("Prototype_GameObject_Shield"), &ShieldDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLink::Ready_State()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Fsm"),
		TEXT("Com_Fsm"), reinterpret_cast<CComponent**>(&m_pFsmCom), this)))
		return E_FAIL;

	m_pFsmCom->Add_State(CState_Link_Idle::Create(m_pFsmCom, this, IDLE));
	m_pFsmCom->Add_State(CState_Link_Walk::Create(m_pFsmCom, this, WALK));
	m_pFsmCom->Add_State(CState_Link_ItemA::Create(m_pFsmCom, this, ITEMA));
	m_pFsmCom->Add_State(CState_Link_ItemB::Create(m_pFsmCom, this, ITEMB));
	m_pFsmCom->Add_State(CState_Link_Jump::Create(m_pFsmCom, this, JUMP));
	m_pFsmCom->Add_State(CState_Link_Slash::Create(m_pFsmCom, this, SLASH));
	m_pFsmCom->Add_State(CState_Link_Slash_Hold::Create(m_pFsmCom, this, SLASH_HOLD));
	m_pFsmCom->Add_State(CState_Link_Shield::Create(m_pFsmCom, this, SHIELD));
	m_pFsmCom->Add_State(CState_Link_Shield_Walk::Create(m_pFsmCom, this, SHIELD_WALK));
	m_pFsmCom->Add_State(CState_Link_Stair_Up::Create(m_pFsmCom, this, STAIR_UP));
	m_pFsmCom->Add_State(CState_Link_Stair_Down::Create(m_pFsmCom, this, STAIR_DOWN));
	m_pFsmCom->Add_State(CState_Link_Fall::Create(m_pFsmCom, this, FALL));
	m_pFsmCom->Add_State(CState_Link_Damage_Front::Create(m_pFsmCom, this, DAMAGE_FRONT));

	return S_OK;
}

CLink* CLink::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLink* pInstance = new CLink(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CLink"));
		Safe_Release(pInstance);
	}

	return pInstance;
}
CGameObject* CLink::Clone(void* pArg)
{
	CLink* pInstance = new CLink(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CLink"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLink::Free()
{
	__super::Free();

	if (nullptr != m_pFsmCom)
		m_pFsmCom->Release_States();

	Safe_Release(m_pFsmCom);
	Safe_Release(m_pPlayerSoundCom);
	Safe_Release(m_pEffectSoundCom);

}
