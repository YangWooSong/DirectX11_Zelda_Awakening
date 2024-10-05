#pragma once
#include "Client_Defines.h"
#include "Player.h"

BEGIN(Engine)
class CShader;
class CModel;
class CFsm;
class CSound;
END

BEGIN(Client)

class CLink :
    public CPlayer
{
public:
	enum LINKSTATE{IDLE, WALK, ITEMA, ITEMB, SLASH, SLASH_HOLD, JUMP, SHIELD, SHIELD_WALK, STAIR_UP, STAIR_DOWN, FALL, DAMAGE_FRONT, CARRY, CARRY_WALK,THROW, GET_ITEM, BOMB, KEY, PUSH, LINK_STATE_END};
	enum UI{INTERACT_UI, ITEM_ICON_UI, UI_END};
private:
	CLink(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLink(const CLink& Prototype);
	virtual ~CLink() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_LightDepth() override;

public:
	virtual void OnCollisionEnter(CGameObject* pOther)override;
	virtual void OnCollisionStay(CGameObject* pOther)override;
	virtual void OnCollisionExit(CGameObject* pOther)override;

public:
	class CItem* Get_ItemA() { return m_pItemA; }
	class CItem* Get_ItemB() { return m_pItemB; }
	_bool IsGetCampus() { return m_bCampus; }

	CGameObject* Get_CarryItem() { return m_pCarryitem; }
	void Set_CarryItem(CGameObject* pObject) { m_pCarryitem = pObject; }

	_bool Get_ActiveSword() { return m_bActiveSword; }
	void Set_ActiveSword(_bool bActive) { m_bActiveSword = bActive; }
	void Set_ActiveSheild(_bool bActive) { m_bActiveSheild = bActive; }
	void Get_Item(_uint iItemIndex);

	_int Get_SmallKeyCount() { return m_iSmallKeyCount; }
	void Minus_SmallKeyCount() {  m_iSmallKeyCount--; }
	_bool Get_isGetBeak() { return m_bBeak; }

	void Set_Talk(_bool bTalk) { m_bTalk = bTalk; }
	_bool Get_isTalk() { return m_bTalk; }

	void Set_Bomb(_bool bBomb) { m_bBomb = bBomb; }
	_bool Get_Bomb() {return m_bBomb;	}

	_int Get_Lupee() { return m_iLupee; }
	void Add_Lupee(_int iNum) { m_iLupee += iNum; }

	_int Get_Player_MaxHP() {	return m_iMaxHp	;	}
	_int Get_Player_CurrHP() {	return m_iCurrentHP;	}
	void Add_MaxHp() { m_iMaxHp++; m_iCurrentHP = m_iMaxHp; }
	_bool Get_isGet_BossKey() { return  m_bGetBossKey; }
	_bool Get_Feather() { return  m_bFeather; }
	_bool Get_Map() { return m_bMap; }
private:
	HRESULT Ready_Components();
	HRESULT Ready_PartObjects();
	HRESULT Ready_State();
	HRESULT Ready_PlayerUI();

	class CItem* m_pItemA = { nullptr };
	class CItem* m_pItemB = { nullptr };
	CGameObject* m_pCarryitem = { nullptr };

	static _bool m_bActiveSheild ;
	static _bool m_bActiveSword ;

	_float m_fBlinkTimer = { 0.f };
	_int m_iBlinkCount = { 0};
	
	static _int m_iMaxHp ;
	static _int m_iCurrentHP ;
	static _int m_iLupee ;
	_int m_iSmallKeyCount = { 0 };

	_bool m_bDungeonKey = { true };
	_bool m_bGetBossKey = { false };
	_bool m_bCampus = { false };
	_bool m_bBeak = { true };
	_bool m_bMap = { true };
	_bool m_bTalk = { false };
	static _bool m_bBomb;
	static _bool m_bFeather ;
	//class CInteractUI* m_pInteractUI = { nullptr };
	//class CItemUI* m_pItemtUI = { nullptr };

public:
	static CLink* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END