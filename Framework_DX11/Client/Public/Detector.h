#pragma once
#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CCollider;
class CFsm;
END

BEGIN(Client)

class CDetector final : public CPartObject
{
public:
	typedef struct : public CPartObject::PARTOBJ_DESC
	{
		CFsm* pOwnerFsm;
	}DETECTOR_DESC;

private:
	CDetector(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDetector(const CDetector& Prototype);
	virtual ~CDetector() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Set_Active_Collider(_bool bActive);
private:
	CCollider* m_pColliderCom = { nullptr };
	CFsm* m_pOwnerFsm = { nullptr };
private:
	const _float4x4* m_pSocketMatrix = { nullptr };
private:
	HRESULT Ready_Components();

public:
	static CDetector* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END