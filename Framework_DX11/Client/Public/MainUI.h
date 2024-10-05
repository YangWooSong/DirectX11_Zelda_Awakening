#pragma once
#include "Client_Defines.h"
#include "UIObject.h"

BEGIN(Client)
class CMainUI :
	public CUIObject
{
public:
	enum CHILD_INDEX { LUPEE,HP, CHILD_END };

	typedef struct : public CUIObject::UI_DESC
	{
		_uint iLevelIndex;
	} MAINUI_DESC;
private:
	CMainUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMainUI(const CMainUI& Prototype);
	virtual ~CMainUI() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	_int  Get_Player_Lupee();
	_int Get_Player_MaxHP();
	_int Get_Player_CurHp();
	_uint Get_LevelIndex() { return m_iLevelIndex; }

private:
	class CLink* m_pPlayer = { nullptr };

	_uint m_iLevelIndex = {LEVEL_END};

private:
	HRESULT Ready_Child_UI();

public:
	static CMainUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END;
