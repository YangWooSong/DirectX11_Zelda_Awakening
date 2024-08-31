#pragma once
#include "Client_Defines.h"
#include "Monster.h"

BEGIN(Engine)
class CShader;
class CModel;
class CFsm;
END

BEGIN(Client)

class COctorok :
	public CMonster
{
public:
	enum PARTID { PART_BULLET, PART_END };
	enum OCTOROK_STATE { IDLE, WALK, ATTACK, DEAD, STATE_END };
private:
	COctorok(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	COctorok(const COctorok& Prototype);
	virtual ~COctorok() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Components();
	HRESULT Ready_PartObjects();
	HRESULT Ready_State();

public:
	static COctorok* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END