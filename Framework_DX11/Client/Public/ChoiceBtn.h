#pragma once
#include "Client_Defines.h"
#include "UIObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)
class CChoiceBtn :
	public CUIObject
{
public:
	enum CHOICETYPE { BUY, NOTBUY, TYPE_END };
public:
	typedef struct : public CUIObject::UI_DESC
	{
		_int iTextureNum;
	} CHOICE_BTN_DESC;
private:
	CChoiceBtn(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CChoiceBtn(const CChoiceBtn& Prototype);
	virtual ~CChoiceBtn() = default;

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
	CTexture* m_pTextureCom[2] = {nullptr};
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

private:
	_int m_iTextureNum = {0};
	_float m_fAlpha = {0.f};

public:
	static CChoiceBtn* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END;
