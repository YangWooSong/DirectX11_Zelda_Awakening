#pragma once
#include "Client_Defines.h"
#include "Monster.h"

BEGIN(Engine)
class CShader;
class CModel;
class CFsm;
END

BEGIN(Client)

class CKuribo :
	public CMonster
{
public:
	enum KURIBO_STATE { IDLE, WALK, WALK_SHAKE, DEAD, STATE_END };
private:
	CKuribo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CKuribo(const CKuribo& Prototype);
	virtual ~CKuribo() = default;

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

	_bool	m_bShake = { false };
public:
	static CKuribo* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END