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
	m_pParentMatrix = pDesc->pParentMatrix;
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
	m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(m_pParentMatrix));

	switch (m_iParticleType)
	{
	case CROSS_MINI:
		m_pVIBufferCom->Spread(fTimeDelta);
		break;
	case BOMB_SMOKE:
		m_pVIBufferCom->Spread(fTimeDelta);
		break;
	case PLAYER_ITEM_GET:
		m_pVIBufferCom->Spread(fTimeDelta);
		break;
	case GLOW:
		m_fColor = { 0.8f,0.8f,1.f ,1.f};
		m_pVIBufferCom->Spread(fTimeDelta);
		break;
	case SHUTTER_DUST:
		m_fColor = { 0.5f,0.5f,0.1f,0.2f };
		m_bSetAlpha = true;
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
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONLIGHT, this);
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
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fColor", &m_fColor, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_bSetAlpha", &m_bSetAlpha, sizeof(_bool))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;


	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	_float4 fcolor = { 1.f,1.f,1.f ,1.f };
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fColor", &fcolor, sizeof(_float4))))
		return E_FAIL;
	_bool bFalse = false;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_bSetAlpha", &bFalse, sizeof(_bool))))
		return E_FAIL;

	return S_OK;
}

HRESULT CParticle_Image::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPointInstance"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	CVIBuffer_Instancing::INSTANCE_DESC Desc{};

	if (m_iParticleType == CROSS_MINI)
	{
		/* FOR.Com_Texture */
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Cross_Mini"),
			TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;

		Desc.iNumInstance = 10;
		Desc.vCenter = _float3(0.f, 0.15f, 0.f);
		Desc.vRange = _float3(0.3f, 0.3f, 0.2f);
		Desc.vSize = _float2(0.2f, 0.4f);
		Desc.vPivot = _float3(0.f, 0.25f, 0.f);
		Desc.vSpeed = _float2(0.4f, 2.f);
		Desc.vLifeTime = _float2(0.4f, 2.f);
		Desc.isLoop = false;
	}
	else if (m_iParticleType == PLAYER_ITEM_GET)
	{
		/* FOR.Com_Texture */
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Cross_Color"),
			TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;

		Desc.iNumInstance = 15;
		Desc.vCenter = _float3(0.f, 1.5f, 0.f);
		Desc.vRange = _float3(0.5f, 0.5f, 0.2f);
		Desc.vSize = _float2(0.2f, 0.5f);
		Desc.vPivot = Desc.vCenter;
		Desc.vSpeed = _float2(0.5f, 1.5f);
		Desc.vLifeTime = _float2(0.4f,1.5f);
		Desc.isLoop = true;
	}
	else if (m_iParticleType == GLOW)
	{
		/* FOR.Com_Texture */
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_point_Glow"),
			TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;

		Desc.iNumInstance = 20;
		Desc.vCenter = _float3(0.f, 0.f, 0.f);
		Desc.vRange = _float3(0.5f, 0.5f, 0.2f);
		Desc.vSize = _float2(0.1f, 0.4f);
		Desc.vPivot = _float3(0.f, -0.2f, 0.f);
		Desc.vSpeed = _float2(0.3f, 0.6f);
		Desc.vLifeTime = _float2(0.4f, 0.8f);
		Desc.isLoop = false;
	}
	else if (m_iParticleType == SHUTTER_DUST)
	{
		/* FOR.Com_Texture */
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Smoke"),
			TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;

		Desc.iNumInstance = 8;
		Desc.vCenter = _float3(0.f, -0.25f, 0.f);
		Desc.vRange = _float3(1.2f, 0.2f, 0.2f);
		Desc.vSize = _float2(1.2f, 1.2f);
		Desc.vPivot = _float3(0.f, -1.f, 0.f);
		Desc.vSpeed = _float2(0.2f, 0.2f);
		Desc.vLifeTime = _float2(2.2f, 2.2f);
		Desc.isLoop = false;
	}
	/* FOR.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_PointInstance"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), &Desc)))
		return E_FAIL;

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
