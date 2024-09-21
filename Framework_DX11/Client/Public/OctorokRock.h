#pragma once
#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CShader;
class CModel;
class CNavigation;
class CCollider;
class CSound;
END

BEGIN(Client)
class COctorokRock :
	public CPartObject
{
public:
	typedef struct : public CPartObject::PARTOBJ_DESC
	{
		_int iCellNum;
		_uint iDir;
		class CMonster* pParent;
	} OCTOROKROCK_DESC;

public:
	enum OCTOROKROCK_DIR { FRONT, BACK, LEFT, RIGHT, OCTOROKROCK_DIR_END};
private:
	COctorokRock(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	COctorokRock(const COctorokRock& Prototype);
	virtual ~COctorokRock() = default;

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

private:
	 CShader* m_pShaderCom = { nullptr };
	 CModel* m_pModelCom = { nullptr };
	 CNavigation* m_pNavigationCom = { nullptr };
	 CCollider* m_pColliderCom = { nullptr };
	 CSound* m_pSoundCom = { nullptr };

private:
	HRESULT Ready_Components();
	void Set_CellNum(_int iNum) { m_iCellNum = iNum; }
public:
	void Shoot() { m_bShoot = true; }
	void Set_Parent(CGameObject* pParent);
private:
	_float m_fSpeed = {  };
	_uint	m_iRockDir = { OCTOROKROCK_DIR_END };
	_bool m_bShoot = { false };
	_bool	m_bIsMove = { true };		//벽에 부딪혔는지
	_bool	m_bRender = { false };	//문어 따라가기
	_float3 m_fOffset = {};

	_bool	m_bBreak = { false };
	class CMonster* m_pParent = { nullptr };

private:
	void Break();
	void Set_StartState();
public:
	static COctorokRock* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};
END
