#include "stdafx.h"

#include "PlayerCamera.h"
#include "GameInstance.h"

#include "Player.h"

CPlayerCamera::CPlayerCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamera{ pDevice, pContext }
{
}

CPlayerCamera::CPlayerCamera(const CCamera& Prototype)
	:CCamera{ Prototype }
{
}

HRESULT CPlayerCamera::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayerCamera::Initialize(void* pArg)
{
	CAMERA_PLAYER_DESC* pDesc = static_cast<CAMERA_PLAYER_DESC*>(pArg);
	m_pPlayer = pDesc->pPlayer;
	m_fSpeed = pDesc->fSpeed;
	m_bFollowPlayer = pDesc->bFollowPlayer;
	_float3 vTarget = {};
	vTarget.x = pDesc->vEye.x;
	vTarget.y = pDesc->vEye.y;
	vTarget.z = pDesc->vEye.z;
	m_vTargetPos = XMLoadFloat3(&vTarget);
	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if(m_bFollowPlayer)
	{
		//초기 위치 설정 (Player Pos + offset)
		_vector vInitPos = m_pPlayer->Get_Transform()->Get_State(CTransform::STATE::STATE_POSITION) + XMVectorSet(m_vOffset.x, m_vOffset.y, m_vOffset.z, 0.f);
		m_pTransformCom->Set_State(CTransform::STATE::STATE_POSITION, vInitPos);
	}

	_vector vDir = m_pTransformCom->Get_State(CTransform::STATE_POSITION) - m_pPlayer->Get_Transform()->Get_State(CTransform::STATE_POSITION);
	vDir = XMVector3Normalize(vDir);
	
	//회전 구하기?
	_vector vRadian = XMVector3Dot(vDir, XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
	XMStoreFloat(&m_fRadian, vRadian);
	m_fRadian = acosf(m_fRadian);


	m_fNewRotationY = pDesc->fDefaultAngle;
	m_fDefaultRotationY = pDesc->fDefaultAngle;
	m_pTransformCom->RotationThreeAxis(_float3(m_fDefaultRotationY, 0.f, 0.f));
	return S_OK;
}

void CPlayerCamera::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CPlayerCamera::Update(_float fTimeDelta)
{
	
#pragma region 회전
	if (fabs(m_fNewRotationY - m_fDefaultRotationY) > 0.2f)
	{
		if(m_fNewRotationY - m_fDefaultRotationY < 0)
			m_fDefaultRotationY -= 0.55f;
		if (m_fNewRotationY - m_fDefaultRotationY > 0)
			m_fDefaultRotationY += 0.55f;
		m_pTransformCom->RotationThreeAxis(_float3(m_fDefaultRotationY, 0.f,0.f));
	}
	else
	{
		m_fDefaultRotationY = m_fNewRotationY;
		m_pTransformCom->RotationThreeAxis(_float3(m_fDefaultRotationY, 0.f, 0.f));
	}
#pragma endregion

#pragma region 줌인 / 줌아웃 
	//줌인
	if (m_bZoomIn && m_fFovy > m_fTargetFovy)
	{
		m_fFovy -= 0.02f;
		if (m_fFovy < 0)
			m_fFovy = 0.01f;
	}
	else
		m_bZoomIn = false;

	//줌 아웃
	if (m_bZoomOut && m_fFovy < m_fTargetFovy)
		m_fFovy += 0.02f;
	else
		m_bZoomOut = false;
#pragma endregion

	if (m_bShake)
		Active_Shake(fTimeDelta);

	//플레이어 따라 다니기
	if (m_bFollowPlayer)
		m_vTargetPos = m_pPlayer->Get_Position() + XMLoadFloat3(&m_vOffset);
	
	_float fLerpSpeed = min(1.0f, 3.f * fTimeDelta);
 	_vector finalPos = XMVectorLerp(m_pTransformCom->Get_State(CTransform::STATE_POSITION), m_vTargetPos, fLerpSpeed);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, finalPos);
}

void CPlayerCamera::Late_Update(_float fTimeDelta)
{
}

HRESULT CPlayerCamera::Render()
{
	return S_OK;
}

void CPlayerCamera::Start_Shake(_float _fAngle, _float _fSpeed, _float _fTime)
{
	m_bShake = true;
	m_fShakeMaxAngle = m_fDefaultRotationY + (_fAngle / 2);
	m_fShakeMinAngle = m_fDefaultRotationY - (_fAngle / 2);
	m_fShakeTime = _fTime;
	m_fSpeed = _fSpeed;
}

void CPlayerCamera::Set_TargetToOtherPos(_vector _vTargetPos)
{
	m_bFollowPlayer = false;
	m_vTargetPos = _vTargetPos;
}

void CPlayerCamera::Zoom_In(_float fZoom, _float _fRotY)
{
	m_bZoomIn = true;
	m_fTargetFovy = m_fFovy / fZoom;

	m_fNewRotationY = _fRotY;
}

void CPlayerCamera::Zoom_Out(_float fZoom, _float _fRotY)
{
	m_bZoomOut = true;
	if (m_fFovy * fZoom <= XMConvertToRadians(180.f)) //화면이 뒤집어지는 것 방지
		m_fTargetFovy = m_fFovy * fZoom;

	m_fNewRotationY = _fRotY;
}

void CPlayerCamera::Active_Shake(_float _fTime)
{
	m_fShakeTime -= _fTime;

	if(m_fShakeTime > 0)
	{
		m_fNewRotationY += m_fSpeed;

		if (m_fNewRotationY > m_fShakeMaxAngle || m_fNewRotationY < m_fShakeMinAngle)
			m_fSpeed = m_fSpeed * (-1.f);
	}
	else
	{
		m_fDefaultRotationY = 45.f;
		m_fNewRotationY = 45.f;
		m_fShakeTime = 0;
		m_bShake = false;
	}
}

HRESULT CPlayerCamera::Ready_Components()
{
	return S_OK;
}

CPlayerCamera* CPlayerCamera::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPlayerCamera* pInstance = new CPlayerCamera(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CCamera_Player"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CPlayerCamera::Clone(void* pArg)
{
	CPlayerCamera* pInstance = new CPlayerCamera(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CCamera_Player"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerCamera::Free()
{
	__super::Free();
}
