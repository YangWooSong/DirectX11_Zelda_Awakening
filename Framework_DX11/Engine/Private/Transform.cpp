#include "..\Public\Transform.h"
#include "Shader.h"


CTransform::CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
{
}

HRESULT CTransform::Initialize_Prototype()
{
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

	return S_OK;
}

HRESULT CTransform::Initialize(void* pArg)
{
	TRANSFORM_DESC* pDesc = static_cast<TRANSFORM_DESC*>(pArg);

	m_fSpeedPerSec = pDesc->fSpeedPerSec;
	m_fRotationPerSec = pDesc->fRotationPerSec;

	return S_OK;
}

_float3 CTransform::Get_Scaled() const
{
	_matrix		WorldMatrix = Get_WorldMatrix();

	return _float3(XMVectorGetX(XMVector3Length(WorldMatrix.r[STATE_RIGHT])),
		XMVectorGetX(XMVector3Length(WorldMatrix.r[STATE_UP])),
		XMVectorGetX(XMVector3Length(WorldMatrix.r[STATE_LOOK])));
}

void CTransform::Set_Scaled(_float fX, _float fY, _float fZ)
{
	Set_State(STATE_RIGHT, XMVector3Normalize(Get_State(STATE_RIGHT)) * fX);
	Set_State(STATE_UP, XMVector3Normalize(Get_State(STATE_UP)) * fY);
	Set_State(STATE_LOOK, XMVector3Normalize(Get_State(STATE_LOOK)) * fZ);
}

