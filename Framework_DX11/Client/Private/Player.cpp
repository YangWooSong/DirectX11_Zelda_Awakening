#include "stdafx.h"
#include "Player.h"

#include "GameInstance.h"
#include "UIObject.h"
#include "Link.h"

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

	/* 직교투영을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(&pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (nullptr != pDesc)
	{
		m_pTransformCom->Set_Scaled(pDesc->vScale.x, pDesc->vScale.y, pDesc->vScale.z);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&pDesc->vPosition));
		m_pTransformCom->RotationThreeAxis(pDesc->vRotation);

		m_iLevelIndex = pDesc->LevelIndex;
		m_iCellNum = pDesc->iStartCellNum;
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
	/* 직교투영을 위한 월드행렬까지 셋팅하게 된다. */
	__super::Late_Update(fTimeDelta);

	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CPlayer::Render()
{
	return S_OK;
}

HRESULT CPlayer::Ready_PartObjects()
{
	/* 실제 추가하고 싶은 파트오브젝트의 갯수만큼 밸류를 셋팅해놓자. */
	/*m_Parts.resize(PART_END);

	CBody_Player::BODY_DESC		BodyDesc{};
	BodyDesc.pParentState = &m_iState;
	BodyDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
	if (FAILED(__super::Add_PartObject(PART_BODY, TEXT("Prototype_GameObject_Body_Player"), &BodyDesc)))
		return E_FAIL;
		*/



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

void CPlayer::Go_World_Straight(_float fTimeDelta, _float fSpeed, class CNavigation* pNavigation, _bool* pIsMove)
{
	m_pTransformCom->Go_World_Straight(fTimeDelta, fSpeed, pNavigation, pIsMove);
}

void CPlayer::Go_World_Backward(_float fTimeDelta, _float fSpeed, class CNavigation* pNavigation, _bool* pIsMove)
{
	m_pTransformCom->Go_World_Backward(fTimeDelta, fSpeed, pNavigation, pIsMove);
}

void CPlayer::Go_World_Right(_float fTimeDelta, _float fSpeed, class CNavigation* pNavigation, _bool* pIsMove)
{
	m_pTransformCom->Go_World_Right(fTimeDelta, fSpeed, pNavigation, pIsMove);
}

void CPlayer::Go_World_Left(_float fTimeDelta, _float fSpeed, class CNavigation* pNavigation, _bool* pIsMove)
{
	m_pTransformCom->Go_World_Left(fTimeDelta, fSpeed, pNavigation, pIsMove);
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

void CPlayer::Change_PlayerUI_TextureNum(_uint iIndex, _int iTextureNum)
{
	if (m_PlayerUI[iIndex] == nullptr)
		return;

	static_cast<CLink*>(m_pGameInstance->Find_Player(LEVEL_DUNGEON))->Get_Item(iTextureNum);

	m_PlayerUI[iIndex]->Set_TextureNum(iTextureNum);
}

void CPlayer::Set_UI_Active(_uint iIndex, _bool bActive)
{
	if (m_PlayerUI[iIndex] == nullptr)
		return;

	m_PlayerUI[iIndex]->SetActive(bActive);
}

_uint CPlayer::Get_CurRoomNum()
{
	return m_pNavigationCom->Get_CurrentCell_RoomNum();
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

	for (auto& pPlayerUI : m_PlayerUI)
		Safe_Release(pPlayerUI);

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pColliderCom);

}
