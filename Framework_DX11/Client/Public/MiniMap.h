#pragma once
#include "Client_Defines.h"
#include "UIObject.h"

BEGIN(Client)
class CMiniMap :
	public CUIObject
{
public:
	enum INVEN_TYPE { BACKGROUND, MAP,ROOM, TYPE_END };

private:
	CMiniMap(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMiniMap(const CMiniMap& Prototype);
	virtual ~CMiniMap() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_ChildUI();
	void Culculate_PlayerIconPos();
public:
	static CMiniMap* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END;
