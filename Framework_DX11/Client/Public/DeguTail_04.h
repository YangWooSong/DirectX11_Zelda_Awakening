#pragma once
#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CShader;
class CModel;
class CNavigation;
class CCollider;
class CFsm;
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
		CFsm* pFsm = { nullptr };
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
	virtual void OnCollisionEnter(CGameObject* pOther) override;
	virtual void OnCollisionStay(CGameObject* pOther) override;
	virtual void OnCollisionExit(CGameObject* pOther) override;

public:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CNavigation* m_pNavigationCom = { nullptr };
	CCollider* m_pColliderCom = { nullptr };
private:
	HRESULT Ready_Components();
	void Set_CellNum(_int iNum) { m_iCellNum = iNum; }
	void Set_Animation();

public:
	void Set_Parent(CGameObject* pParent);
	void Set_Render(_bool bRender) { m_bRender = bRender; }

private:
	_uint			m_iWalkAnimIndex = { 0 };
	_uint			m_iGuardAnimIndex = { 0 };
	_uint			m_iHurtAnimIndex = { 0 };
	_uint			m_iDeadAnimIndex = { 0 };
	_uint			m_iAppearAnimIndex = { 0 };

	_uint			m_iCurrentAnimIndex = { 0 };

	_float			m_fTimer = { 0.f };
	_float			m_fWaitTime = { 0.f };
	_float3			m_fSize = { };
	_bool			m_bMove = { false };
	_bool			m_bRender= { false };

	class CGameObject* m_pParent = { nullptr };
	class CGameObject* m_pEffect = { nullptr };
	vector<_matrix> m_pParentWorldMatrixVector;

	CFsm* m_pHeadFsm = { nullptr };
public:
	static CDeguTail_04* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};
END
