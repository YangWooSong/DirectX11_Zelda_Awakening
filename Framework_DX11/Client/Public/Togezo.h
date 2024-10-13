#pragma once
#include "Client_Defines.h"
#include "Monster.h"

BEGIN(Engine)
class CShader;
class CModel;
class CFsm;
END

BEGIN(Client)

class CTogezo :
	public CMonster
{
public:
	enum PARTID { PART_DETECTOR_HORIZON, PART_DETECTOR_VERTICAL, PART_END };
	enum  TOGEZO_STATE { IDLE, RUN, REBOUND, DISCOVER, DAMAGE, STATE_END };
private:
	CTogezo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTogezo(const CTogezo& Prototype);
	virtual ~CTogezo() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void OnCollisionEnter(CGameObject* pOther) override;
	virtual void OnCollisionStay(CGameObject* pOther) override;
	virtual void OnCollisionExit(CGameObject* pOther) override;

	_bool* Get_bDetectHor() { return &m_bHoriDetect; }
	_bool* Get_bDetectVer() { return &m_bVerDetect; }

private:
	_bool m_bVerDetect = { false };
	_bool m_bHoriDetect = { false };
	_bool	m_bBodyRed = { false };
	_bool	m_bActiveEffect = { false };
	_float	m_fRedTimer = { false };
	_float	m_fEffectTimer = { false };
private:
	HRESULT Ready_Components();
	HRESULT Ready_State();
	HRESULT Ready_PartObjects();

public:
	static CTogezo* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END