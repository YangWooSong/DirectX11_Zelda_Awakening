#pragma once
#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CShader;
class CModel;
class CNavigation;
class CCollider;
END

BEGIN(Client)
class CDeguTail_01 :
	public CPartObject
{
public:
	typedef struct : public CPartObject::PARTOBJ_DESC
	{
		_int iCellNum;
		_uint iBodyNum;
		_float3 vSize = {1.f,1.f,1.f};
		class CMonster* pParent;
	} DEGUBODY_DESC;

private:
	CDeguTail_01(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDeguTail_01(const CDeguTail_01& Prototype);
	virtual ~CDeguTail_01() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CNavigation* m_pNavigationCom = { nullptr };
	CCollider* m_pColliderCom = { nullptr };
private:
	HRESULT Ready_Components();
	void Set_CellNum(_int iNum) { m_iCellNum = iNum; }
public:
	void Set_Parent(CGameObject* pParent);
private:
	_float m_fSpeed = {  };
	class CMonster* m_pParent = { nullptr };
	_uint	m_iBodyNum = { 0 };
public:
	static CDeguTail_01* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};
END
