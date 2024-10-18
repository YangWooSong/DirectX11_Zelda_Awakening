#pragma once
#include "Client_Defines.h"
#include "NPC.h"

BEGIN(Client)
class CTarin :
	public CNPC
{
private:
	CTarin(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTarin(const CTarin& Prototype);
	virtual ~CTarin() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_int m_iAnimIndex = { 0 };
private:
	HRESULT Ready_Components();
	HRESULT Ready_PartObjects();

public:
	static CTarin* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END