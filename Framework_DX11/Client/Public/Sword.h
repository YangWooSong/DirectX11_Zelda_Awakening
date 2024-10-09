#pragma once
#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CCollider;
class CFsm;
END

BEGIN(Client)

class CSword final : public CPartObject
{
public:
	typedef struct : public CPartObject::PARTOBJ_DESC
	{
		CFsm* pPlayerFsm;
		const _float4x4* pSocketBoneMatrix = { nullptr };
	}SWORD_DESC;

private:
	CSword(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSword(const CSword& Prototype);
	virtual ~CSword() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CCollider* m_pColliderCom = { nullptr };
	CFsm* m_pPlayerFsm = { nullptr };
private:
	const _float4x4* m_pSocketMatrix = { nullptr };
	CGameObject* m_pEffect = { nullptr };
private:
	HRESULT Ready_Components();

public:
	static CSword* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END