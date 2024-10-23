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
class CRollingSpike :
	public CGameObject
{
private:
	CRollingSpike(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRollingSpike(const CRollingSpike& Prototype);
	virtual ~CRollingSpike() = default;

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


	void Start_Move_Right() { m_bMoveRight = true; }
	void Start_Move_Left() { m_bMoveLeft = true; }
	void Set_Dissolve(_bool _bActive) { m_bActiveDissolve = _bActive; }
public:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CCollider* m_pColliderCom = { nullptr };
	CSound* m_pSoundCom = { nullptr };

private:
	_int m_iCurrentAnimIndex = { 0 };

	_bool m_bMoveRight = { false };
	_bool m_bMoveLeft = { false };
	_bool m_bActiveDissolve = { false };
	_bool m_bCreateEffect = { false };

	_float m_fMoveTimer = { 0.f };
	_float m_fRotateSpeed = { 0.f };
	_float m_fMoveSpeed = { 0.f };
	_float m_fAlpha = { 0.f };		//µðÁ¹ºê °ª

	CGameObject* m_pEffect = { nullptr };
private:
	HRESULT Ready_Components();

	void Move_Right(_float fTimeDelta);
	void Move_Left(_float fTimeDelta);
public:
	static CRollingSpike* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END