#pragma once
#include "Client_Defines.h"
#include "2DEffects.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)
class CLightning_Effect :
	public C2DEffects
{
private:
	CLightning_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLightning_Effect(const CLightning_Effect& Prototype);
	virtual ~CLightning_Effect() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	bool m_bSizeDown = { false };
	bool m_bSizeUp = { false };
	bool m_bReset = { false };

	_int m_iFrameIndex = { 0 };
	_int m_iTexCoordCol = { 0 };
	_int m_iTexCoordRow = { 0 };

	_float m_fMaxSize = { 0.f };
	_float m_fFrame = { 0.f };

	_float3 m_vOriSize = {};
	_float3 m_vOffset = {};
private:
	HRESULT Ready_Components();
	void Lerp_Size(_float fTimeDelta);	//커졌다 작아짐(한번)

public:
	static CLightning_Effect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END;
