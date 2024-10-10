#include "stdafx.h"
#include "TailCaveShutter.h"
#include "GameInstance.h"
#include "PlayerCamera.h"
#include "Particle_Image.h"
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
	m_iOpenAnimIndex = m_pModelCom->Get_AnimationIndex("open");
	m_pModelCom->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.1f, false);

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
	if (m_bOpen)
	{
		m_fOpenedTimer += fTimeDelta;

		if (m_bPlaySound && m_pSoundCom->Get_IsPlaying() == false && m_bCameraBackOrigin == false)
			Camera_Set_to_Origin();

		if (m_bCameraSetting == false)
		{
			Camera_Set();
		}

		if (m_iCurrentAnimIndex != m_iOpenAnimIndex &&  m_fOpenedTimer > 1.f) //문 열리는 애니메이션으로 변경
		{
			m_bChangeAnim = true;
			m_iCurrentAnimIndex = m_iOpenAnimIndex;
			m_pModelCom->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.1f, false);
			CPlayerCamera* pCamera = static_cast<CPlayerCamera*>(m_pGameInstance->Find_Camera(LEVEL_FIELD));
			pCamera->Start_Shake(0.2f, 0.06f, 3.f);
		}
		if (m_iCurrentAnimIndex == m_iOpenAnimIndex && m_fOpenedTimer > 1.3f) //먼지 이펙트 실행
		{
			m_bActiveEffect = true;
			m_pEffect->Update(fTimeDelta);
		}
		if (m_iCurrentAnimIndex == m_iOpenAnimIndex && m_pModelCom->Get_IsEnd_CurrentAnimation() && m_bPlaySound == false && m_pSoundCom->Get_IsPlaying() == false)
		{
			m_bPlaySound = true;
			m_pSoundCom->Play_Sound(TEXT("5_GimmickSolve.wav"), 0.8f);
			m_pEffect->SetActive(false);
		}
	}
	m_pModelCom->Play_Animation(fTimeDelta);
	m_pSoundCom->Update(fTimeDelta);
}

void CTailCaveShutter::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
	if (m_bActiveEffect)
	{
		m_pEffect->Late_Update(fTimeDelta);
	}
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

	/* FOR.Com_Sound */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sound"),
		TEXT("Com_Sound"), reinterpret_cast<CComponent**>(&m_pSoundCom))))
		return E_FAIL;
	m_pSoundCom->Set_Owner(this);

	CGameObject* pGameObj = m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Particle_Image"));
	if (pGameObj != nullptr)
	{
		CParticle_Image::IMAGE_PARTICLE_DESC pImageDesc = {};
		pImageDesc.iParticleType = CParticle_Image::SHUTTER_DUST;
		pImageDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
		m_pEffect = dynamic_cast<CGameObject*>(pGameObj->Clone(&pImageDesc));
	}


	return S_OK;
}

void CTailCaveShutter::Camera_Set()
{
	m_pSoundCom->Play_Sound(TEXT("4_Obj_TailCaveShutter_Open.wav"), 1.f);

	m_bCameraSetting = true;

	CPlayerCamera* pCamera = static_cast<CPlayerCamera*>(m_pGameInstance->Find_Camera(LEVEL_FIELD));
	pCamera->Set_FollowPlayer(false);
	pCamera->Set_TargetToOtherPos(m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMLoadFloat3(&pCamera->Get_Offset()));


}

void CTailCaveShutter::Camera_Set_to_Origin()
{
	m_bCameraBackOrigin = true;
	m_pGameInstance->Pause_BGM();

	CPlayerCamera* pCamera = static_cast<CPlayerCamera*>(m_pGameInstance->Find_Camera(LEVEL_FIELD));
	pCamera->Set_FollowPlayer(true);
	pCamera->Zoom_Out(1.3f, 45.f);
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

	Safe_Release(m_pEffect);
	Safe_Release(m_pSoundCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}

