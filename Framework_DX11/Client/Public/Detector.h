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
		_bool* pDetect = { nullptr };
		_float3 vSize = {};
		_float3 vOffset= {};
		class CTransform* pTransform = { nullptr };
		_bool onlyPos = false;

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
	virtual void OnCollisionEnter(CGameObject* pOther) override;
	virtual void OnCollisionStay(CGameObject* pOther) override;
	virtual void OnCollisionExit(CGameObject* pOther) override;

public:
	void Set_Active_Collider(_bool bActive);
private:
	CCollider* m_pColliderCom = { nullptr };
	CFsm* m_pOwnerFsm = { nullptr };
	CTransform* m_pOwnerTransform = { nullptr };

private:
	const _float4x4* m_pSocketMatrix = { nullptr };
	_bool* m_pDetect = { nullptr };
	_float3 m_vOffset = { };
	_float3 m_vSize = { };
	_bool m_bOnlyPos = { false };
 private:
	HRESULT Ready_Components();

public:
	static CDetector* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END