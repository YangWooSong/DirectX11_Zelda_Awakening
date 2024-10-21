#pragma once
#include "Client_Defines.h"
#include "2DEffects.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)
class CFootDust :
	public C2DEffects
{
private:
	CFootDust(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFootDust(const CFootDust& Prototype);
	virtual ~CFootDust() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_int m_iFrameIndex = { 0 };
	_int m_iTexCoordCol = { 0 };
	_int m_iTexCoordRow = { 0 };

	_float m_fFrame = { 0.f };
	_float3 m_vOffset = {};
	_float3 m_vNewPos = {};


private:
	HRESULT Ready_Components();

public:
	static CFootDust* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END;
