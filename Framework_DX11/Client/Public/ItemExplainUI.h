#pragma once
#include "Client_Defines.h"
#include "UIObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)
class CItemExplainUI :
	public CUIObject
{
public:
	enum ITEM_TYPE{BOMB, HEART, TYPE_END};
	typedef struct : public CUIObject::UI_DESC
	{
		_uint iItemType = { TYPE_END };
	} ITEM_EXPLAIN_DESC;

private:
	CItemExplainUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CItemExplainUI(const CItemExplainUI& Prototype);
	virtual ~CItemExplainUI() = default;

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

private:
	_bool m_bShow = { false };
	_bool m_bShowEnd = { false };
	_bool m_bHide = { false };

	_uint m_iLevelIndex = { LEVEL_END };
	_uint m_iItemIndex = { LEVEL_END };

	_float m_fAlpha = { 0.5f };

private:
	void Show(_float fTimeDelta);
	void Hide(_float fTimeDelta);

public:
	static CItemExplainUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END;
