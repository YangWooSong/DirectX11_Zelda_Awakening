#pragma once
#include "Client_Defines.h"
#include "UIObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)
class C2DEffects :
	public CUIObject
{
public:
	enum EFFECT_TYPE{MONSTER_DIED, TYPE_END};
	typedef struct : public CGameObject::GAMEOBJECT_DESC
	{
		_float4 fColor = {1.f,1.f,1.f,1.f};
		_uint iLevelIndex = LEVEL_END;
		CGameObject* pParent = { nullptr };
		_uint iEffectType = { TYPE_END };
	} EFFECT_DESC;

protected:
	C2DEffects(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	C2DEffects(const C2DEffects& Prototype);
	virtual ~C2DEffects() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Components();

public:
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

protected:
	CGameObject* m_pParentObj = { nullptr };
	_float4 m_fColor = {};
	_uint m_iLevelIndex = { LEVEL_END };
	_uint m_iEffectType = { TYPE_END };
	vector<CGameObject*>	m_Child_List;
public:
	static C2DEffects* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END;
