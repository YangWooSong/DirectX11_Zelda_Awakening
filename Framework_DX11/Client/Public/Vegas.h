#pragma once
#include "Client_Defines.h"
#include "Monster.h"

BEGIN(Engine)
class CShader;
class CModel;
class CFsm;
END

BEGIN(Client)

class CVegas :
	public CMonster
{
public:
	enum VEGAS_STATE { IDLE, WALK_F, WALK_B, WALK_L, WALK_R, DAMAGE, DEAD, STATE_END };
private:
	CVegas(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVegas(const CVegas& Prototype);
	virtual ~CVegas() = default;

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

	_uint Get_TextureNum() { return m_iTextureNum; }
	void Set_Hide(_bool bHide) { m_bHide = bHide; }
private:
	HRESULT Ready_Components();
	HRESULT Ready_State();

	_uint	m_iTextureNum = { 0 };

	_bool   m_bHide = { false };
	_bool m_bActiveEffect = { false };

	_float m_fTimer = { 0.f };
	_float m_fEffectTimer = { 0.f };
	_float m_fDeadTimer = { 0.f };
public:
	static CVegas* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END