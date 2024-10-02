#include "stdafx.h"
#include "..\Public\Particle_Model.h"

#include "GameInstance.h"
#include "VIBuffer_Model_Instance.h"
#include "Mesh.h"

CParticle_Model::CParticle_Model(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CParticle_Model::CParticle_Model(const CParticle_Model& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CParticle_Model::Initialize_Prototype()
{
	/* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CParticle_Model::Initialize(void* pArg)
{
	MODEL_PARTICLE_DESC* pDesc = static_cast<MODEL_PARTICLE_DESC*>(pArg);
	m_iParticleType = pDesc->iParticleType;

	/* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize()))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	return S_OK;
}

void CParticle_Model::Priority_Update(_float fTimeDelta)
{

}

void CParticle_Model::Update(_float fTimeDelta)
{
	switch (m_iParticleType)
	{
	case PURPLEQUARTZ:
		static_cast<CVIBuffer_Model_Instance*>(m_pVIBufferCom)->PurpleQuartz_Spread(fTimeDelta);
		break;
	case HOUSEPOT:
		static_cast<CVIBuffer_Model_Instance*>(m_pVIBufferCom)->PurpleQuartz_Spread(fTimeDelta);
		break;
	case GRASS:
		static_cast<CVIBuffer_Model_Instance*>(m_pVIBufferCom)->Grass_Spread(fTimeDelta, 0.5f, 1.f);
	default:
		break;
	}
	
}

void CParticle_Model::Late_Update(_float fTimeDelta)
{
	/* 직교투영을 위한 월드행렬까지 셋팅하게 된다. */
	__super::Late_Update(fTimeDelta);


	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CParticle_Model::Render()
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

HRESULT CParticle_Model::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxModelInstance"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if(m_iParticleType == PURPLEQUARTZ)
	{
		/* FOR.Com_Model */
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Purplequartz_Particle"),
			TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
			return E_FAIL;

		CVIBuffer_Model_Instance::MODEL_INSTANCE_DESC Desc{};

		Desc.iNumVertices = m_pModelCom->Get_Mesh(0)->Get_NumVertices();
		Desc.iNumIndices = m_pModelCom->Get_Mesh(0)->Get_NumIndices();
		Desc.pVB = m_pModelCom->Get_Mesh(0)->Get_VB();
		Desc.pIB = m_pModelCom->Get_Mesh(0)->Get_IB();

		Desc.iNumInstance = 13;
		Desc.vCenter = _float3(0.f, 2.f, 0.f);
		Desc.vRange = _float3(0.6f, 2.f, 0.6f);
		Desc.vSize = _float2(0.5f, 1.2f);
		Desc.vPivot = _float3(0.f, 4.f, 0.f);
		Desc.vSpeed = _float2(3.f, 5.f);
		Desc.vLifeTime = _float2(0.5f, 1.f);
		Desc.isLoop = false;

		/* FOR.Com_VIBuffer */
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Model_Instance"),
			TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), &Desc)))
			return E_FAIL;
	}
	else if(m_iParticleType == HOUSEPOT)
	{
		/* FOR.Com_Model */
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_HousePot_Particle"),
			TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
			return E_FAIL;

		CVIBuffer_Model_Instance::MODEL_INSTANCE_DESC Desc{};

		Desc.iNumVertices = m_pModelCom->Get_Mesh(0)->Get_NumVertices();
		Desc.iNumIndices = m_pModelCom->Get_Mesh(0)->Get_NumIndices();
		Desc.pVB = m_pModelCom->Get_Mesh(0)->Get_VB();
		Desc.pIB = m_pModelCom->Get_Mesh(0)->Get_IB();

		Desc.iNumInstance = 13;
		Desc.vCenter = _float3(0.f, 1.5f, 0.f);
		Desc.vRange = _float3(1.f, 2.f, 1.f);
		Desc.vSize = _float2(0.2f, 0.8f);
		Desc.vPivot = _float3(0.f, 4.f, 0.f);
		Desc.vSpeed = _float2(3.f, 5.f);
		Desc.vLifeTime = _float2(0.5f, 1.f);
		Desc.isLoop = false;

		/* FOR.Com_VIBuffer */
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Model_Instance"),
			TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), &Desc)))
			return E_FAIL;
	}
	else if (m_iParticleType == ROCK)
	{
		/* FOR.Com_Model */
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_OctoRockRock_Particle"),
			TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
			return E_FAIL;

		CVIBuffer_Model_Instance::MODEL_INSTANCE_DESC Desc{};

		Desc.iNumVertices = m_pModelCom->Get_Mesh(0)->Get_NumVertices();
		Desc.iNumIndices = m_pModelCom->Get_Mesh(0)->Get_NumIndices();
		Desc.pVB = m_pModelCom->Get_Mesh(0)->Get_VB();
		Desc.pIB = m_pModelCom->Get_Mesh(0)->Get_IB();

		Desc.iNumInstance = 4;
		Desc.vCenter = _float3(0.f, 0.5f, 0.f);
		Desc.vRange = _float3(0.5f, 0.5f, 0.5f);
		Desc.vSize = _float2(0.1f, 0.1f);
		Desc.vPivot = _float3(0.f, 4.f, 0.f);
		Desc.vSpeed = _float2(1.f, 3.f);
		Desc.vLifeTime = _float2(0.3f, 1.f);
		Desc.isLoop = false;

		/* FOR.Com_VIBuffer */
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Model_Instance"),
			TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), &Desc)))
			return E_FAIL;
	}
	else if (m_iParticleType == GRASS)
	{
		/* FOR.Com_Model */
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Grass_Particle"),
			TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
			return E_FAIL;

		CVIBuffer_Model_Instance::MODEL_INSTANCE_DESC Desc{};

		Desc.iNumVertices = m_pModelCom->Get_Mesh(0)->Get_NumVertices();
		Desc.iNumIndices = m_pModelCom->Get_Mesh(0)->Get_NumIndices();
		Desc.pVB = m_pModelCom->Get_Mesh(0)->Get_VB();
		Desc.pIB = m_pModelCom->Get_Mesh(0)->Get_IB();

		Desc.iNumInstance = 10;
		Desc.vCenter = _float3(0.f, 0.5f, 0.f);
		Desc.vRange = _float3(1.f, 2.f, 1.f);
		Desc.vSize = _float2(0.3f, 1.f);
		Desc.vPivot = _float3(0.f, 0.5f, 0.f);
		Desc.vSpeed = _float2(1.f, 3.f);
		Desc.vLifeTime = _float2(0.3f, 1.f);
		Desc.isLoop = false;

		/* FOR.Com_VIBuffer */
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Model_Instance"),
			TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), &Desc)))
			return E_FAIL;
	}
	else if (m_iParticleType == LAWN)
	{
		/* FOR.Com_Model */
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_cut_lawn_Particle"),
			TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
			return E_FAIL;

		CVIBuffer_Model_Instance::MODEL_INSTANCE_DESC Desc{};

		Desc.iNumVertices = m_pModelCom->Get_Mesh(0)->Get_NumVertices();
		Desc.iNumIndices = m_pModelCom->Get_Mesh(0)->Get_NumIndices();
		Desc.pVB = m_pModelCom->Get_Mesh(0)->Get_VB();
		Desc.pIB = m_pModelCom->Get_Mesh(0)->Get_IB();

		Desc.iNumInstance = 4;
		Desc.vCenter = _float3(0.f, 0.5f, 0.f);
		Desc.vRange = _float3(0.5f, 0.5f, 0.5f);
		Desc.vSize = _float2(0.1f, 0.1f);
		Desc.vPivot = _float3(0.f, 4.f, 0.f);
		Desc.vSpeed = _float2(1.f, 3.f);
		Desc.vLifeTime = _float2(0.3f, 1.f);
		Desc.isLoop = false;

		/* FOR.Com_VIBuffer */
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Model_Instance"),
			TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), &Desc)))
			return E_FAIL;
			}
	

	return S_OK;
}

CParticle_Model* CParticle_Model::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CParticle_Model* pInstance = new CParticle_Model(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CParticle_Model"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CParticle_Model::Clone(void* pArg)
{
	CParticle_Model* pInstance = new CParticle_Model(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CParticle_Model"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CParticle_Model::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pModelCom);
}
