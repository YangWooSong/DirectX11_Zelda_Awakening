#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
class CCollider;
END


BEGIN(Client)
class CCrackedWall :
	public CGameObject
{
private:
	CCrackedWall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCrackedWall(const CCrackedWall& Prototype);
	virtual ~CCrackedWall() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void OnCollisionEnter(CGameObject* pOther)  override;
	virtual void OnCollisionStay(CGameObject* pOther) override;
	virtual void OnCollisionExit(CGameObject* pOther)  override;

public:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CCollider* m_pColliderCom = { nullptr };

private:
	_bool m_bCracked = { false };

private:
	HRESULT Ready_Components();

public:
	static CCrackedWall* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END