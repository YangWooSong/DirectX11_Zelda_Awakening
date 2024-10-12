#pragma once
#include "Client_Defines.h"
#include "3D_Effects.h"

BEGIN(Engine)
class CShader;
class CModel;
class CTexture;
END
BEGIN(Client)
class CFire_Plane_Effect :
	public C3D_Effects
{
public:
	typedef struct : public C3D_Effects::MODEL_EFFECT_DESC
	{
		_uint iDifuseNum = { 0 }; //0은 위로 향한거, 1은 옆을 향한거
	} FIRE_EFFECT_DESC;
private:
	CFire_Plane_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFire_Plane_Effect(const CFire_Plane_Effect& Prototype);
	virtual ~CFire_Plane_Effect() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_uint m_iDuffuseNum = { 0 };
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