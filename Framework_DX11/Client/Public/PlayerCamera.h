#pragma once
#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Client)

class CPlayerCamera :
	public CCamera
{
public:
	typedef struct : public CCamera::CAMERA_DESC {
		class CPlayer*	pPlayer = { nullptr };
		_float			fSpeed = {};
	}CAMERA_PLAYER_DESC;
private:
	CPlayerCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayerCamera(const CCamera& Prototype);
	virtual ~CPlayerCamera() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Start_Shake(_float _fAngle, _float _fSpeed, _float _fTime);	//흔들 각도, 속도(클수록 느림), 시간
	void Set_FollowPlayer(_bool _bFollowPlayer) { m_bFollowPlayer = _bFollowPlayer; }
	void Set_TargetToOtherPos(_vector _vTargetPos);

	void	Zoom_In(_float fZoom, _float _fRotY);
	void	Zoom_Out(_float fZoom, _float _fRotY);
	void	Active_Shake(_float _fTime);
private:
	HRESULT					Ready_Components();

private:
	class CPlayer*			m_pPlayer = { nullptr };

	_float					m_fSpeed = { 0.f };
	_float					m_fShakeMaxAngle = { 0.f };
	_float					m_fShakeMinAngle = { 0.f };
	_float					m_fShakeTime = { 0.f };
	_float					m_fRadian = { 0.f };
	_float					m_fTargetFovy = { 0.f };//목표 fovy

	_float					m_fNewRotationY = { 45.f };	//새로운 목표 각도
	_float					m_fDefaultRotationY = { 45.f };
	_float					m_fLerp = { 0.f };
	
	_float3					m_vOffset = { 0.0f, 10.0f, -10.0f };

	_bool					m_bShake = { false };
	_bool					m_bZoomIn = { false };
	_bool					m_bZoomOut = { false };
	_bool					m_bFollowPlayer = {true};

	_vector					m_vTargetPos = {};

public:
	static CPlayerCamera* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END