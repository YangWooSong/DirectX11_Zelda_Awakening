#pragma once
#include "Client_Defines.h"
#include "UIObject.h"

BEGIN(Client)
class CKeyUI :
	public CUIObject
{
public:
	enum KEY_TYPE{SMALL1, SMALL2, BOSS, TYPE_END};

private:
	CKeyUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CKeyUI(const CKeyUI& Prototype);
	virtual ~CKeyUI() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_ChildUI();

public:
	static CKeyUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END;
