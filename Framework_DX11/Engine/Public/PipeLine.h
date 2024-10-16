#pragma once

#include "Base.h"

/* ��, ������ĸ� �����Ѵ�. */
/* �� �����Ӹ��� Update�Լ��� ȣ���ϸ鼭 ����־��� ��,��������� �������  ī�޶��� ��ġ �̸� ���صд�. */

BEGIN(Engine)

class CPipeLine final : public CBase
{
public:
	enum D3DTRANSFORMSTATE { D3DTS_VIEW, D3DTS_PROJ, D3DTS_SHADOW_LIGHT, D3DTS_END };
private:
	CPipeLine();
	virtual ~CPipeLine() = default;

public:
	_float4x4 Get_Transform_Float4x4(D3DTRANSFORMSTATE eState) const {
		return m_TransformMatrices[eState];
	}
	void Set_Transform(D3DTRANSFORMSTATE eState, _fmatrix TransformMatrix) {
		XMStoreFloat4x4(&m_TransformMatrices[eState], TransformMatrix);
	}
	void Set_ShadowLight(D3DTRANSFORMSTATE eState, _fvector vPlayerPos) {
		_vector vOffset = { 5.f, 20.f,-15.f,0.f };
		XMStoreFloat4(&m_vShadowLightPos, vPlayerPos + vOffset);
		XMStoreFloat4(&m_vLightLookPos, vPlayerPos);
	}
	_matrix Get_Transform_Matrix(D3DTRANSFORMSTATE eState) const {
		return XMLoadFloat4x4(&m_TransformMatrices[eState]);
	}
	_float4x4 Get_Transform_Inverse_Float4x4(D3DTRANSFORMSTATE eState) const {
		return m_TransformInverseMatrices[eState];
	}
	_matrix Get_Transform_Inverse_Matrix(D3DTRANSFORMSTATE eState) const {
		return XMLoadFloat4x4(&m_TransformInverseMatrices[eState]);
	}

	_float4 Get_CamPosition_Float4() const {
		return m_vCamPosition;
	}
	_vector Get_CamPosition_Vector() const {
		return XMLoadFloat4(&m_vCamPosition);
	}
	_vector Get_ShadowLightPos_Vector() const {
		return XMLoadFloat4(&m_vShadowLightPos);
	}

	_vector Get_LightLook_Vector() const {
		return XMLoadFloat4(&m_vLightLookPos);
	}

public:
	HRESULT Initialize();
	void Update();

private:
	_float4x4			m_TransformMatrices[D3DTS_END];
	_float4x4			m_TransformInverseMatrices[D3DTS_END];
	_float4				m_vCamPosition = {};
	_float4				m_vShadowLightPos= {5.f,20.f,-15.f,1.f};
	_float4				m_vLightLookPos= {0.f,0.f,0.f,1.f};

public:
	static CPipeLine* Create();
	virtual void Free() override;
};

END