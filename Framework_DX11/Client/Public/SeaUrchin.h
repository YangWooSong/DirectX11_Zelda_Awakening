#pragma once
#include "Client_Defines.h"
#include "Monster.h"

BEGIN(Engine)
class CShader;
class CModel;
class CFsm;
END

BEGIN(Client)

class CSeaUrchin :
    public CMonster
{
public:
	enum SEAURCHIN_STATE { IDLE, PUSHED, DEAD, STATE_END };
private:
	CSeaUrchin(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSeaUrchin(const CSeaUrchin& Prototype);
	virtual ~CSeaUrchin() = default;

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

public:
	void Set_Render(_bool bRender) { m_bRender = bRender; }
private:
	HRESULT Ready_Components();
	HRESULT Ready_State();

	void Shrink(float fTimeDelta);

private:
	_float m_fShrinkTime = { 0.f };
	_float3 m_fOrginScale = { };
	_float3 m_fShrinklScale = { };

	_bool	m_bBodyRed = { false };
	_bool	m_bRender = { true };
	_bool	m_bEffect = { false };
public:
	static CSeaUrchin* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END