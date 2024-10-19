#include "stdafx.h"
#include "Link.h"
#include "GameInstance.h"
#include "Shader.h"
#include "Model.h"
#include "PartObject.h"
#include "../../EngineSDK/Inc/Navigation.h"

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
#include "State_Link_Carry.h"
#include "State_Link_Carry_Walk.h"
#include "State_Link_Throw.h"
#include "State_Link_Get_Item.h"
#include "State_Link_Bomb.h"
#include "State_Link_Bomb.h"
#include "State_Link_Key.h"
#include "State_Link_Push.h"

#include "Cell.h"
#include "Sword.h"
#include "Shield.h"
#include "InteractUI.h"
#include "TreasureBox.h"
#include "ItemUI.h"
#include "Bomb.h"
#include "SquareBlock.h"
#include "NPC.h"
#include "ToolShopkeeper.h"
#include "2DEffects.h"
#include "Player_3D_Effects.h"
#include "MainUI.h"
#include "DialogueUI.h"

_bool CLink::m_bActiveSheild = { true };
_bool CLink::m_bActiveSword = { true };
_bool  CLink::m_bDungeonKey = { true };
 _int CLink::m_iLupee = { 5 };
  _int  CLink::m_iMaxHp = { 8 };
  _int  CLink::m_iCurrentHP = { m_iMaxHp };
  _bool  CLink::m_bBomb = { true };
  _bool  CLink::m_bFeather = { false };

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

	m_iLevelIndex = pDesc->LevelIndex;
	m_iStartCellNum = pDesc->iStartCellNum;

	/* 직교투영을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_State()))
		return E_FAIL;

	if (FAILED(Ready_PartObjects()))
		return E_FAIL;

	if (FAILED(Ready_PlayerUI()))
		return E_FAIL;
	
	if (FAILED(Ready_Effect()))
		return E_FAIL;

	m_pModelCom->SetUp_Animation(30, true);
	m_pFsmCom->Set_State(IDLE);

	m_ePlayer_Dir = FRONT;

	//m_pGameInstance->SetUp_Player(this);

	return S_OK;
}

void CLink::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);

	for (auto& pPartObject : m_Parts)
		pPartObject->Priority_Update(fTimeDelta);

	for (auto& pPlayerUI : m_PlayerUI)
		pPlayerUI->Priority_Update(fTimeDelta);

	for (auto& pEffect : m_pEffect)
		pEffect->Priority_Update(fTimeDelta);
	
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

	for (auto& pPlayerUI : m_PlayerUI)
		pPlayerUI->Update(fTimeDelta);

	for (auto& pEffect : m_pEffect)
		pEffect->Update(fTimeDelta);

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
				//m_bRender = !m_bRender;
				m_bLowAlpha = !m_bLowAlpha;
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

	for (auto& pPlayerUI : m_PlayerUI)
		pPlayerUI->Late_Update(fTimeDelta);

	for (auto& pEffect : m_pEffect)
		pEffect->Late_Update(fTimeDelta);

	m_pGameInstance->Add_ColliderList(m_pColliderCom);
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
	m_pGameInstance->Add_RenderObject(CRenderer::RG_SHADOWOBJ, this);

#ifdef _DEBUG
		m_pGameInstance->Add_DebugObject(m_pColliderCom);
#endif
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
		
		if (FAILED(m_pShaderCom->Bind_RawValue("g_bLowAlpha", &m_bLowAlpha, sizeof(_bool))))
			return E_FAIL;

		_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

		for (size_t i = 0; i < iNumMeshes; i++)
		{
			if (i == 2 || i == 3 || i == 5 || i == 6 || i == 8 || i == 13 || i == 14 || i == 17 || i == 18)
				continue;

			if (m_bActiveSheild == false)
			{
				if ( i == 7)
					continue;
			}

			if (m_bActiveSword == false)
			{
				if (i == 10 || i == 15)
					continue;
			}

			m_pModelCom->Bind_MeshBoneMatrices(m_pShaderCom, "g_BoneMatrices", (_uint)i);

			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", TEXTURE_TYPE::DIFFUSE, (_uint)i)))
				return E_FAIL;

			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", TEXTURE_TYPE::NORMALS, (_uint)i)))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Begin(1)))
				return E_FAIL;			

			if (FAILED(m_pModelCom->Render((_uint)i)))
				return E_FAIL;
		}

		_bool bFalse = false;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_bIsRed", &bFalse, sizeof(_bool))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_bLowAlpha", &bFalse, sizeof(_bool))))
			return E_FAIL;
	
	}

	return S_OK;
}

HRESULT CLink::Render_LightDepth()
{

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldMatrix_Ptr())))
		return E_FAIL;

	_float4x4		ViewMatrix;
	XMStoreFloat4x4(&ViewMatrix, XMMatrixLookAtLH(m_pGameInstance->Get_ShadowLightPos_Vector(), m_pGameInstance->Get_LightLook_Vector(), XMVectorSet(0.f, 1.f, 0.f, 0.f)));
	//XMStoreFloat4x4(&ViewMatrix, XMMatrixLookAtLH(XMVectorSet(5.f, 30.f, -15.f, 1.f), m_pGameInstance->Get_LightLook_Vector(), XMVectorSet(0.f, 1.f, 0.f, 0.f)));

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (i == 2 || i == 3 || i == 5 || i == 6 || i == 8 || i == 13 || i == 14 || i == 17 || i == 18)
			continue;

		if (m_bActiveSheild == false)
		{
			if (i == 7)
				continue;
		}

		if (m_bActiveSword == false)
		{
			if (i == 10 || i == 15)
				continue;
		}

		m_pModelCom->Bind_MeshBoneMatrices(m_pShaderCom, "g_BoneMatrices", (_uint)i);

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", TEXTURE_TYPE::DIFFUSE, (_uint)i)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", TEXTURE_TYPE::NORMALS, (_uint)i)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(6)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render((_uint)i)))
			return E_FAIL;
	}

	return S_OK;
}

void CLink::OnCollisionEnter(CGameObject* pOther)
{
	if (m_pColliderCom->Get_IsColl())
	{
		if (pOther->Get_LayerTag() == TEXT("Layer_Monster") || pOther->Get_ObjType() == CGameObject::ANIM_MONSTER)
			Change_State(DAMAGE_FRONT);

		if (pOther->Get_LayerTag() == TEXT("Layer_TreasureBox"))
		{
			if (static_cast<CTreasureBox*>(pOther)->Get_IsOpened() == false)
			{
				m_PlayerUI[INTERACT_UI]->SetActive(true);
				m_PlayerUI[INTERACT_UI]->Set_TextureNum(0);
				
			}
		}

		else if (pOther->Get_LayerTag() == TEXT("Layer_HousePot"))
		{		
			if(m_pCarryitem == nullptr)
			{
				m_PlayerUI[INTERACT_UI]->SetActive(true);
				m_PlayerUI[INTERACT_UI]->Set_TextureNum(2);
			}
		}
		
		else if (pOther->Get_LayerTag() == TEXT("Layer_TailLockStatue"))
		{		
			if(m_bDungeonKey)
			{
				m_PlayerUI[INTERACT_UI]->SetActive(true);
				m_PlayerUI[INTERACT_UI]->Set_TextureNum(0);
			}
		}

		else if (pOther->Get_LayerTag() == TEXT("Layer_SmallKey"))
		{
			Change_PlayerUI_TextureNum(ITEM_ICON_UI,CItemUI::SMALLKEY);
			static_cast<CDialogueUI*>(static_cast<CMainUI*>(m_pGameInstance->Find_Object(m_iLevelIndex, TEXT("Layer_MainUI"), 0))->Get_ChildUI(CMainUI::DIALOGUE))->Set_LineNum(CDialogueUI::SMALL_KEY);
			Change_State(GET_ITEM);
		}

		else if (pOther->Get_LayerTag() == TEXT("Layer_LockDoor") || pOther->Get_LayerTag() == TEXT("Layer_LockBlock"))
		{
			if (m_iSmallKeyCount >= 1)
			{
				m_PlayerUI[INTERACT_UI]->SetActive(true);
				m_PlayerUI[INTERACT_UI]->Set_TextureNum(0);
			}
		}


		else if (pOther->Get_LayerTag() == TEXT("Layer_BossDoor"))
		{
			if (m_bGetBossKey)
			{
				m_PlayerUI[INTERACT_UI]->SetActive(true);
				m_PlayerUI[INTERACT_UI]->Set_TextureNum(0);
			}
		}

		else if (pOther->Get_LayerTag() == TEXT("Layer_DungeonOwlStatue") && m_bBeak)
		{

			m_PlayerUI[INTERACT_UI]->SetActive(true);
			m_PlayerUI[INTERACT_UI]->Set_TextureNum(3);
			
		}

		else if (pOther->Get_LayerTag() == TEXT("Layer_ConchHorn"))
		{
			m_bEnding = true;
			m_PlayerUI[ITEM_ICON_UI]->SetActive(true);
			m_PlayerUI[ITEM_ICON_UI]->Set_TextureNum(7);
			m_pFsmCom->Change_State(GET_ITEM);
		}

		else if (pOther->Get_LayerTag() == TEXT("Layer_SquareBlock_Gimmick"))
		{
			if(static_cast<CSquareBlock*>(pOther)->Get_IsPushed() == false)
				m_pFsmCom->Change_State(PUSH);
		}

		else if (pOther->Get_LayerTag() == TEXT("Layer_SinkingSword"))
		{
			m_PlayerUI[ITEM_ICON_UI]->SetActive(true);
			m_PlayerUI[ITEM_ICON_UI]->Set_TextureNum(8);
			static_cast<CDialogueUI*>(static_cast<CMainUI*>(m_pGameInstance->Find_Object(m_iLevelIndex, TEXT("Layer_MainUI"), 0))->Get_ChildUI(CMainUI::DIALOGUE))->Set_LineNum(CDialogueUI::SWORD);
			m_pFsmCom->Change_State(GET_ITEM);
		}
		
	}
}

void CLink::OnCollisionStay(CGameObject* pOther)
{
	if (m_pColliderCom->Get_IsColl())
	{
		if (pOther->Get_LayerTag() == TEXT("Layer_TreasureBox"))
		{
			if (static_cast<CTreasureBox*>(pOther)->Get_IsOpened() == true)
			{
				m_PlayerUI[INTERACT_UI]->SetActive(false);
			}
		}

		else if (pOther->Get_LayerTag() == TEXT("Layer_HousePot"))
		{
			if (KEY_TAP(E))
			{
				m_pFsmCom->Change_State(CARRY);
				m_pGameInstance->Destroy_PhysXActor(pOther);
				m_pCarryitem = pOther;
			}
		}

		else if (pOther->Get_LayerTag() == TEXT("Layer_LockDoor") || pOther->Get_LayerTag() == TEXT("Layer_LockBlock"))
		{
			if (m_iSmallKeyCount >= 1)
			{
				if (KEY_AWAY(E))
				{
					if(m_pFsmCom->Get_CurrentState() != KEY)
						m_pFsmCom->Change_State(KEY);
				}
			}
		}

		else if (pOther->Get_LayerTag() == TEXT("Layer_BossDoor") )
		{
			if (m_bGetBossKey)
			{
				if (KEY_AWAY(E))
				{
					m_bGetBossKey = false;

					if (m_pFsmCom->Get_CurrentState() != KEY)
						m_pFsmCom->Change_State(KEY);
				}
			}
		}

		else if (pOther->Get_LayerTag() == TEXT("Layer_DungeonOwlStatue"))
		{
			if (KEY_TAP(E))
			{
				m_PlayerUI[INTERACT_UI]->SetActive(false);
			}
		}

		else if (pOther->Get_LayerTag() == TEXT("Layer_TailLockStatue"))
		{
			if (m_bDungeonKey && KEY_TAP(E))
			{
				if (m_pFsmCom->Get_CurrentState() != KEY)
					m_pFsmCom->Change_State(KEY);
				m_PlayerUI[INTERACT_UI]->SetActive(false);
				m_bDungeonKey = false;
			}
		}

		else if (pOther->Get_LayerTag() == TEXT("Layer_Store_Item"))
		{
			if (m_pCarryitem == nullptr)
			{
				m_PlayerUI[INTERACT_UI]->SetActive(true);
				m_PlayerUI[INTERACT_UI]->Set_TextureNum(2);
			}
			else
				m_PlayerUI[INTERACT_UI]->SetActive(false);
			if (KEY_TAP(E))
			{
				m_PlayerUI[INTERACT_UI]->SetActive(false);
				Change_State(CARRY);
				m_pCarryitem = pOther;
			}
		}

		else if (pOther->Get_LayerTag() == TEXT("Layer_NPC"))
		{
			if (static_cast<CNPC*>(pOther)->Get_Talk() == false)
			{
				//UI가 너무 빨리 나와서 딜레이를 줌
				if(static_cast<CNPC*>(pOther)->Get_UIObject(CNPC::DIALOGUE_UI)->IsActive() == false && m_bTalk == false) 
				{
					if(m_pFsmCom->Get_CurrentState() != GET_ITEM)
					{
						m_PlayerUI[INTERACT_UI]->SetActive(true);
						m_PlayerUI[INTERACT_UI]->Set_TextureNum(1);
					}
				}

				if (KEY_TAP(KEY::E))
				{
					if(dynamic_cast<CToolShopkeeper*>(pOther) != nullptr)
					{
						if(dynamic_cast<CToolShopkeeper*>(pOther)->Get_Fsm()->Get_PrevState() != CToolShopkeeper::TALK)
						{
							static_cast<CNPC*>(pOther)->Set_Talk(true);
							m_bTalk = true;
						}
					}
				}
			}
			else
				m_PlayerUI[INTERACT_UI]->SetActive(false);
		}

		if (pOther->Get_LayerTag() == TEXT("Layer_Lupee"))
		{
			m_iLupee += 5;
		}
	}
}

void CLink::OnCollisionExit(CGameObject* pOther)
{

	if (pOther->Get_LayerTag() == TEXT("Layer_TreasureBox") || 
		pOther->Get_LayerTag() == TEXT("Layer_LockDoor") ||
		pOther->Get_LayerTag() == TEXT("Layer_LockBlock") || 
		pOther->Get_LayerTag() == TEXT("Layer_TailLockStatue") || 
		pOther->Get_LayerTag() == TEXT("Layer_BossDoor")
		)
	{
		m_PlayerUI[0]->SetActive(false);
	}
	
	if (pOther->Get_LayerTag() == TEXT("Layer_HousePot"))
	{
		m_PlayerUI[0]->SetActive(false);
	}

	if (pOther->Get_LayerTag() == TEXT("Layer_DungeonOwlStatue"))
	{
		m_PlayerUI[0]->SetActive(false);
	}

	if (pOther->Get_LayerTag() == TEXT("Layer_Store_Item"))
	{
		m_PlayerUI[INTERACT_UI]->SetActive(false);
	}

	if (pOther->Get_LayerTag() == TEXT("Layer_NPC"))
	{
		m_PlayerUI[INTERACT_UI]->SetActive(false);
	}
}

void CLink::Get_Item(_uint iItemIndex)
{
	switch (iItemIndex)
	{
	case CItemUI::STONEBEAK:
		m_bBeak = true;
		break;
	case CItemUI::CAMPUS:
		m_bCampus = true;
		break;
	case CItemUI::SMALLKEY:
		m_iSmallKeyCount++;
		break;
	case CItemUI::MAP:
		m_bMap = true;
		break;
	case CItemUI::LUPEE:
		m_iLupee += 20;
		break;
	case CItemUI::FEATHER:
		m_bFeather = true;
		break;
	case CItemUI::BOSSKEY:
		m_bGetBossKey = true;
		break;
	default:
		break;
	}
}

HRESULT CLink::Ready_Components()
{
	__super::Ready_Components();

	if(m_iLevelIndex != LEVEL_TEST)

	{
		/* For.Com_Navigation */
		CNavigation::NAVIGATION_DESC			NaviDesc{};

		NaviDesc.iCurrentIndex = m_iStartCellNum;
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
	SwordDesc.pMonsterCount = &m_fMonsterCount;

	if (FAILED(__super::Add_PartObject(CPlayer::PART_SWORD, TEXT("Prototype_GameObject_Sword"), &SwordDesc)))
		return E_FAIL;
	

	CShield::SHIELD_DESC		ShieldDesc{};
	ShieldDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
	ShieldDesc.pSocketBoneMatrix = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr("itemA_R");
	ShieldDesc.pPlayerFsm = m_pFsmCom;

	if (FAILED(__super::Add_PartObject(CPlayer::PART_SHIELD, TEXT("Prototype_GameObject_Shield"), &ShieldDesc)))
		return E_FAIL;

	CBomb::BOMB_DESC bombDesc{};
	bombDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
	bombDesc.pSocketBoneMatrix = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr("itemA_L");
	bombDesc.iLevelIndex = m_iLevelIndex;
	if (FAILED(__super::Add_PartObject(CPlayer::PART_BOMB, TEXT("Prototype_GameObject_Bomb"), &bombDesc)))
		return E_FAIL;

	CPlayer_3D_Effects::PLAYER_3D_DESC effectDesc{};
	effectDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
	effectDesc.pSocketBoneMatrix = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr("itemA_L");
	if (FAILED(__super::Add_PartObject(CPlayer::PART_3D_EFFECT, TEXT("Prototype_GameObject_Player_3D_Effects"), &effectDesc)))
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
	m_pFsmCom->Add_State(CState_Link_Carry::Create(m_pFsmCom, this, CARRY));
	m_pFsmCom->Add_State(CState_Link_Carry_Walk::Create(m_pFsmCom, this, CARRY_WALK));
	m_pFsmCom->Add_State(CState_Link_Throw::Create(m_pFsmCom, this, THROW));
	m_pFsmCom->Add_State(CState_Link_Get_Item::Create(m_pFsmCom, this, GET_ITEM));
	m_pFsmCom->Add_State(CState_Link_Bomb::Create(m_pFsmCom, this, BOMB));
	m_pFsmCom->Add_State(CState_Link_Key::Create(m_pFsmCom, this, KEY));
	m_pFsmCom->Add_State(CState_Link_Push::Create(m_pFsmCom, this, PUSH));

	return S_OK;
}

