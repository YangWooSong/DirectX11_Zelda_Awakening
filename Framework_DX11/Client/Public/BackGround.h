#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CBackGround final : public CUIObject
{
public: 
	enum TYPE{BACKGROUND, KEY, KEY_BACKGROUND, LOADING_BACKGROUND, LOADING_ICON, HOUSE_BACKGROUND, TYPE_END};
	typedef struct : public CUIObject::UI_DESC
	{
		TYPE eType;
	} BACKGROUND_DESC;
private:
	CBackGround(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBackGround(const CBackGround& Prototype);
	virtual ~CBackGround() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	class CShader* m_pShaderCom = { nullptr };
	class CTexture* m_pTextureCom = { nullptr };
	class CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

private:
	HRESULT Ready_Components();
	TYPE m_eType = { TYPE_END };
	_bool m_bBlink = { false };
	_float m_fBrightness = { 0.4f };
	_float m_fBrightAmout = (0.01f);

	_float m_fSpeed = { 0.f };
public:
	static CBackGround* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END