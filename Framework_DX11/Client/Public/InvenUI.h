#pragma once
#include "Client_Defines.h"
#include "UIObject.h"

BEGIN(Client)
class CInvenUI :
	public CUIObject
{
public:
	enum INVEN_TYPE { FEATHER, BOMB, TYPE_END };

private:
	CInvenUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CInvenUI(const CInvenUI& Prototype);
	virtual ~CInvenUI() = default;

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
	static CInvenUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END;
