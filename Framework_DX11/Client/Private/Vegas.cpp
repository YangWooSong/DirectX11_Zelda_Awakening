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

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_State()))
		return E_FAIL;

	//Read한 정보 세팅
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&pDesc->vPosition));
	//m_pTransformCom->Set_Scaled(pDesc->vScale.x, pDesc->vScale.y, pDesc->vScale.z);
	//m_pTransformCom->RotationThreeAxis(pDesc->vRotation);
	//m_vRot = pDesc->vRotation;
	//m_pModelCom->SetUp_Animation(30, true);
	m_pFsmCom->Set_State(IDLE);
	m_pModelCom->Add_Texture_to_Material(TEXT("../Bin/ModelData/Anim/Monster/Vegas/MI_body_alb.1.dds"), TEXTURE_TYPE::DIFFUSE, 0);
	m_pModelCom->Add_Texture_to_Material(TEXT("../Bin/ModelData/Anim/Monster/Vegas/MI_body_alb.2.dds"), TEXTURE_TYPE::DIFFUSE, 0);
	m_pModelCom->Add_Texture_to_Material(TEXT("../Bin/ModelData/Anim/Monster/Vegas/MI_body_alb.3.dds"), TEXTURE_TYPE::DIFFUSE, 0);

	m_iTextureNum = 0;
	m_eObjType = CGameObject::ANIM_MONSTER;
	return S_OK;
}


void CVegas::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CVegas::Update(_float fTimeDelta)
{
	m_fTimer += fTimeDelta;

	if (m_pFsmCom->Get_CurrentState() != DEAD && m_fTimer > 0.7f)
	{
		m_iTextureNum++;
		m_fTimer = 0;
		if (m_iTextureNum >= 4)
			m_iTextureNum = 0;
	}
	m_pFsmCom->Update(fTimeDelta);
	m_pModelCom->Play_Animation(fTimeDelta);

	__super::Update(fTimeDelta);
}

void CVegas::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}


HRESULT CVegas::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_bIsDead", &m_isDead, sizeof(_bool))))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pModelCom->Bind_MeshBoneMatrices(m_pShaderCom, "g_BoneMatrices", (_uint)i);

		//요기에 인덱스 넣자
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", TEXTURE_TYPE::DIFFUSE, (_uint)i, m_iTextureNum)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render((_uint)i)))
			return E_FAIL;
	}

	//다른 모델한테 영향이 가면 안되서 dead처리를 풀어줘야 함
	_bool bFalse = false;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_bIsDead", &bFalse, sizeof(_bool))))
		return E_FAIL;

	return S_OK;
}

HRESULT CVegas::Ready_Components()
{
	__super::Ready_Components();

	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_TEST, TEXT("Prototype_Component_Model_Vegas0"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;


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
}


