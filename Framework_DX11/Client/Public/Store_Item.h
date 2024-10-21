#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
class CCollider;
class CUIObject;
END


BEGIN(Client)
class CStore_Item :
	public CGameObject
{
public:
	enum ITEM_TYPE {BOMB, SOLD_OUT, HEART, ITEM_TYPE_END};

	typedef struct : public CGameObject::GAMEOBJECT_DESC
	{
		_uint iItemType = { ITEM_TYPE_END };
	} STORE_ITEM_DESC;

private:
	CStore_Item(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CStore_Item(const CStore_Item& Prototype);
	virtual ~CStore_Item() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_LightDepth() override;

public:
	virtual void OnCollisionEnter(CGameObject* pOther)  override;
	virtual void OnCollisionStay(CGameObject* pOther) override;
	virtual void OnCollisionExit(CGameObject* pOther)  override;

public:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = {nullptr};
	CCollider* m_pColliderCom = { nullptr };
	CUIObject* m_pEffect = { nullptr };

public:
	_uint Get_ItemType() { return m_iItemType; }
	void Go_Back_OriginPos();

public:
	void Set_Picked(_bool bPick) { m_bPicked = bPick; }
private:
	_bool m_bPicked = { false };
	_bool m_bActiveEffect = { false };
	_float m_fTimer = { 0.f };
	_uint m_iItemType = { ITEM_TYPE_END };
	_float3 m_vOriginPos = {};

	const _float4x4* m_pSocketMatrix = { nullptr };
	class CLink* m_pPlayer = { nullptr };

private:
	HRESULT Ready_Components();

public:
	static CStore_Item* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END