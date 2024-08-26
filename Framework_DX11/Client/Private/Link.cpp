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

	m_pModelCom->SetUp_Animation(30, true);
	m_pFsmCom->Set_State(IDLE);

	m_ePlayer_Dir = FRONT;
	m_iLevelIndex = pDesc->LevelIndex;
	//m_pGameInstance->SetUp_Player(this);

	return S_OK;
}

void CLink::Priority_Update(_float fTimeDelta)
{
	for (auto& pPartObject : m_Parts)
		pPartObject->Priority_Update(fTimeDelta);
}

void CLink::Update(_float fTimeDelta)
{
	m_pFsmCom->Update(fTimeDelta);

	m_pModelCom->Play_Animation(fTimeDelta);

	//점프일때는 자동으로 땅 타지 않도록
	if(m_pFsmCom->Get_CurrentState() != JUMP)
		m_pNavigationCom->SetUp_OnCell(m_pTransformCom, 0.f );

	for (auto& pPartObject : m_Parts)
		pPartObject->Update(fTimeDelta);
}

void CLink::Late_Update(_float fTimeDelta)
{
	for (auto& pPartObject : m_Parts)
		pPartObject->Late_Update(fTimeDelta);

	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CLink::Render()
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
		if (i == 0 ||  i == 4 || i == 10 || i == 12 ||  i == 17 || i == 1 || i == 6 || i == 7)
			continue;

		if(m_bActiveSheild == false)
		{
			if (i == 2 || i == 18)
				continue;
		}

		if (m_bActiveSword == false)
		{
			if (i == 13 || i == 16)
				continue;
		}

		m_pModelCom->Bind_MeshBoneMatrices(m_pShaderCom, "g_BoneMatrices", (_uint)i);

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", TEXTURE_TYPE::DIFFUSE, (_uint)i)))
			return E_FAIL;


		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render((_uint)i)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CLink::Ready_Components()
{
	__super::Ready_Components();

	if(m_iLevelIndex == LEVEL_MARINHOUSE)

	{
		/* For.Com_Navigation */
		CNavigation::NAVIGATION_DESC			NaviDesc{};

		NaviDesc.iCurrentIndex = 0;

		if (FAILED(__super::Add_Component(m_iLevelIndex, TEXT("Prototype_Component_Navigation"),
			TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &NaviDesc)))
			return E_FAIL;
	}

	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Link"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
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

}
