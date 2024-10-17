#pragma once
#include "Client_Defines.h"
#include "NPC.h"

BEGIN(Client)
class CMarin :
	public CNPC
{
public:
	enum MARIN_STATE { IDLE, TALK, STATE_END };
private:
	CMarin(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMarin(const CMarin& Prototype);
	virtual ~CMarin() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void OnCollisionEnter(CGameObject* pOther);
	virtual void OnCollisionStay(CGameObject* pOther);
	virtual void OnCollisionExit(CGameObject* pOther);

private:
	HRESULT Ready_Components();
	HRESULT Ready_PartObjects();
	HRESULT Ready_UIObjects();
	HRESULT Ready_State();

public:
	static CMarin* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END