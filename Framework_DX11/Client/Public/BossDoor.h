#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
class CCollider;
class CSound;
END


BEGIN(Client)
class CBossDoor :
	public CGameObject
{
private:
	CBossDoor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBossDoor(const CBossDoor& Prototype);
	virtual ~CBossDoor() = default;

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
	CSound* m_pSoundCom = { nullptr };

private:
	HRESULT Ready_Components();

	_bool		m_bOpend = { false };
	_bool		m_bPlayedSound = { false };
	_int m_iCurrentAnimIndex = { 0 };
	_int m_iRemoveKeyAnimIndex = { 0 };
	_int m_iOpenAnimIndex = { 0 };
	_float m_fTimer = { 0.f };

public:
	static CBossDoor* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END