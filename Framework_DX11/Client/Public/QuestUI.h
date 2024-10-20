#pragma once
#include "Client_Defines.h"
#include "UIObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)
class CQuestUI :
	public CUIObject
{
private:
	CQuestUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CQuestUI(const CQuestUI& Prototype);
	virtual ~CQuestUI() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	void Start_Hide() 
	{ 
		m_bHide = true; 
		m_bShow = false; 
	}
private:
	HRESULT Ready_Components();

public:
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom[2] = {nullptr};
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

private:
	_bool m_bShow = { false };
	_bool m_bShowEnd = { false };
	_bool m_bHide = { false };
	_bool m_bHideEnd = { false };

	_int m_iKillMonsterCount = { 0 };
	_uint m_iLevelIndex = { LEVEL_END };

	_float m_fActiveTimer = { 0.f };
	_float m_fTargetX = { g_iWinSizeX * 0.87f };
	_float m_fOriPosX = { 0.f };
	_float m_fAlpha = { 0.f };

private:
	void Show(_float fTimeDelta);
	void Hide(_float fTimeDelta);

public:
	static CQuestUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END;
