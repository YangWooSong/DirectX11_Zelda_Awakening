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
private:
	HRESULT Ready_Components();
	HRESULT Ready_PartObjects();
	HRESULT Ready_State();
	HRESULT Ready_PlayerUI();

	class CItem* m_pItemA = { nullptr };
	class CItem* m_pItemB = { nullptr };
	CGameObject* m_pCarryitem = { nullptr };

	_bool m_bActiveSheild = { true };
	_bool m_bActiveSword = { false };

	_float m_fBlinkTimer = { 0.f };
	_int m_iBlinkCount = { 0};
	
	_int m_iLupee = { 0 };
	_int m_iSmallKeyCount = { 0 };
	_bool m_bDungoneKey = { false };
	_bool m_bGetBossKey = { false };
	_bool m_bCampus = { false };
	_bool m_bBeak = { true };
	_bool m_bFeather = { false };
	_bool m_bMap = { false };

	//class CInteractUI* m_pInteractUI = { nullptr };
	//class CItemUI* m_pItemtUI = { nullptr };

public:
	static CLink* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END