#include "stdafx.h"
#include "TailLockStatue.h"
#include "GameInstance.h"
#include "Link.h"
#include "PlayerCamera.h"
#include "TailCaveShutter.h"

CTailLockStatue::CTailLockStatue(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}

CTailLockStatue::CTailLockStatue(const CTailLockStatue& Prototype)
    : CGameObject{ Prototype }
{
}

HRESULT CTailLockStatue::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CTailLockStatue::Initialize(void* pArg)
{
	GAMEOBJECT_DESC* pDesc = static_cast<GAMEOBJECT_DESC*>(pArg);

	/* 직교투영을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_iCurrentAnimIndex = m_pModelCom->Get_AnimationIndex("unlock");
	m_pModelCom->SetUp_NextAnimation(m_iCurrentAnimIndex, true);

	//Read한 정보 세팅
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&pDesc->vPosition));
	m_pTransformCom->Set_Scaled(pDesc->vScale.x, pDesc->vScale.y, pDesc->vScale.z);
	m_pTransformCom->RotationThreeAxis(pDesc->vRotation);
	m_vRot = pDesc->vRotation;

	m_pGameInstance->AddScene_ColMesh(this, TEXT("TailLockStatue"));
	return S_OK;
}
void CTailLockStatue::Priority_Update(_float fTimeDelta)
{
}

void CTailLockStatue::Update(_float fTimeDelta)
{
	if (m_bOpened)
	{
		m_fOpenedTimer += fTimeDelta;

		if (m_bOpenSound == false) //처음으로 효과음 틀리
		{
			m_bOpenSound = true;
			m_pSoundCom->Play_Sound(TEXT("4_Obj_TailLockStatue_Open.wav"), 1.f);
		}

		if (m_fOpenedTimer > 1.6f) //자물쇠 말파 조정
			m_fAlpha -= fTimeDelta * 2.f;

		if (m_bCameraSet == false)	//자물쇠 열면서 카메라 세팅
			Camera_Set();

		if (m_pSoundCom->Get_IsPlaying() == false)
			m_fAfterSoundTimer += fTimeDelta;

		if (m_fAfterSoundTimer > 1.f)
			Shutter_Set();
	}
	m_pModelCom->Play_Animation(fTimeDelta);
	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_Ptr());
	m_pSoundCom->Update(fTimeDelta);
}

void CTailLockStatue::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_ColliderList(m_pColliderCom);
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);

#ifdef _DEBUG
	m_pGameInstance->Add_DebugObject(m_pColliderCom);
#endif
}

HRESULT CTailLockStatue::Render()
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
		if (i == 2)
		{
			if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float))))
				return E_FAIL;
		}
		else
		{
			_float fAlpha = 1.f;
			if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &fAlpha, sizeof(_float))))
				return E_FAIL;
		}

		m_pModelCom->Bind_MeshBoneMatrices(m_pShaderCom, "g_BoneMatrices", (_uint)i);

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", TEXTURE_TYPE::DIFFUSE, (_uint)i)))
			return E_FAIL;


		if (FAILED(m_pShaderCom->Begin(5)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render((_uint)i)))
			return E_FAIL;

	}

	return S_OK;
}

void CTailLockStatue::OnCollisionEnter(CGameObject* pOther)
{
}

void CTailLockStatue::OnCollisionStay(CGameObject* pOther)
{
	if (m_pColliderCom->Get_IsColl())
	{
		if (pOther->Get_LayerTag() == TEXT("Layer_Player"))
		{
			if (static_cast<CLink*>(pOther)->Get_Fsm()->Get_PrevState() == CLink::KEY && static_cast<CLink*>(pOther)->Get_Fsm()->Get_CurrentState() == CLink::IDLE)
				m_bOpened = true;
		}
	}
}

void CTailLockStatue::OnCollisionExit(CGameObject* pOther)
{
}

HRESULT CTailLockStatue::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_FIELD, TEXT("Prototype_Component_Model_TailLockStatue"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* FOR.Com_Sound */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sound"),
		TEXT("Com_Sound"), reinterpret_cast<CComponent**>(&m_pSoundCom))))
		return E_FAIL;
	m_pSoundCom->Set_Owner(this);

	/* For.Com_Collider */
	CBounding_AABB::BOUNDING_AABB_DESC			ColliderDesc{};
	ColliderDesc.vExtents = _float3(0.7f, 1.f, 0.8f);
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vExtents.y, -0.1f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;
	m_pColliderCom->Set_Owner(this);
    return S_OK;
}

void CTailLockStatue::Camera_Set()
{
	m_bCameraSet = true;

	m_pGameInstance->Pause_BGM();

	CPlayerCamera* pCamera = static_cast<CPlayerCamera*>(m_pGameInstance->Find_Camera(LEVEL_FIELD));
	pCamera->Set_FollowPlayer(false);
	pCamera->Set_TargetToOtherPos(m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMLoadFloat3(&pCamera->Get_Offset()));
	pCamera->Zoom_In(1.3f, 45.f);
}

void CTailLockStatue::Shutter_Set()
{
	CTailCaveShutter* pShutter = static_cast<CTailCaveShutter*>(m_pGameInstance->Find_Object(LEVEL_FIELD, TEXT("Layer_TailCaveShutter"), 0));
	pShutter->Set_Open(true);
}

CTailLockStatue* CTailLockStatue::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTailLockStatue* pInstance = new CTailLockStatue(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CTailLockStatue"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CTailLockStatue::Clone(void* pArg)
{
	CTailLockStatue* pInstance = new CTailLockStatue(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CTailLockStatue"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CTailLockStatue::Free()
{
	__super::Free();

	Safe_Release(m_pSoundCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pColliderCom);
}