HRESULT CLink::Ready_PlayerUI()
{
	CGameObject* pGameObj = m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_InteractUI"));
	if (pGameObj != nullptr)
	{
		CUIObject::UI_DESC pDesc;
		pDesc.fSizeX = 192.f / 3.f;
		pDesc.fSizeY = 96.f / 3.f;
		pDesc.fX = 640.f;
		pDesc.fY = 360.f;

		CUIObject* m_pInteractUI = dynamic_cast<CUIObject*>(pGameObj->Clone(&pDesc));
		m_pInteractUI->Set_ParentObj(this);
		m_PlayerUI.push_back(m_pInteractUI);
	}

	pGameObj = m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_ItemtUI"));
	if (pGameObj != nullptr)
	{
		CUIObject::UI_DESC pDesc;
		pDesc.fSizeX = 96.f;
		pDesc.fSizeY = 96.f ;
		pDesc.fX = 640.f;
		pDesc.fY = 360.f;
	
		CUIObject*  m_pItemUI = dynamic_cast<CUIObject*>(pGameObj->Clone(&pDesc));
		m_pItemUI->Set_ParentObj(this);
		m_PlayerUI.push_back(m_pItemUI);
	}

	return S_OK;
}

HRESULT CLink::Ready_Effect()
{
	CGameObject* pGameObj = m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Player_ItemGet_Effect"));

	if (pGameObj != nullptr)
	{
		C2DEffects::EFFECT_DESC Desc{};
		Desc.iLevelIndex = m_iLevelIndex;
		Desc.pParent = this;
		Desc.iEffectType = PLAYER_ITEM_GET_EFFECT;
		CGameObject* pEffect = dynamic_cast<CGameObject*>(pGameObj->Clone(&Desc));
		m_pEffect.push_back(pEffect);
	}

	pGameObj = m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Player_Charge_Slash"));
	if (pGameObj != nullptr)
	{
		C2DEffects::EFFECT_DESC Desc{};
		Desc.iLevelIndex = m_iLevelIndex;
		Desc.pParent = this;
		Desc.iEffectType = PLAYER_CHARGE_SLASH_EFFECT;
		CGameObject* pEffect = dynamic_cast<CGameObject*>(pGameObj->Clone(&Desc));
		m_pEffect.push_back(pEffect);
	}
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
