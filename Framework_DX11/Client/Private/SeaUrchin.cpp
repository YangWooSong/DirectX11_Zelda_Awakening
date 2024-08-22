#include "stdafx.h"
#include "SeaUrchin.h"
#include "GameInstance.h"
#include "Shader.h"
#include "Model.h"
#include "State_SeaUrchin_Idle.h"
#include "State_SeaUrchin_Pushed.h"
#include "State_SeaUrchin_Dead.h"

CSeaUrchin::CSeaUrchin(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CMonster{ pDevice, pContext }
{
}

CSeaUrchin::CSeaUrchin(const CSeaUrchin& Prototype)
    : CMonster{ Prototype }
{
}

HRESULT CSeaUrchin::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CSeaUrchin::Initialize(void* pArg)
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

	m_fOrginScale = m_pTransformCom->Get_Scaled();

	m_fShrinklScale.x = m_fOrginScale.x / 1.3f;
	m_fShrinklScale.y = m_fOrginScale.y / 1.3f;
	m_fShrinklScale.z = m_fOrginScale.z / 1.3f;

	return S_OK;
}

void CSeaUrchin::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CSeaUrchin::Update(_float fTimeDelta)
{
	m_pFsmCom->Update(fTimeDelta);

	m_pModelCom->Play_Animation(fTimeDelta);

	if (m_isDead)
		Shrink(fTimeDelta);
	else
		m_pTransformCom->Set_Scaled(m_fOrginScale.x, m_fOrginScale.y, m_fOrginScale.z);

	__super::Update(fTimeDelta);
}

void CSeaUrchin::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CSeaUrchin::Render()
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

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", TEXTURE_TYPE::DIFFUSE, (_uint)i)))
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


HRESULT CSeaUrchin::Ready_Components()
{
	__super::Ready_Components();

	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_FIELD, TEXT("Prototype_Component_Model_SeaUrchin"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CSeaUrchin::Ready_State()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Fsm"),
		TEXT("Com_Fsm"), reinterpret_cast<CComponent**>(&m_pFsmCom), this)))
		return E_FAIL;

	m_pFsmCom->Add_State(CState_SeaUrchin_Idle::Create(m_pFsmCom, this, IDLE));
	m_pFsmCom->Add_State(CState_SeaUrchin_Pushed::Create(m_pFsmCom, this, PUSHED));
	m_pFsmCom->Add_State(CState_SeaUrchin_Dead::Create(m_pFsmCom, this, DEAD));
	return S_OK;
}

void CSeaUrchin::Shrink(_float fTimeDelta)
{
	m_fShrinkTime += fTimeDelta;

	if (m_fShrinkTime > 0.2f)
	{
		m_pTransformCom->Set_Scaled(m_fOrginScale.x, m_fOrginScale.y, m_fOrginScale.z );
		m_fShrinkTime = 0.f;
	}
	else if (m_fShrinkTime > 0.1f)
	{
		m_pTransformCom->Set_Scaled(m_fShrinklScale.x, m_fShrinklScale.y, m_fShrinklScale.z);
	}
}

CSeaUrchin* CSeaUrchin::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSeaUrchin* pInstance = new CSeaUrchin(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CSeaUrchin"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSeaUrchin::Clone(void* pArg)
{
	CSeaUrchin* pInstance = new CSeaUrchin(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CSeaUrchin"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSeaUrchin::Free()
{
	__super::Free();

	if (nullptr != m_pFsmCom)
		m_pFsmCom->Release_States();
	Safe_Release(m_pFsmCom);
}