void CTransform::Go_Straight(_float fTimeDelta, _float fSpeed)
{
	_vector		vPosition = Get_State(STATE_POSITION);
	_vector		vLook = Get_State(STATE_LOOK);

	if (fSpeed <= 0.f)
		fSpeed = m_fSpeedPerSec;

	vPosition += XMVector3Normalize(vLook) * fSpeed * fTimeDelta;

	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Backward(_float fTimeDelta, _float fSpeed)
{
	_vector		vPosition = Get_State(STATE_POSITION);
	_vector		vLook = Get_State(STATE_LOOK);

	if (fSpeed <= 0.f)
		fSpeed = m_fSpeedPerSec;

	vPosition -= XMVector3Normalize(vLook) * fSpeed * fTimeDelta;

	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Right(_float fTimeDelta, _float fSpeed)
{
	_vector		vPosition = Get_State(STATE_POSITION);
	_vector		vRight = Get_State(STATE_RIGHT);

	if (fSpeed <= 0.f)
		fSpeed = m_fSpeedPerSec;

	vPosition += XMVector3Normalize(vRight) * fSpeed * fTimeDelta;

	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Left(_float fTimeDelta, _float fSpeed)
{
	_vector		vPosition = Get_State(STATE_POSITION);
	_vector		vRight = Get_State(STATE_RIGHT);

	if (fSpeed <= 0.f)
		fSpeed = m_fSpeedPerSec;

	vPosition -= XMVector3Normalize(vRight) * fSpeed * fTimeDelta;

	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_World_Straight(_float fTimeDelta, _float fSpeed)
{
	_vector		vPosition = Get_State(STATE_POSITION);
	_float3     vDir = { 0.0f, 0.0f,1.f };

	if (fSpeed <= 0.f)
		fSpeed = m_fSpeedPerSec;

	vPosition += XMLoadFloat3(&vDir) * fSpeed * fTimeDelta;

	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_World_Backward(_float fTimeDelta, _float fSpeed)
{
	_vector		vPosition = Get_State(STATE_POSITION);
	_float3     vDir = { 0.0f, 0.0f,-1.f };

	if (fSpeed <= 0.f)
		fSpeed = m_fSpeedPerSec;

	vPosition += XMLoadFloat3(&vDir) * fSpeed * fTimeDelta;

	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_World_Right(_float fTimeDelta, _float fSpeed)
{
	_vector		vPosition = Get_State(STATE_POSITION);
	_float3     vDir = { 1.f, 0.0f,0.0f };

	if (fSpeed <= 0.f)
		fSpeed = m_fSpeedPerSec;

	vPosition += XMLoadFloat3(&vDir) * fSpeed * fTimeDelta;

	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_World_Left(_float fTimeDelta, _float fSpeed)
{
	_vector		vPosition = Get_State(STATE_POSITION);
	_float3     vDir = { -1.f, 0.0f,0.0f };

	if (fSpeed <= 0.f)
		fSpeed = m_fSpeedPerSec;

	vPosition += XMLoadFloat3(&vDir) * fSpeed * fTimeDelta;

	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_World_Up(_float fTimeDelta, _float fSpeed)
{
	_vector		vPosition = Get_State(STATE_POSITION);
	_float3     vDir = { 0.f, 1.0f,0.0f };

	//if (fSpeed <= 0.f)
	//	fSpeed = m_fSpeedPerSec;

	vPosition += XMLoadFloat3(&vDir) * fSpeed * fTimeDelta;

	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_World_Down(_float fTimeDelta, _float fSpeed)
{
	_vector		vPosition = Get_State(STATE_POSITION);
	_float3     vDir = { 0.f, -1.0f,0.0f };

	if (fSpeed <= 0.f)
		fSpeed = m_fSpeedPerSec;

	vPosition += XMLoadFloat3(&vDir) * fSpeed * fTimeDelta;

	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Lerp(_float3 _CurrentPos, _float3 _TargetPos, _float _fSpeed)
{
	_vector CurPos = XMLoadFloat3(&_CurrentPos);
	_vector TaergetPos = XMLoadFloat3(&_TargetPos);
	
	_vector vDir = XMVector3Normalize(TaergetPos - CurPos);
	CurPos += vDir * _fSpeed;

	Set_State(STATE_POSITION, CurPos);
}


// 서서히 돌아감
void CTransform::Turn(const _vector& vAxis, _float fTimeDelta)
{
	_vector		vRight = Get_State(STATE_RIGHT);
	_vector		vUp = Get_State(STATE_UP);
	_vector		vLook = Get_State(STATE_LOOK);

	_matrix		RotationMatrix{};

	RotationMatrix = XMMatrixRotationAxis(vAxis, m_fRotationPerSec * fTimeDelta);

	Set_State(STATE_RIGHT, XMVector3TransformNormal(vRight, RotationMatrix));
	Set_State(STATE_UP, XMVector3TransformNormal(vUp, RotationMatrix));
	Set_State(STATE_LOOK, XMVector3TransformNormal(vLook, RotationMatrix));
}

void CTransform::Turn_Lerp(_fvector vLook, _float fTurnSpeed, _float fTimeDelta)
{
	_vector vCurrentLook = Get_State(STATE::STATE_LOOK);
	vCurrentLook = XMVector3Normalize(vCurrentLook);

	_vector vCurrentUp = Get_State(STATE::STATE_UP);
	vCurrentUp = XMVector3Normalize(vCurrentUp);

	_vector vTargetLook = vLook;
	vTargetLook = XMVector3Normalize(vTargetLook);

	_vector vTargetRight = XMVector3Cross(vCurrentUp, vTargetLook);
	vTargetLook = XMVector3Cross(vTargetRight, vCurrentUp);
	vTargetRight = XMVector3Normalize(vTargetRight);

	_vector vRadian = XMVector3Dot(vTargetLook, vCurrentLook);
	_float fRadian = {};

	XMStoreFloat(&fRadian, vRadian);
	fRadian = acosf(fRadian);;

	if (fRadian > 1.0f)
	{
		fRadian = 1.0f;
	}
	else if (fRadian < -1.0f)
	{
		fRadian = -1.0f;
	}

	if (fRadian <= fTurnSpeed * fTimeDelta)
	{
		vCurrentLook = vTargetLook;

		_vector vCurrentRight = XMVector3Cross(vCurrentUp, vCurrentLook);
		vCurrentLook = XMVector3Cross(vCurrentRight, vCurrentUp);
		vCurrentUp = XMVector3Cross(vCurrentLook, vCurrentRight);

		_float3 vScale = Get_Scaled();

		Set_State(STATE_RIGHT, XMVector3Normalize(vCurrentRight) * vScale.x);
		Set_State(STATE_UP, XMVector3Normalize(vCurrentUp) * vScale.y);
		Set_State(STATE_LOOK, XMVector3Normalize(vCurrentLook) * vScale.z);

		return;
	}

	_vector vCrossUp = XMVector3Cross(vCurrentLook, vTargetLook);

	_float fDot;
	XMStoreFloat(&fDot, XMVector3Dot(vCurrentUp, vCrossUp));

	if (fDot < 0)
		fTurnSpeed *= -1;

	// 쿼터니언 생성
	XMVECTOR vRotQuaternion = XMQuaternionRotationAxis(vCurrentUp, fTurnSpeed * fTimeDelta);

	// 회전 행렬 생성
	XMMATRIX RotationMatrix = XMMatrixRotationQuaternion(vRotQuaternion);

	XMMATRIX WorldMatirx = XMLoadFloat4x4(&m_WorldMatrix) * RotationMatrix;
	_float4x4 World4x4;
	XMStoreFloat4x4(&World4x4, WorldMatirx);

	// 상태 업데이트
	Set_State(STATE_RIGHT, XMLoadFloat4((_float4*)&World4x4.m[0]));
	Set_State(STATE_UP, XMLoadFloat4((_float4*)&World4x4.m[1]));
	Set_State(STATE_LOOK, XMLoadFloat4((_float4*)&World4x4.m[2]));
}

void CTransform::Turn_Lerp_Angle(_float3 _Origin, _float3 _Target, _float fTimeDelta)
{
	m_bIsTurning = true;
	_float fLerpSpeed = min(1.0f, 3.f * fTimeDelta);
	_vector vtargetRot = XMVectorLerp(XMLoadFloat3(&_Origin), XMLoadFloat3(&_Target), fLerpSpeed);

	_float3 vTargetRot = {};
	XMStoreFloat3(&vTargetRot, vtargetRot);
	RotationThreeAxis(vTargetRot);
	m_fRot = vTargetRot;
	if (fabs(vTargetRot.x - _Origin.x) < 0.2  && fabs(vTargetRot.y - _Origin.y) < 0.2 && fabs(vTargetRot.x - _Origin.z) < 0.2)
		m_bIsTurning = false;
}

// 원하는 회전으로 세팅
void CTransform::Rotation(const _vector& vAxis, _float fRadian)
{
	_float3		vScaled = Get_Scaled();

	_vector		vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f);
	_vector		vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	_vector		vLook = XMVectorSet(0.f, 0.f, 1.f,  0.f);

	_matrix		RotationMatrix{};

	RotationMatrix = XMMatrixRotationAxis(vAxis, fRadian);

	Set_State(STATE_RIGHT, XMVector3TransformNormal(vRight, RotationMatrix));
	Set_State(STATE_UP, XMVector3TransformNormal(vUp, RotationMatrix));
	Set_State(STATE_LOOK, XMVector3TransformNormal(vLook, RotationMatrix));
}

void CTransform::Rotation(_float fX, _float fY, _float fZ)
{
	_vector		vRotation = XMQuaternionRotationRollPitchYaw(fX, fY, fZ);

	_float3		vScaled = Get_Scaled();

	_vector		vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * vScaled.x;
	_vector		vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * vScaled.y;
	_vector		vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * vScaled.z;

	_matrix		RotationMatrix = XMMatrixRotationQuaternion(vRotation);

	Set_State(STATE_RIGHT, XMVector3TransformNormal(vRight, RotationMatrix));
	Set_State(STATE_UP, XMVector3TransformNormal(vUp, RotationMatrix));
	Set_State(STATE_LOOK, XMVector3TransformNormal(vLook, RotationMatrix));

}

void CTransform::LookAt(_fvector vAt)
{
	_float3		vScale = Get_Scaled();

	_vector		vPosition = Get_State(STATE_POSITION);

	_vector		vLook = vAt - vPosition;

	// Up 벡터와 Look 벡터를 외적하여 Right 벡터를 구함
	_vector		vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);

	// Look 벡터와 Right 벡터를 외적하여 진짜 Up 벡터를 구함
	_vector		vUp = XMVector3Cross(vLook, vRight);

	// 스케일을 맞춰줌
	Set_State(STATE_RIGHT, XMVector3Normalize(vRight) * vScale.x);
	Set_State(STATE_UP, XMVector3Normalize(vUp) * vScale.y);
	Set_State(STATE_LOOK, XMVector3Normalize(vLook) * vScale.z);
}

void CTransform::RotationThreeAxis(const _float3 fRadians)
{
	_float3		vScaled = Get_Scaled();

	_float3		vRight = _float3(Get_Scaled().x, 0.f, 0.f);
	_float3		vUp = _float3(0.f, Get_Scaled().y, 0.f);
	_float3		vLook = _float3(0.f, 0.f, Get_Scaled().z);

	_matrix		RotationMatrix{};

	XMMATRIX rotationMatrixX = XMMatrixRotationX(XMConvertToRadians(fRadians.x));
	XMMATRIX rotationMatrixY = XMMatrixRotationY(XMConvertToRadians(fRadians.y));
	XMMATRIX rotationMatrixZ = XMMatrixRotationZ(XMConvertToRadians(fRadians.z));

	RotationMatrix = XMMatrixMultiply(rotationMatrixX, XMMatrixMultiply(rotationMatrixY, rotationMatrixZ));

	XMStoreFloat3(&vRight, XMVector3TransformNormal(XMLoadFloat3(&vRight), RotationMatrix));
	XMStoreFloat3(&vUp, XMVector3TransformNormal(XMLoadFloat3(&vUp), RotationMatrix));
	XMStoreFloat3(&vLook, XMVector3TransformNormal(XMLoadFloat3(&vLook), RotationMatrix));

	Set_State(STATE_RIGHT, XMLoadFloat3(&vRight));
	Set_State(STATE_UP, XMLoadFloat3(&vUp));
	Set_State(STATE_LOOK, XMLoadFloat3(&vLook));
}

_vector CTransform::Get_Dir(_vector _pos1, _vector _pos2)
{
	_vector vDir = XMVector3Normalize(_pos1 - _pos2);
	return vDir;
}

HRESULT CTransform::Bind_ShaderResource(CShader* pShader, const _char* pConstantName)
{
	return pShader->Bind_Matrix(pConstantName, &m_WorldMatrix);

}

CTransform* CTransform::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CTransform* pInstance = new CTransform(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CTransform"));
		Safe_Release(pInstance);
	}


	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CTransform"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTransform::Free()
{
	__super::Free();
}
