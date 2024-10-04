#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CCollider;
END

BEGIN(Client)

class CTeleport final : public CGameObject
{
public:
	typedef struct : public CGameObject::GAMEOBJECT_DESC
	{
		_uint iNextLevelIndex;

	}TELEPORT_DESC;

private:
	CTeleport(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTeleport(const CTeleport& Prototype);
	virtual ~CTeleport() = default;

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
	_bool Get_Change_Level() { return m_bChangeScene; }
	_uint Get_NextLevel() { return m_iNextLevelIndex; }
private:
	CCollider* m_pColliderCom = { nullptr };

private:
	_bool m_bChangeScene = { false };
	_uint m_iNextLevelIndex = { LEVEL_END };
private:
	HRESULT Ready_Components();

public:
	static CTeleport* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END