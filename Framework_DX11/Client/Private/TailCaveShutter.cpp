#include "stdafx.h"
#include "TailCaveShutter.h"
#include "GameInstance.h"

CTailCaveShutter::CTailCaveShutter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}

CTailCaveShutter::CTailCaveShutter(const CTailCaveShutter& Prototype)
    : CGameObject{ Prototype }
{
}

HRESULT CTailCaveShutter::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CTailCaveShutter::Initialize(void* pArg)
{
	GAMEOBJECT_DESC* pDesc = static_cast<GAMEOBJECT_DESC*>(pArg);

	/* 직교투영을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	m_iCurrentAnimIndex = m_pModelCom->Get_AnimationIndex("close_wait");
	m_pModelCom->SetUp_NextAnimation(m_iCurrentAnimIndex, true);

	//Read한 정보 세팅
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&pDesc->vPosition));
	m_pTransformCom->Set_Scaled(pDesc->vScale.x, pDesc->vScale.y, pDesc->vScale.z);
	m_pTransformCom->RotationThreeAxis(pDesc->vRotation);
	m_vRot = pDesc->vRotation;

	return S_OK;
}
void CTailCaveShutter::Priority_Update(_float fTimeDelta)
{
}

void CTailCaveShutter::Update(_float fTimeDelta)
{
	m_pModelCom->Play_Animation(fTimeDelta);
}

void CTailCaveShutter::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CTailCaveShutter::Render()
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

HRESULT CTailCaveShutter::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_FIELD, TEXT("Prototype_Component_Model_TailCaveShutter"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}


CTailCaveShutter* CTailCaveShutter::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTailCaveShutter* pInstance = new CTailCaveShutter(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CTailCaveShutter"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CTailCaveShutter::Clone(void* pArg)
{
	CTailCaveShutter* pInstance = new CTailCaveShutter(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CTailCaveShutter"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CTailCaveShutter::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}

