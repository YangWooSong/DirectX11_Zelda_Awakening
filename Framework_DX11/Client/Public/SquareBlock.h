#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
class CCollider;
END


BEGIN(Client)
class CSquareBlock :
	public CGameObject
{
public:
	typedef struct : public CGameObject::GAMEOBJECT_DESC
	{
		_bool bActiveCollider = false;
	} SQUARE_DESC;
private:
	CSquareBlock(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSquareBlock(const CSquareBlock& Prototype);
	virtual ~CSquareBlock() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	_bool Get_IsPushed() { return m_bPushed; }
	void Set_IsPushed(_bool bPushed) { m_bPushed = bPushed; }
	_bool Get_isStopMove() { return m_bStopMove; }
public:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CCollider* m_pColliderCom = { nullptr };

private:
	_bool m_bColliderActive = { false };
	_bool m_bPushed = { false };
	_bool m_bStopMove = { false };

	_uint m_iDir = { 0 };
	_float3 m_vTarget = { -18.72f, 0.f, 48.72f };

	class CLink* m_pPlayer = { nullptr };

private:
	HRESULT Ready_Components();
	void Move(_float fTimeDelta);
public:
	static CSquareBlock* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END