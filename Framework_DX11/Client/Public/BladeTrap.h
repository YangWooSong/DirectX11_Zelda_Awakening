#pragma once
#include "Client_Defines.h"
#include "ContainerObject.h"

BEGIN(Engine)
class CShader;
class CModel;
class CCollider;
class CNavigation;
class CSound;
END


BEGIN(Client)
class CBladeTrap :
	public CContainerObject
{
public:
	enum PARTID { PART_DETECTOR_HORIZON, PART_DETECTOR_VERTICAL, PART_END };
	enum DIR {RIGHT, LEFT, DOWN, UP, DIR_END};
private:
	CBladeTrap(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBladeTrap(const CBladeTrap& Prototype);
	virtual ~CBladeTrap() = default;

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
	CCollider* m_pColliderCom = { nullptr };
	CNavigation* m_pNavigationCom = { nullptr };
	CSound* m_pSoundCom = { nullptr };

private:
	_int m_iCurrentAnimIndex = { 0 };
	_int m_fStopCount = { 0 };

	_uint m_iDir = { DIR_END };

	_bool m_bVerDetect = { false };
	_bool m_bHoriDetect = { false };
	_bool m_bMove = { true };
	_bool m_bGoBack = { false };
	_bool m_bPlaySoundMove = { false };

	_float m_fSpeed= { 5.f };
	_float m_fWaitTimer= { 0.f };
         
	_vector m_vOriPos = {};
private:
	HRESULT Ready_Components();
	HRESULT Ready_PartObjects();

	void Move(_float fTimeDelta);
	void Move_Reverse(_float fTimeDelta);
	void Set_Dir();

public:
	static CBladeTrap* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END