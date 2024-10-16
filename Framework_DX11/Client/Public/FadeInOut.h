#pragma once
#include "Client_Defines.h"
#include "UIObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END


BEGIN(Client)
class CFadeInOut :
	public CUIObject
{
private:
	CFadeInOut(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFadeInOut(const CFadeInOut& Prototype);
	virtual ~CFadeInOut() = default;

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

public:
	void FadeIn();
	void FadeOut();

private:
	_bool m_bFadeIn = { false }; //점점 어두운게 사라지는
	_bool m_bFadeOut = { false }; //점점 어두워 지는

	_bool m_bRenderEndingText = { false };

	_float m_fAlpha = {};

private:
	HRESULT Ready_Components();

public:
	static CFadeInOut* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END;
