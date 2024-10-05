#pragma once
#include "Client_Defines.h"
#include "UIObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END


BEGIN(Client)
class CMapBackGround :
	public CUIObject
{
public:
	enum MAP_BACK_TYPE { BACKGROUND, GRID_MAP, TYPE_END };
	typedef struct : public CUIObject::UI_DESC
	{
		_uint iBackType = { TYPE_END };
	} MAP_BACK_DESC;

private:
	CMapBackGround(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMapBackGround(const CMapBackGround& Prototype);
	virtual ~CMapBackGround() = default;

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
	_uint m_iBackType = { TYPE_END };
private:
	HRESULT Ready_Components();

public:
	static CMapBackGround* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END;
