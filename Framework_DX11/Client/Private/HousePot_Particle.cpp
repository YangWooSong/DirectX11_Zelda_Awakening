#include "stdafx.h"
#include "..\Public\HousePot_Particle.h"

#include "GameInstance.h"
#include "VIBuffer_Model_Instance.h"
#include "Mesh.h"

CHousePot_Particle::CHousePot_Particle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CHousePot_Particle::CHousePot_Particle(const CHousePot_Particle& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CHousePot_Particle::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CHousePot_Particle::Initialize(void* pArg)
{
	/* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize()))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CHousePot_Particle::Priority_Update(_float fTimeDelta)
{

}

void CHousePot_Particle::Update(_float fTimeDelta)
{
	m_pVIBufferCom->Spread(fTimeDelta);

}

void CHousePot_Particle::Late_Update(_float fTimeDelta)
{
	/* 직교투영을 위한 월드행렬까지 셋팅하게 된다. */
	__super::Late_Update(fTimeDelta);


	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONLIGHT, this);
}

HRESULT CHousePot_Particle::Render()
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
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", TEXTURE_TYPE::DIFFUSE, i)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pVIBufferCom->Bind_Buffers()))
			return E_FAIL;

		if (FAILED(m_pVIBufferCom->Render()))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CHousePot_Particle::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxModelInstance"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_HousePot_Particle"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	CVIBuffer_Model_Instance::MODEL_INSTANCE_DESC Desc{};

	Desc.iNumVertices = m_pModelCom->Get_Mesh(0)->Get_NumVertices();
	Desc.iNumIndices = m_pModelCom->Get_Mesh(0)->Get_NumIndices();
	Desc.pVB = m_pModelCom->Get_Mesh(0)->Get_VB();
	Desc.pIB = m_pModelCom->Get_Mesh(0)->Get_IB();

	/* FOR.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Model_Instance"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), &Desc)))
		return E_FAIL;

	return S_OK;
}

CHousePot_Particle* CHousePot_Particle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CHousePot_Particle* pInstance = new CHousePot_Particle(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CHousePot_Particle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CHousePot_Particle::Clone(void* pArg)
{
	CHousePot_Particle* pInstance = new CHousePot_Particle(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CHousePot_Particle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHousePot_Particle::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pModelCom);
}
