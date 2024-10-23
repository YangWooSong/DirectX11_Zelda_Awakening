#include "stdafx.h"
#include "Rola_Hand_Effect.h"
#include "GameInstance.h"
#include "Particle_Image.h"
CRola_Hand_Effect::CRola_Hand_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{
}

CRola_Hand_Effect::CRola_Hand_Effect(const CRola_Hand_Effect& Prototype)
	: CPartObject{ Prototype }
{
}


HRESULT CRola_Hand_Effect::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CRola_Hand_Effect::Initialize(void* pArg)
{
	ROLA_HAND_DESC* pDesc = static_cast<ROLA_HAND_DESC*>(pArg);
	m_pSocketMatrix = pDesc->pSocketBoneMatrix;
	m_pParent = pDesc->m_pParent;

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_isActive = false;

	
	return S_OK;
}
void CRola_Hand_Effect::Priority_Update(_float fTimeDelta)
{
	if (m_isActive == false)
	{
		m_bCreateEffect = false;
		Safe_AddRef(m_pEffect);
	}
	else
	{
		if(m_bCreateEffect == false)
		{
			CGameObject* pGameObj = m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Particle_Image"));
			if (pGameObj != nullptr)
			{
				CParticle_Image::IMAGE_PARTICLE_DESC pImageDesc = {};
				pImageDesc.iParticleType = CParticle_Image::JUMP_DUST;
				pImageDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
				m_pEffect = dynamic_cast<CGameObject*>(pGameObj->Clone(&pImageDesc));

			}
			m_pEffect->SetActive(true);
			m_bCreateEffect = true;
		}
		
	}
	
}

void CRola_Hand_Effect::Update(_float fTimeDelta)
{
	if(m_isActive)
	{
		_vector newPos = m_pParent->Get_Transform()->Get_State(CTransform::STATE_POSITION);
		newPos = XMVectorSetY(newPos, XMVectorGetY(newPos) - 0.5f);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pParent->Get_Transform()->Get_State(CTransform::STATE_POSITION));
		if(m_pEffect != nullptr)
			m_pEffect->Update(fTimeDelta);
	}
}


void CRola_Hand_Effect::Late_Update(_float fTimeDelta)
{
	if(m_isActive)
	{
		if (m_pEffect != nullptr)
			m_pEffect->Late_Update(fTimeDelta);
	}
}

HRESULT CRola_Hand_Effect::Render()
{
	return S_OK;
}

HRESULT CRola_Hand_Effect::Ready_Components()
{
	CGameObject*  pGameObj = m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Particle_Image"));
	if (pGameObj != nullptr)
	{
		CParticle_Image::IMAGE_PARTICLE_DESC pImageDesc = {};
		pImageDesc.iParticleType = CParticle_Image::JUMP_DUST;
		pImageDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
	    m_pEffect = dynamic_cast<CGameObject*>(pGameObj->Clone(&pImageDesc));

	}
	return S_OK;
}

CRola_Hand_Effect* CRola_Hand_Effect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRola_Hand_Effect* pInstance = new CRola_Hand_Effect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CRola_Hand_Effect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CRola_Hand_Effect::Clone(void* pArg)
{
	CRola_Hand_Effect* pInstance = new CRola_Hand_Effect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CRola_Hand_Effect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRola_Hand_Effect::Free()
{
	__super::Free();

	Safe_Release(m_pEffect);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
