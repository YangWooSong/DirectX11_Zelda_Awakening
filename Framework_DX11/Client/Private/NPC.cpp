#include "stdafx.h"
#include "NPC.h"
#include "GameInstance.h"
#include "PartObject.h"
#include "UIObject.h"

CNPC::CNPC(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CContainerObject{ pDevice, pContext }
{
}

CNPC::CNPC(const CNPC& Prototype)
    : CContainerObject{ Prototype }
{
}

HRESULT CNPC::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CNPC::Initialize(void* pArg)
{
	CGameObject::GAMEOBJECT_DESC* pDesc = static_cast<GAMEOBJECT_DESC*>(pArg);

	/* 직교투영을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(&pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_eObjType = pDesc->eType;

	//m_pTransformCom->Set_Scaled(pDesc->vScale.x, pDesc->vScale.y, pDesc->vScale.z);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&pDesc->vPosition));
	

	return S_OK;
}


void CNPC::Priority_Update(_float fTimeDelta)
{
	for (auto& pPartObject : m_Parts)
		pPartObject->Priority_Update(fTimeDelta);

	for (auto& pNPCUI : m_NPCUI)
		pNPCUI->Priority_Update(fTimeDelta);
}

void CNPC::Update(_float fTimeDelta)
{
	m_pModelCom->Play_Animation(fTimeDelta);
	m_pSoundCom->Update(fTimeDelta);

	for (auto& pPartObject : m_Parts)
		pPartObject->Update(fTimeDelta);

	for (auto& pNPCUI : m_NPCUI)
		pNPCUI->Update(fTimeDelta);
}


void CNPC::Late_Update(_float fTimeDelta)
{	/* 직교투영을 위한 월드행렬까지 셋팅하게 된다. */
	__super::Late_Update(fTimeDelta);

	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
	m_pGameInstance->Add_RenderObject(CRenderer::RG_SHADOWOBJ, this);

	for (auto& pPartObject : m_Parts)
		pPartObject->Late_Update(fTimeDelta);

	for (auto& pNPCUI : m_NPCUI)
		pNPCUI->Late_Update(fTimeDelta);
}

HRESULT CNPC::Render()
{

	for (auto& pPartObject : m_Parts)
		pPartObject->Render();

	for (auto& pNPCUI : m_NPCUI)
		pNPCUI->Render();

    return S_OK;
}

HRESULT CNPC::Render_LightDepth()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldMatrix_Ptr())))
		return E_FAIL;

	_float4x4		ViewMatrix;
	XMStoreFloat4x4(&ViewMatrix, XMMatrixLookAtLH(XMVectorSet(0.f, 20.f, -15.f, 1.f), XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 1.f, 0.f, 0.f)));

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pModelCom->Bind_MeshBoneMatrices(m_pShaderCom, "g_BoneMatrices", (_uint)i);

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", TEXTURE_TYPE::DIFFUSE, (_uint)i)))
			return E_FAIL;
		/*if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", aiTextureType_NORMALS, i)))
		return E_FAIL;*/

		if (FAILED(m_pShaderCom->Begin(6)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render((_uint)i)))
			return E_FAIL;
	}

	return S_OK;
}

void CNPC::Change_State(const _uint iState)
{
	m_pFsmCom->Change_State(iState);
}

void CNPC::SetUp_NextAnimation(_uint iNextAnimationIndex, _float fChangeDuration, _bool _bLoop)
{
	m_pModelCom->SetUp_NextAnimation(iNextAnimationIndex, fChangeDuration, _bLoop);
}

_bool CNPC::Get_IsEnd_CurrentAnimation()
{
	return m_pModelCom->Get_IsEnd_CurrentAnimation();
}

HRESULT CNPC::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Sound */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sound"),
		TEXT("Com_Sound0"), reinterpret_cast<CComponent**>(&m_pSoundCom))))
		return E_FAIL;
	m_pSoundCom->Set_Owner(this);

    return S_OK;
}

HRESULT CNPC::Ready_PartObjects()
{
    return S_OK;
}

CNPC* CNPC::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CNPC* pInstance = new CNPC(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CNPC"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CNPC::Clone(void* pArg)
{
	CNPC* pInstance = new CNPC(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CNPC"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNPC::Free()
{
	__super::Free();

	for (auto& pNPCUI : m_NPCUI)
		Safe_Release(pNPCUI);

	m_NPCUI.clear();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pSoundCom);
	Safe_Release(m_pColliderCom);

}
