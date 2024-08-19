#pragma once
#include "Client_Defines.h"
#include "Monster.h"

BEGIN(Engine)
class CShader;
class CModel;
class CFsm;
END

BEGIN(Client)

class CRola :
    public CMonster
{
public:
	enum ROLA_STATE { IDLE, JUMP, PUSH, DEMAGE, DEAD, STATE_END };
private:
	CRola(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRola(const CRola& Prototype);
	virtual ~CRola() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	void Reset_RedCount() { m_iRedCount = 2; }
	void Set_Blink(_bool _bBlink) { m_bBlink = _bBlink; }
private:
	HRESULT Ready_Components();
	HRESULT Ready_State();

private:
	_uint		m_iRedCount = { 0 };
	_float		m_fTimer = { 0.f };
	_float		m_fNonRedTimer = { 0.f };
	_bool		m_bBlink = { false };
public:
	static CRola* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END