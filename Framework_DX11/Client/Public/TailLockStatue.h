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

class CTailLockStatue :
	public CGameObject
{
private:
	CTailLockStatue(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTailLockStatue(const CTailLockStatue& Prototype);
	virtual ~CTailLockStatue() = default;

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
	_int m_iCurrentAnimIndex = { 0 };

	_bool m_bOpened = { false };
	_bool m_bOpenSound = { false };
	_bool m_bHideFront = { false };
	_bool m_bCameraSet = { false };
	
	_float m_fAlpha = { 1.f };

	_float m_fOpenedTimer = { 0.f };
	_float m_fAfterSoundTimer = { 0.f };

	CGameObject* m_pEffect = { nullptr };

private:
	HRESULT Ready_Components();
	void Camera_Set();
	void Shutter_Set();
public:
	static CTailLockStatue* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END