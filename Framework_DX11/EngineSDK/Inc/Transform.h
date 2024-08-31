#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CTransform final : public CComponent
{
public:
	typedef struct
	{
		_float		fSpeedPerSec{}, fRotationPerSec{};
	}TRANSFORM_DESC;

	enum STATE { STATE_RIGHT, STATE_UP, STATE_LOOK, STATE_POSITION, STATE_END };
	
private:
	CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CTransform() = default;

public:
	_vector		Get_State(STATE eState) { return XMLoadFloat4x4(&m_WorldMatrix).r[eState]; }
	void		Set_State(STATE eState, _fvector vState) { XMStoreFloat3((_float3*)&m_WorldMatrix.m[eState][0], vState); }

	_matrix		Get_WorldMatrix() const { return XMLoadFloat4x4(&m_WorldMatrix); }	//연산용
	_float4x4	Get_World4x4() const { return m_WorldMatrix; }	//저장용

	_matrix		Get_WorldMatrix_Inverse() const { return XMMatrixInverse(nullptr, Get_WorldMatrix()); }
	const _float4x4* Get_WorldMatrix_Ptr() const {	return &m_WorldMatrix;}

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);

public:
	_float3		Get_Scaled() const;
	void		Set_Scaled(_float fX, _float fY, _float fZ);

	void		Go_Straight(_float fTimeDelta, _float fSpeed = 0.f, class CNavigation* pNavigation = nullptr);
	void		Go_Backward(_float fTimeDelta, _float fSpeed = 0.f);
	void		Go_Right(_float fTimeDelta, _float fSpeed = 0.f);
	void		Go_Left(_float fTimeDelta, _float fSpeed = 0.f);

	void		Go_World_Straight(_float fTimeDelta, _float fSpeed = 0.f, class CNavigation* pNavigation = nullptr, _bool* bIsMove = nullptr);
	void		Go_World_Backward(_float fTimeDelta, _float fSpeed = 0.f, class CNavigation* pNavigation = nullptr, _bool* bIsMove = nullptr);
	void		Go_World_Right(_float fTimeDelta, _float fSpeed = 0.f, class CNavigation* pNavigation = nullptr, _bool* bIsMove = nullptr);
	void		Go_World_Left(_float fTimeDelta, _float fSpeed = 0.f, class CNavigation* pNavigation = nullptr, _bool* bIsMove = nullptr);

	void		Go_World_Up(_float fTimeDelta, _float fSpeed = 0.f);
	void		Go_World_Down(_float fTimeDelta, _float fSpeed = 0.f);

	void		Go_Lerp(_float3 _CurrentPos, _float3 _TargetPos, _float _fSpeed);

	void		Turn(const _vector& vAxis, _float fTimeDelta);
	void		Turn_Lerp(_fvector vLook, _float fTurnSpeed, _float fTimeDelta);
	void		Turn_Lerp_Angle(_float3 _Origin, _float3 _Target, _float fTimeDelta);
	void		Rotation(const _vector& vAxis, _float fRadian);
	void		Rotation(_float fX, _float fY, _float fZ);

	void		LookAt(_fvector vAt);

	void		RotationThreeAxis(const _float3 fRadians);

	void		Set_WorldMatrix(const _matrix _worldMatrix) { XMStoreFloat4x4(&m_WorldMatrix, _worldMatrix); }
	
	_bool		Get_IsTurning() { return m_bIsTurning; }
	_float3		Get_Rot() { return m_fRot; }
	_vector		Get_Dir(_vector _pos1, _vector _pos2);

	void		ChangePosToPreCellMiddle(CNavigation* pNavigation);
public:
	HRESULT Bind_ShaderResource(class CShader* pShader, const _char* pConstantName);

private:
	_float			m_fSpeedPerSec{};
	_float			m_fRotationPerSec{};

	_float4x4		m_WorldMatrix{};
	_bool			m_bIsTurning = { false };
	_float3			m_fRot = { };

public:
	static CTransform* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	virtual CComponent* Clone(void* pArg) { return nullptr; }
	virtual void Free() override;
};

END