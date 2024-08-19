#pragma once
#include "Client_Defines.h"
#include "Player.h"

BEGIN(Engine)
class CShader;
class CModel;
class CFsm;
END

BEGIN(Client)

class CLink :
    public CPlayer
{
public:
	enum LINKSTATE{IDLE, WALK, ITEMA, ITEMB, SLASH, JUMP, SHIELD, SHIELD_WALK, STAIR_UP, STAIR_DOWN, LINK_STATE_END};
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
	class CItem* Get_ItemA() { return m_pItemA; }
	class CItem* Get_ItemB() { return m_pItemB; }

private:
	HRESULT Ready_Components();
	HRESULT Ready_State();

	class CItem* m_pItemA = { nullptr };
	class CItem* m_pItemB = { nullptr };
public:
	static CLink* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END