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
	enum VEGAS_STATE { IDLE, STATE_END };
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

private:
	HRESULT Ready_Components();
	HRESULT Ready_State();

	class CModel* m_pModelComs[4] = {};

	_uint			m_iModelIndex = { 0 };
public:
	static CVegas* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END