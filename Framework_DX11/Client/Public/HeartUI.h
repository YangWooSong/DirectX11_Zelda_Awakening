#pragma once
#include "Client_Defines.h"
#include "UIObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)
class CHeartUI :
	public CUIObject
{
public:
	typedef struct : public CUIObject::UI_DESC
	{
		_bool bNew = false;
	} HEART_DESC;

private:
	CHeartUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CHeartUI(const CHeartUI& Prototype);
	virtual ~CHeartUI() = default;

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
	void Set_Decrease(_bool bDecrease) { m_bStartDecrease = bDecrease; }
	void Set_Increase(_bool bIncrease) { m_bStartIncrease = bIncrease; }

private:
	_bool m_bStartDecrease = { false };
	_bool m_bStartIncrease= { false };
	_bool m_bNew = { false };

	_float m_fInvisibleAngle = { 0.f };
private:
	HRESULT Ready_Components();

public:
	static CHeartUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END;
