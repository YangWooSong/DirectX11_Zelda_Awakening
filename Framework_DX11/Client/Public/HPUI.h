#pragma once
#include "Client_Defines.h"
#include "UIObject.h"

BEGIN(Client)
class CHPUI :
	public CUIObject
{
private:
	CHPUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CHPUI(const CHPUI& Prototype);
	virtual ~CHPUI() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_int m_iMaxHP = (0);
	_int m_iCurHp = { 0 };

private:
	HRESULT Ready_ChildUI();
	void Add_Hp();
	void Set_HeartUI();
public:
	static CHPUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END;
