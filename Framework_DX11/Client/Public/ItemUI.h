#pragma once
#include "Client_Defines.h"
#include "UIObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)
class CItemUI :
	public CUIObject
{
public:
	enum ITEMINDEX { STONEBEAK, CAMPUS, SMALLKEY, MAP, LUPEE, FEATHER, BOSSKEY, CONCHHORN,SWORD, ITEM_END };
private:
	CItemUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CItemUI(const CItemUI& Prototype);
	virtual ~CItemUI() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	void Set_Show(_bool bShow) { m_bShow = bShow; }
	void Set_Disappear(_bool bDisappear) { m_bDisappear = bDisappear; }
private:
	HRESULT Ready_Components();

public:
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

private:
	_bool m_bShow = { false };
	_bool m_bOff = { false };
	_bool m_bDisappear = { false };
	_bool m_bSizeDown = { false };

	_vector m_TargetPos = {};
	_float3 m_MinSize = {0.1f,0.1f,0.1f };

private:
	void Lerp_Size(_float fTimeDelta);
	void Lerp_Pos(_float fTimeDelta);
public:
	static CItemUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END;
