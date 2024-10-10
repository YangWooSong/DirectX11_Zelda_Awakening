#include "stdafx.h"
#include "..\Public\Player_3D_Effects.h"

#include "Player.h"
#include "Link.h"
#include "GameInstance.h"
#include "3D_Effects.h"

CPlayer_3D_Effects::CPlayer_3D_Effects(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{
}

CPlayer_3D_Effects::CPlayer_3D_Effects(const CPlayer_3D_Effects& Prototype)
	: CPartObject{ Prototype }
{
}

HRESULT CPlayer_3D_Effects::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayer_3D_Effects::Initialize(void* pArg)
{
	PLAYER_3D_DESC* pDesc = static_cast<PLAYER_3D_DESC*>(pArg);
	m_pSocketMatrix = pDesc->pSocketBoneMatrix;
//	m_pPlayerFsm = pDesc->pPlayerFsm;
//	m_pPlayerFsm->AddRef();
	
	/* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	//m_pTransformCom->Set_Scaled(1.f, 1.f, 2.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, -0.6f, 1.f));

	//Set_LayerTag(TEXT("Layer_Player_3D_Effects"));
	m_isActive = false;
	return S_OK;
}

void CPlayer_3D_Effects::Priority_Update(_float fTimeDelta)
{
}

void CPlayer_3D_Effects::Update(_float fTimeDelta)
{
	_matrix		SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

	for (size_t i = 0; i < 3; i++)
	{
		SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);
	}
	XMStoreFloat4x4(&m_WorldMatrix, XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()) * SocketMatrix * XMLoadFloat4x4(m_pParentMatrix));

	m_fAlpha = 0.6f;
	m_fBright = 1.5f;

	//상태에 따라 활성화 결정
}

void CPlayer_3D_Effects::Late_Update(_float fTimeDelta)
{
	/* 직교투영을 위한 월드행렬까지 셋팅하게 된다. */
	__super::Late_Update(fTimeDelta);
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONLIGHT, this);

}

HRESULT CPlayer_3D_Effects::Render()
{
	if (m_isActive)
	{
		if (FAILED(__super::Bind_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
			return E_FAIL;

		/*if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
			return E_FAIL;*/

		if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_fBright", &m_fBright, sizeof(_float))))
			return E_FAIL;

		_uint		iNumMeshes = m_pModelCom[m_iEffectType]->Get_NumMeshes();

		for (size_t i = 0; i < iNumMeshes; i++)
		{
			if (FAILED(m_pModelCom[m_iEffectType]->Bind_Material(m_pShaderCom, "g_DiffuseTexture", TEXTURE_TYPE::DIFFUSE, (_uint)i)))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Begin(6)))
				return E_FAIL;

			if (FAILED(m_pModelCom[m_iEffectType]->Render((_uint)i)))
				return E_FAIL;
		}

		_float fOne = 1.f;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &fOne, sizeof(_float))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_fBright", &fOne, sizeof(_float))))
			return E_FAIL;
	}
	return S_OK;
}

HRESULT CPlayer_3D_Effects::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxModel"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_swordslash_00"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom[SWISH]))))
		return E_FAIL;

	return S_OK;
}

CPlayer_3D_Effects* CPlayer_3D_Effects::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPlayer_3D_Effects* pInstance = new CPlayer_3D_Effects(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CPlayer_3D_Effects"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CPlayer_3D_Effects::Clone(void* pArg)
{
	CPlayer_3D_Effects* pInstance = new CPlayer_3D_Effects(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CPlayer_3D_Effects"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_3D_Effects::Free()
{
	__super::Free();

	for (int i = 0; i < TYPE_END-1; ++i)
		Safe_Release(m_pModelCom[i]);

//	Safe_Release(m_pPlayerFsm);
	Safe_Release(m_pShaderCom);
}
