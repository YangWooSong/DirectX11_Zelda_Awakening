#include "stdafx.h"
#include "SeaUrchin.h"
#include "GameInstance.h"
#include "Shader.h"
#include "Model.h"
#include "Link.h"
#include "State_SeaUrchin_Idle.h"
#include "State_SeaUrchin_Pushed.h"
#include "State_SeaUrchin_Dead.h"

CSeaUrchin::CSeaUrchin(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CMonster{ pDevice, pContext }
{
}

CSeaUrchin::CSeaUrchin(const CSeaUrchin& Prototype)
    : CMonster{ Prototype }
{
}

HRESULT CSeaUrchin::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CSeaUrchin::Initialize(void* pArg)
{

	/* 직교투영을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_State()))
		return E_FAIL;

	//Read한 정보 세팅
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&pDesc->vPosition));
	//m_pTransformCom->Set_Scaled(pDesc->vScale.x, pDesc->vScale.y, pDesc->vScale.z);
	//m_pTransformCom->RotationThreeAxis(pDesc->vRotation);
	//m_vRot = pDesc->vRotation;

	//m_pModelCom->SetUp_Animation(30, true);
	m_pFsmCom->Set_State(IDLE);

	m_fOrginScale = m_pTransformCom->Get_Scaled();

	m_fShrinklScale.x = m_fOrginScale.x / 1.3f;
	m_fShrinklScale.y = m_fOrginScale.y / 1.3f;
	m_fShrinklScale.z = m_fOrginScale.z / 1.3f;

	m_eObjType = CGameObject::ANIM_MONSTER;
	return S_OK;
}

void CSeaUrchin::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CSeaUrchin::Update(_float fTimeDelta)
{
	m_pFsmCom->Update(fTimeDelta);

	m_pModelCom->Play_Animation(fTimeDelta);

	if (m_isDead)
	{
		Shrink(fTimeDelta);
		m_bBodyRed = true;
	}
	else
		m_pTransformCom->Set_Scaled(m_fOrginScale.x, m_fOrginScale.y, m_fOrginScale.z);

	__super::Update(fTimeDelta);

	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_Ptr());
}

void CSeaUrchin::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	m_pGameInstance->Add_ColliderList(m_pColliderCom);
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CSeaUrchin::Render()
{
	if(m_bRender)
	{
		if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_bIsRed", &m_bBodyRed, sizeof(_bool))))
			return E_FAIL;

		_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

		for (size_t i = 0; i < iNumMeshes; i++)
		{
			m_pModelCom->Bind_MeshBoneMatrices(m_pShaderCom, "g_BoneMatrices", (_uint)i);

			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", TEXTURE_TYPE::DIFFUSE, (_uint)i)))
				return E_FAIL;


			if (FAILED(m_pShaderCom->Begin(1)))
				return E_FAIL;

			if (FAILED(m_pModelCom->Render((_uint)i)))
				return E_FAIL;
		}

		_bool bFalse = { false };
		if (FAILED(m_pShaderCom->Bind_RawValue("g_bIsRed", &bFalse, sizeof(_bool))))
			return E_FAIL;
	}


#ifdef _DEBUG
	m_pColliderCom->Render();
#endif

	return S_OK;
}

void CSeaUrchin::OnCollisionEnter(CGameObject* pOther)
{
	if (m_pColliderCom->Get_IsColl())
	{
		if (pOther->Get_LayerTag() == TEXT("Layer_Sword"))
		{
			Change_State(DEAD);
		}
	}
}

void CSeaUrchin::OnCollisionStay(CGameObject* pOther)
{
	if (m_pColliderCom->Get_IsColl())
	{
		if (pOther->Get_LayerTag() == TEXT("Layer_Shield"))
		{
			if(m_pFsmCom->Get_CurrentState() != PUSHED)
				Change_State(PUSHED);
		}


		if (pOther->Get_LayerTag() == TEXT("Layer_Monster"))
		{
			_vector newLook = pOther->Get_Transform()->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			newLook = XMVector3Normalize(newLook);

			_vector newBodyPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + -newLook * 0.02f;
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, newBodyPos);
		}
		
	}
}

void CSeaUrchin::OnCollisionExit(CGameObject* pOther)
{
	if(m_pColliderCom->Get_IsColl() == false)
	{
		if (pOther->Get_LayerTag() == TEXT("Layer_Shield"))
		{
			Change_State(IDLE);
		}
	}

}


HRESULT CSeaUrchin::Ready_Components()
{
	__super::Ready_Components();

	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_FIELD, TEXT("Prototype_Component_Model_SeaUrchin"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Navigation */
	CNavigation::NAVIGATION_DESC			NaviDesc{};
	NaviDesc.iOwnerType = CNavigation::NONPLAYER;
	NaviDesc.iCurrentIndex = m_iCellNum;

	if (FAILED(__super::Add_Component(LEVEL_FIELD, TEXT("Prototype_Component_Navigation"),
		TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &NaviDesc)))
		return E_FAIL;

	/* For.Com_Collider */
	CBounding_AABB::BOUNDING_AABB_DESC			ColliderDesc{};
	ColliderDesc.vExtents = _float3(0.6f, 0.6f, 0.6f);
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vExtents.y, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;
	m_pColliderCom->Set_Owner(this);

	/* FOR.Com_Sound */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sound"),
		TEXT("Com_Sound"), reinterpret_cast<CComponent**>(&m_pMonsterSoundCom))))
		return E_FAIL;
	m_pMonsterSoundCom->Set_Owner(this);

	return S_OK;
}

HRESULT CSeaUrchin::Ready_State()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Fsm"),
		TEXT("Com_Fsm"), reinterpret_cast<CComponent**>(&m_pFsmCom), this)))
		return E_FAIL;

	m_pFsmCom->Add_State(CState_SeaUrchin_Idle::Create(m_pFsmCom, this, IDLE));
	m_pFsmCom->Add_State(CState_SeaUrchin_Pushed::Create(m_pFsmCom, this, PUSHED));
	m_pFsmCom->Add_State(CState_SeaUrchin_Dead::Create(m_pFsmCom, this, DEAD));
	return S_OK;
}

void CSeaUrchin::Shrink(_float fTimeDelta)
{
	m_fShrinkTime += fTimeDelta;

	if (m_fShrinkTime > 0.2f)
	{
		m_pTransformCom->Set_Scaled(m_fOrginScale.x, m_fOrginScale.y, m_fOrginScale.z );
		m_fShrinkTime = 0.f;
	}
	else if (m_fShrinkTime > 0.1f)
	{
		m_pTransformCom->Set_Scaled(m_fShrinklScale.x, m_fShrinklScale.y, m_fShrinklScale.z);
	}
}

CSeaUrchin* CSeaUrchin::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSeaUrchin* pInstance = new CSeaUrchin(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CSeaUrchin"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSeaUrchin::Clone(void* pArg)
{
	CSeaUrchin* pInstance = new CSeaUrchin(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CSeaUrchin"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSeaUrchin::Free()
{
	__super::Free();

	if (nullptr != m_pFsmCom)
		m_pFsmCom->Release_States();
	Safe_Release(m_pFsmCom);
	Safe_Release(m_pMonsterSoundCom);
}

