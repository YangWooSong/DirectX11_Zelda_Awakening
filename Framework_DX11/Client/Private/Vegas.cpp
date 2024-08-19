#include "stdafx.h"
#include "Vegas.h"
#include "GameInstance.h"
#include "Shader.h"
#include "Model.h"
#include "State_Vegas_Idle.h"

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
	/* ���������� ���� �����͵��� ��� ���ϳ�. */
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_State()))
		return E_FAIL;

	//Read�� ���� ����
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&pDesc->vPosition));
	//m_pTransformCom->Set_Scaled(pDesc->vScale.x, pDesc->vScale.y, pDesc->vScale.z);
	//m_pTransformCom->RotationThreeAxis(pDesc->vRotation);
	//m_vRot = pDesc->vRotation;

	//m_pModelCom->SetUp_Animation(30, true);
	m_pFsmCom->Set_State(IDLE);

	return S_OK;
}


void CVegas::Priority_Update(_float fTimeDelta)
{
}

void CVegas::Update(_float fTimeDelta)
{
	m_pFsmCom->Update(fTimeDelta);

	m_pModelCom->Play_Animation(fTimeDelta);
}

void CVegas::Late_Update(_float fTimeDelta)
{
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

	/*_uint		iNumMeshes = m_pModelComs[m_iModelIndex]->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pModelComs[m_iModelIndex]->Bind_MeshBoneMatrices(m_pShaderCom, "g_BoneMatrices", (_uint)i);

		if (FAILED(m_pModelComs[m_iModelIndex]->Bind_Material(m_pShaderCom, "g_DiffuseTexture", TEXTURE_TYPE::DIFFUSE, (_uint)i)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pModelComs[m_iModelIndex]->Render((_uint)i)))
			return E_FAIL;
	}*/

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

HRESULT CVegas::Ready_Components()
{
	__super::Ready_Components();

	/* FOR.Com_Model */
	//if (FAILED(__super::Add_Component(LEVEL_FIELD, TEXT("Prototype_Component_Model_Vegas0"),
	//	TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelComs[0]))))
	//	return E_FAIL;

	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_FIELD, TEXT("Prototype_Component_Model_Vegas1"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	///* FOR.Com_Model */
	//if (FAILED(__super::Add_Component(LEVEL_FIELD, TEXT("Prototype_Component_Model_Vegas2"),
	//	TEXT("Com_Model2"), reinterpret_cast<CComponent**>(&m_pModelComs[2]))))
	//	return E_FAIL;

	///* FOR.Com_Model */
	//if (FAILED(__super::Add_Component(LEVEL_FIELD, TEXT("Prototype_Component_Model_Vegas3"),
	//	TEXT("Com_Model3"), reinterpret_cast<CComponent**>(&m_pModelComs[3]))))
	//	return E_FAIL;

	return S_OK;
}


HRESULT CVegas::Ready_State()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Fsm"),
		TEXT("Com_Fsm"), reinterpret_cast<CComponent**>(&m_pFsmCom), this)))
		return E_FAIL;

	m_pFsmCom->Add_State(CState_Vegas_Idle::Create(m_pFsmCom, this, IDLE));
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


