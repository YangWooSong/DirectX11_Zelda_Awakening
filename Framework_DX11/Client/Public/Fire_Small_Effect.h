#pragma once
#include "Client_Defines.h"
#include "2DEffects.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)
class CFire_Small_Effect :
	public C2DEffects
{
public:
	typedef struct : public C2DEffects::EFFECT_DESC
	{
		_uint iFireTypeNum = { 0 }; //0 = 큰 불, 1 == 작은 불
	} FIRE_SAMLL_EFFECT_DESC;
private:
	CFire_Small_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFire_Small_Effect(const CFire_Small_Effect& Prototype);
	virtual ~CFire_Small_Effect() = default;

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

	_uint m_iFireTypeNum = { 0 };

	_float m_fMaxSize = { 0.f };
	_float3 m_vOriSize = {};
	_float3 m_vOffset = {};
private:
	HRESULT Ready_Components();
	void Lerp_Size(_float fTimeDelta);	//커졌다 작아짐(한번)

public:
	static CFire_Small_Effect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END;
