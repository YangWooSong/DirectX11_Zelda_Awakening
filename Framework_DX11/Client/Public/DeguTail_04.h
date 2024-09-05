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
class CDeguTail_04 :
	public CPartObject
{
public:
	typedef struct : public CPartObject::PARTOBJ_DESC
	{
		_int iCellNum;
		_float	fWaitTime = 0.f;
		_float3 vSize = { 1.f,1.f,1.f };
		class CGameObject* pParent;
	} DEGUTAIL_DESC;

private:
	CDeguTail_04(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDeguTail_04(const CDeguTail_04& Prototype);
	virtual ~CDeguTail_04() = default;

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

	_float			m_fTimer = { 0.f };
	_float			m_fWaitTime = { 0.f };
	_float3			m_fSize = { };
	_bool			m_bMove = { false };

	class CGameObject* m_pParent = { nullptr };
	vector<_matrix> m_pParentWorldMatrixVector;
public:
	static CDeguTail_04* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};
END
