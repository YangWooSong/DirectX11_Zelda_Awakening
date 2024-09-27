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

public:
	virtual void OnCollisionEnter(CGameObject* pOther)  override;
	virtual void OnCollisionStay(CGameObject* pOther) override;
	virtual void OnCollisionExit(CGameObject* pOther)  override;

public:
	void Reset_RedCount() { m_iRedCount = 2; }
	void Set_Blink(_bool _bBlink) { m_bBlink = _bBlink; }
	_int* Get_AddDir() { return &m_iAddDir; }
	_bool* Get_Arrived() { return &m_bArrived; }
	_uint* Get_TargetPosIndex() { return &m_iTargetPosIndex; }

private:
	HRESULT Ready_Components();
	HRESULT Ready_State();

private:
	_uint		m_iRedCount = { 0 };
	_uint		m_iTargetPosIndex = { 0 };
	_int		m_iAddDir = { -1 };			//인덱스 늘릴때 쓰는 방향 변수 + -1은 오른쪽으로 이동, 1은 왼쪽으로 이동

	_float		m_fTimer = { 0.f };
	_float		m_fNonRedTimer = { 0.f };
	_bool		m_bBlink = { false };
	_bool		m_bArrived = { true };

public:
	static CRola* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END