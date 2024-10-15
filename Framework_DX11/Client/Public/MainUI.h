#pragma once
#include "Client_Defines.h"
#include "UIObject.h"

BEGIN(Client)
class CMainUI :
	public CUIObject
{
public:
	enum CHILD_INDEX { LUPEE,HP, KEY, CAMPUS, INVEN, MAP, DIALOGUE, FADE_IN_OUT, CHILD_END };

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
	_int Get_Player_KeyCount();

	_bool Get_Player_Get_BossKey();
	_bool Get_Bomb();
	_bool Get_Feather();
	_bool Get_Map();
	_bool Get_Active_Campus() { return m_bActive_Campus; }
	_bool Get_isFinishFadeIn() { return m_isFinishFadeIn; }
	_bool Get_isFinishFadeOut() { return m_isFinishFadeOut; }

	_uint Get_LevelIndex() { return m_iLevelIndex; }

	_float3 Get_PlayerPos_Float3();

	void SEt_Active_Campus(_bool m_bActive) { m_bActive_Campus = m_bActive; }
	void Active_LevelText() { m_bRenderLevelText = true; }
	void Active_BossNameText() { m_bRenderBossName = true; }

	void Active_FadeIn();
	void Active_FadeOut();

	void Set_isFinishFadeIn(_bool bFinishIn) { m_isFinishFadeIn = bFinishIn; }
	void Set_isFinishFadeOut(_bool bFinishOut) { m_isFinishFadeOut = bFinishOut; }
private:
	class CLink* m_pPlayer = { nullptr };

	_bool m_bActive_Campus = { false };
	_bool m_bRenderLevelText = { false };
	_bool m_bRenderBossName = { false };
	_bool m_isFinishFadeIn = { false };
	_bool m_isFinishFadeOut = { false };

	_float m_fTextAlpha = { 0.f };
	_float m_fTextTimer = { 0.f };

	_uint m_iLevelIndex = {LEVEL_END};

private:
	HRESULT Ready_Child_UI();

public:
	static CMainUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END;
