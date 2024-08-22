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
	/* ���������� ���� �����͵��� ��� ���ϳ�. */
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_State()))
		return E_FAIL;

	//m_pModelCom->SetUp_Animation(30, true);
	m_pFsmCom->Set_State(IDLE);

	return S_OK;
}

void CRola::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CRola::Update(_float fTimeDelta)
{
	m_pFsmCom->Update(fTimeDelta);

	m_pModelCom->Play_Animation(fTimeDelta);

	//������ �ι� ������
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

	__super::Update(fTimeDelta);
}

void CRola::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CRola::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_bIsDead", &m_bBlink, sizeof(_bool))))
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

	//�ٸ� ������ ������ ���� �ȵǼ� deadó���� Ǯ����� ��
	_bool bFalse = false;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_bIsDead", &bFalse, sizeof(_bool))))
		return E_FAIL;

	return S_OK;
}


HRESULT CRola::Ready_Components()
{
	__super::Ready_Components();

	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_FIELD, TEXT("Prototype_Component_Model_Rola"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

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
}

