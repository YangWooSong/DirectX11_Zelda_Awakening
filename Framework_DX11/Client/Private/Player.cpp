#include "stdafx.h"
#include "Player.h"

#include "GameInstance.h"

CPlayer::CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CContainerObject{ pDevice, pContext }
{
}

CPlayer::CPlayer(const CPlayer& Prototype)
	: CContainerObject{ Prototype }
{
}

HRESULT CPlayer::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayer::Initialize(void* pArg)
{
	PLAYER_DESC* pDesc = static_cast<PLAYER_DESC*>(pArg);

	/* ���������� ���� �����͵��� ��� ���ϳ�. */
	if (FAILED(__super::Initialize(&pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (nullptr != pDesc)
	{
		m_pTransformCom->Set_Scaled(pDesc->vScale.x, pDesc->vScale.y, pDesc->vScale.z);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&pDesc->vPosition));
		m_iLevelIndex = pDesc->LevelIndex;
	}

	return S_OK;
}

void CPlayer::Priority_Update(_float fTimeDelta)
{
}

void CPlayer::Update(_float fTimeDelta)
{
	m_pModelCom->Play_Animation(fTimeDelta);
}

void CPlayer::Late_Update(_float fTimeDelta)
{
	/* ���������� ���� ������ı��� �����ϰ� �ȴ�. */
	__super::Late_Update(fTimeDelta);

	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CPlayer::Render()
{
	return S_OK;
}

HRESULT CPlayer::Ready_PartObjects()
{
	/* ���� �߰��ϰ� ���� ��Ʈ������Ʈ�� ������ŭ ����� �����س���. */
	/*m_Parts.resize(PART_END);

	CBody_Player::BODY_DESC		BodyDesc{};
	BodyDesc.pParentState = &m_iState;
	BodyDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
	if (FAILED(__super::Add_PartObject(PART_BODY, TEXT("Prototype_GameObject_Body_Player"), &BodyDesc)))
		return E_FAIL;

	CWeapon::WEAPON_DESC		WeaponDesc{};
	WeaponDesc.pParentState = &m_iState;
	WeaponDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
	WeaponDesc.pSocketBoneMatrix = dynamic_cast<CBody_Player*>(m_Parts[PART_BODY])->Get_BoneMatrix_Ptr("SWORD");

	if (FAILED(__super::Add_PartObject(PART_WEAPON, TEXT("Prototype_GameObject_Weapon"), &WeaponDesc)))
		return E_FAIL;*/

	return S_OK;
}

void CPlayer::Move_Dir(_fvector vDir, _float fSpeed, _float fTimeDelta)
{
	m_pTransformCom->Turn_Lerp(vDir, 2.0f, fTimeDelta);
	m_pTransformCom->Go_Straight(fSpeed, fTimeDelta);
}
void CPlayer::Change_State(const _uint iState)
{
	m_pFsmCom->Change_State(iState);
}

void CPlayer::SetUp_NextAnimation(_uint iNextAnimationIndex, _float fChangeDuration, _bool _bLoop)
{
	m_pModelCom->SetUp_NextAnimation(iNextAnimationIndex, fChangeDuration, _bLoop);
}

_bool CPlayer::Get_IsEnd_CurrentAnimation()
{
	return m_pModelCom->Get_IsEnd_CurrentAnimation();
}

void CPlayer::Go_Straight(_float fTimeDelta, _float fSpeed, class CNavigation* pNavigation )
{
	m_pTransformCom->Go_Straight(fTimeDelta, fSpeed, pNavigation);
}

void CPlayer::Go_Backward(_float fTimeDelta, _float fSpeed)
{
	m_pTransformCom->Go_Backward(fTimeDelta, fSpeed);
}

void CPlayer::Go_Right(_float fTimeDelta, _float fSpeed)
{
	m_pTransformCom->Go_Right(fTimeDelta, fSpeed);
}

void CPlayer::Go_Left(_float fTimeDelta, _float fSpeed)
{
	m_pTransformCom->Go_Left(fTimeDelta, fSpeed);
}

void CPlayer::Go_World_Straight(_float fTimeDelta, _float fSpeed, class CNavigation* pNavigation)
{
	m_pTransformCom->Go_World_Straight(fTimeDelta, fSpeed, pNavigation);
}

void CPlayer::Go_World_Backward(_float fTimeDelta, _float fSpeed, class CNavigation* pNavigation)
{
	m_pTransformCom->Go_World_Backward(fTimeDelta, fSpeed, pNavigation);
}

void CPlayer::Go_World_Right(_float fTimeDelta, _float fSpeed, class CNavigation* pNavigation)
{
	m_pTransformCom->Go_World_Right(fTimeDelta, fSpeed, pNavigation);
}

void CPlayer::Go_World_Left(_float fTimeDelta, _float fSpeed, class CNavigation* pNavigation)
{
	m_pTransformCom->Go_World_Left(fTimeDelta, fSpeed, pNavigation);
}

void CPlayer::Go_Stair_Down(_float fTimeDelta, _float fSpeed)
{
	
	_vector vDir = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	vDir = XMVector3Normalize(vDir);

	_vector newPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + vDir * fSpeed;
	_float f = XMVectorGetY(newPos) - fSpeed;
	newPos = XMVectorSetY(newPos, f);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, newPos);
}

void CPlayer::Go_Stair_Up(_float fTimeDelta, _float fSpeed)
{
	_vector vDir = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	vDir = XMVector3Normalize(vDir);

	_vector newPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + vDir * fSpeed;
	_float f = XMVectorGetY(newPos) + fSpeed;
	newPos = XMVectorSetY(newPos, f);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, newPos);
}

_vector CPlayer::Calculate_Direction_Straight()
{
	_vector vLook = m_pTransformCom->Get_State(CTransform::STATE::STATE_LOOK);
	_vector vUp = m_pTransformCom->Get_State(CTransform::STATE::STATE_UP);
	vUp = XMVector3Normalize(vUp);

	_vector vRight = XMVector3Cross(vUp, vLook);
	_vector vDir = XMVector3Cross(vRight, vUp);

	vDir = XMVector3Normalize(vDir);

	return vDir;
}

_uint CPlayer::Get_Player_State()
{
	return m_pFsmCom->Get_CurrentState();
}

const _float4x4* CPlayer::Get_BoneMatrix_Ptr(const _char* pBoneName) const
{
	return m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(pBoneName);
}

HRESULT CPlayer::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;
	return S_OK;

}

CPlayer* CPlayer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPlayer* pInstance = new CPlayer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CPlayer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CPlayer::Clone(void* pArg)
{
	CPlayer* pInstance = new CPlayer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CPlayer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
