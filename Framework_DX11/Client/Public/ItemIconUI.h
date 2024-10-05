#pragma once
#include "Client_Defines.h"
#include "UIObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)
class CItemIconUI :
	public CUIObject
{
public:
public:
	enum ITEM_TYPE { SMALL_KEY, BOSS_KEY, INVEN_FEATHER, INVEN_BOMB, TYPE_END };
	typedef struct : public CUIObject::UI_DESC
	{
		_uint iItemType = { TYPE_END };
	} ITEM_ICON_DESC;

private:
	CItemIconUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CItemIconUI(const CItemIconUI& Prototype);
	virtual ~CItemIconUI() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

private:
	_uint m_iIconType = { TYPE_END };

private:
	HRESULT Ready_Components();

public:
	static CItemIconUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END;
