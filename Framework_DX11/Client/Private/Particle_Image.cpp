#include "stdafx.h"
#include "..\Public\Particle_Image.h"
#include "GameInstance.h"
#include "VIBuffer_Point_Instance.h"
#include "Mesh.h"

CParticle_Image::CParticle_Image(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CParticle_Image::CParticle_Image(const CParticle_Image& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CParticle_Image::Initialize_Prototype()
{
	/* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CParticle_Image::Initialize(void* pArg)
{
	IMAGE_PARTICLE_DESC* pDesc = static_cast<IMAGE_PARTICLE_DESC*>(pArg);
	m_iParticleType = pDesc->iParticleType;

	/* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize()))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	return S_OK;
}

void CParticle_Image::Priority_Update(_float fTimeDelta)
{

}

void CParticle_Image::Update(_float fTimeDelta)
{
	switch (m_iParticleType)
	{
	case CROSS_MINI:
		m_pVIBufferCom->Spread(fTimeDelta);
		break;
	default:
		break;
	}

}

void CParticle_Image::Late_Update(_float fTimeDelta)
{
	/* 직교투영을 위한 월드행렬까지 셋팅하게 된다. */
	__super::Late_Update(fTimeDelta);


	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CParticle_Image::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_ShadeResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition_Float4(), sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;


	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CParticle_Image::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxModelInstance"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;



	if (m_iParticleType == CROSS_MINI)
	{
		/* FOR.Com_Texture */
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Cross_Mini"),
			TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;


		CVIBuffer_Instancing::INSTANCE_DESC Desc{};

		Desc.iNumInstance = 50;
		Desc.vCenter = _float3(0.f, 0.5f, 0.f);
		Desc.vRange = _float3(5.f, 5.f, 5.f);
		Desc.vSize = _float2(1.f, 5.f);
		Desc.vPivot = _float3(0.f, -2.f, 0.f);
		Desc.vSpeed = _float2(3.f, 5.f);
		Desc.vLifeTime = _float2(0.5f, 1.f);
		Desc.isLoop = false;

		/* FOR.Com_VIBuffer */
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_PointInstance"),
			TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), &Desc)))
			return E_FAIL;
	}
	
	return S_OK;
}

CParticle_Image* CParticle_Image::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CParticle_Image* pInstance = new CParticle_Image(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CParticle_Image"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CParticle_Image::Clone(void* pArg)
{
	CParticle_Image* pInstance = new CParticle_Image(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CParticle_Image"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CParticle_Image::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
}
