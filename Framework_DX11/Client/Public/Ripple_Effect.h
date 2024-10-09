#pragma once
#include "Client_Defines.h"
#include "2DEffects.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)
class CRipple_Effect :
	public C2DEffects
{
public:
	typedef struct : public C2DEffects::EFFECT_DESC
	{
		_uint iTextureNum = 0;
	} RIPPLE_DESC;
private:
	CRipple_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRipple_Effect(const CRipple_Effect& Prototype);
	virtual ~CRipple_Effect() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_bool m_bSizeUp = { false };
	_float3 m_fOffset = {};	//위치 오프셋
	_float3 m_fOriSize = {};	//위치 오프셋
	_float4 m_fOriColor = {};	//위치 오프셋
	bool m_bAlphaChange = { false };
	_uint m_iTextureNum = { 0 };
	_bool m_bReset = { false };
	_float m_fSpeed = {};
	_float m_fMaxSize = {};


private:
	HRESULT Ready_Components();
	void AlphaDown(_float fTimeDelta);
	void Lerp_Size_Up(_float fTimeDelta);

public:
	static CRipple_Effect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END;
