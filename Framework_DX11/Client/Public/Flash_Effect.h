#pragma once
#include "Client_Defines.h"
#include "2DEffects.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)
class CFlash_Effect :
	public C2DEffects
{
public:
	typedef struct : public C2DEffects::EFFECT_DESC
	{
		_uint iTextureNum = 0;
	} FLASH_DESC;
private:
	CFlash_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFlash_Effect(const CFlash_Effect& Prototype);
	virtual ~CFlash_Effect() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	//폭탄 심지 이펙트를 위한 변수들
	_bool m_bSizedown = { false };
	_float m_fAngle = 0.f;	//회전 값
	_float3 m_fOffset = {};	//위치 오프셋

	//공용 변수
	bool m_bAlphaDown = { false };
	_uint m_iTextureNum = { 0 };
	_bool m_bReset = { false };
	_bool m_bDisappear = { false };
	_float m_fSpeed = {};
	_float4 m_fOriColor = {};

private:
	HRESULT Ready_Components();
	void AlphaDown(_float fTimeDelta);
	void Lerp_Size(_float fTimeDelta);
	void Lerp_SizeUp(_float fTimeDelta);
	void Change_Color(_float fTimeDelta);

public:
	static CFlash_Effect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END;
