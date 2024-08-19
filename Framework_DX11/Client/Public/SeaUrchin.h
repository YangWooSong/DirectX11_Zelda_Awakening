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

private:
	HRESULT Ready_Components();
	HRESULT Ready_State();

	void Shrink(float fTimeDelta);

private:
	_float m_fShrinkTime = { 0.f };
	_float3 m_fOrginScale = { };
	_float3 m_fShrinklScale = { };

public:
	static CSeaUrchin* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END