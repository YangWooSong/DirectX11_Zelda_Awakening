#pragma once
#include "Client_Defines.h"
#include "3D_Effects.h"

BEGIN(Engine)
class CShader;
class CModel;
class CTexture;
END
BEGIN(Client)
class CCubeHalfDome :
	public C3D_Effects
{
private:
	CCubeHalfDome(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCubeHalfDome(const CCubeHalfDome& Prototype);
	virtual ~CCubeHalfDome() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_float m_fTexMove = { 0.f };
	_float3 m_fOriSize = {};
	_bool m_bMoveAlpha = { false };
	_float4 m_fColor = { 1.f,1.f,1.f,1.f };
private:
	HRESULT Ready_Components();
public:
	static C3D_Effects* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END