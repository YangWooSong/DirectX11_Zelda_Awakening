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
class CDeguTail_01 :
	public CPartObject
{
public:
	enum DEGUBODY_STATE { WALK, GUARD, HURT, DEAD, STATE_END };
	typedef struct : public CPartObject::PARTOBJ_DESC
	{
		_int iCellNum;
		_float	fWaitTime = 0.f;
		_float3 vSize = {1.f,1.f,1.f};
		class CGameObject* pParent;
		CFsm* pFsm = { nullptr };
		_bool* pBodyRed = { nullptr };
		_bool* pOutBodyRed = { nullptr };
		_bool* pBlink = { nullptr };
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
	virtual HRESULT Render_LightDepth() override;

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
public:
	void Add_Vec_Matrix();
	void Vec_PopBackp() { m_MParentWorldMarix.pop_back(); }
	vector<_matrix>& Get_Parent_WorlMatrix_Vector() { return  m_MParentWorldMarix; }
private:
	vector<_matrix> m_MParentWorldMarix;

	_uint			m_iWalkAnimIndex = { 0 };
	_uint			m_iGuardAnimIndex = { 0 };
	_uint			m_iHurtAnimIndex = { 0 };
	_uint			m_iDeadAnimIndex = { 0 };
	_uint			m_iAppearAnimIndex = { 0 };
	_uint			m_iCurrentAnimIndex = { 0 };


	_float			m_fTimer = { 0.f };
	_float			m_fWaitTime = { 0.f };
	_float			m_fEffectTimer = { 0.f };

	_float3			m_fSize = { };

	_bool			m_bMove = { false };
	_bool			m_bRender = { false };
	_bool			m_bActiveEffect = { false };

	_bool*			m_pOutBodyRed = { nullptr };
	_bool*			m_pBodyRed = { nullptr };
	_bool*			m_pBlink = { nullptr };

	class CGameObject* m_pParent = { nullptr };
	class CGameObject* m_pEffect = { nullptr };
	class CGameObject* m_p3DEffect = { nullptr };
	vector<_matrix> m_pParentWorldMatrixVector;

	CFsm* m_pHeadFsm = { nullptr };
public:
	static CDeguTail_01* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};
END
