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

private:
	HRESULT Ready_Components();
	HRESULT Ready_State();

public:
	static CTogezo* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END