#pragma once
#include "Client_Defines.h"
#include "Monster.h"

BEGIN(Engine)
class CShader;
class CModel;
class CFsm;
END

BEGIN(Client)

class CSpark :
	public CMonster
{
public:
	enum SPARK_STATE { IDLE, STATE_END };
private:
	CSpark(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSpark(const CSpark& Prototype);
	virtual ~CSpark() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	_int Get_MonsterNum() { return m_iMonsterNum; }
private:
	_int m_iMonsterNum = { 0 };
	CGameObject* m_pEffect[2] = {nullptr};
private:
	HRESULT Ready_Components();
	HRESULT Ready_State();

private:
	void Set_MonsterNum(void* pArg);

public:
	static CSpark* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END