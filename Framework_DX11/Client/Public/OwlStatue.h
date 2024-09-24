#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
class CCollider;
END


BEGIN(Client)
class COwlStatue :
	public CGameObject
{
private:
	COwlStatue(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	COwlStatue(const COwlStatue& Prototype);
	virtual ~COwlStatue() = default;

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
	class CCollider* m_pColliderCom = { nullptr };

private:
	_bool m_bInteract = { false };

private:
	HRESULT Ready_Components();

public:
	static COwlStatue* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END