#pragma once
#include "Client_Defines.h"
#include "UIObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END


BEGIN(Client)
class CMapUI :
	public CUIObject
{
public:
	enum MAP_TYPE { MASK, REVEAL, TYPE_END };
	typedef struct : public CUIObject::UI_DESC
	{
		_int iMapNum = { 0 };
		_uint iMapType = { 0 };
	} MAP_DESC;

private:
	CMapUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMapUI(const CMapUI& Prototype);
	virtual ~CMapUI() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom[2] = {nullptr};
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };


private:
	_uint m_iMapNUm = { 0 };
	_uint m_iMapType = { TYPE_END };
private:
	HRESULT Ready_Components();

public:
	static CMapUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END;
