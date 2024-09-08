#pragma once

#include "Base.h"
#include "Transform.h"

/* ��� ���ӳ��� ���Ǵ� ���� ������Ʈ���� �θ� Ŭ������. */

BEGIN(Engine)

class ENGINE_DLL CGameObject abstract : public CBase
{
public:
	enum OBJ_TYPE { ANIM_MONSTER, ANIM_NPC, ANIM_OBJ, NONANIM_LEVEL, NONANIM_OBJ, NONANIM_LEVEL_DUNGEON, NONANIM_LEVEL_FIELD, TYPE_END };
	typedef struct : public CTransform::TRANSFORM_DESC
	{
		OBJ_TYPE eType = TYPE_END;
		_uint listIndex = 100;
		_float3		vPosition = _float3(0.f, 0.f, 0.f);
		_float3		vScale = _float3(1.f, 1.f, 1.f);
		_float3		vRotation = _float3(0.f, 0.f, 0.f);
		_int		iCellNum = -1;
	} GAMEOBJECT_DESC;

protected:
	CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject(const CGameObject& Prototype);
	virtual ~CGameObject() = default;

public:
	_bool		Get_Dead() { return m_isDead; }
	void		Set_Dead(_bool isDead) { m_isDead = isDead; }

	_bool		IsActive() { return m_isActive; }
	void		IsActive(_bool isActive) { m_isActive = isActive; }

	class CTransform* Get_Transform() {
		if (m_pTransformCom)
			return m_pTransformCom;
		return nullptr;
	}

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg = nullptr);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

public:
	class CComponent* Find_Component(const _wstring& strComponentTag);
	class CComponent* Find_Component(const COMPONENT_TYPE eType);

	OBJ_TYPE Get_ObjType() { return m_eObjType; }
	_uint Get_ListIndex() { return m_iListIndex; }
	_float3 Get_Rotation() { return m_vRot; }
	void Set_Rotation(_float3 _rot) {  m_vRot = _rot; }

public:
	virtual void OnCollisionEnter(CGameObject* pOther) {};
	virtual void OnCollisionStay(CGameObject* pOther) {};
	virtual void OnCollisionExit(CGameObject* pOther) {};

protected:
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };

	class CTransform*			m_pTransformCom = { nullptr };
	class CGameInstance*		m_pGameInstance = { nullptr };

protected:
	map<const _wstring, class CComponent*>			m_Components;

protected:
	_bool						m_isActive = { true };
	_bool						m_isCloned = { false };
	_bool						m_isDead = { false };

	OBJ_TYPE					m_eObjType = { TYPE_END };
	_uint						m_iListIndex = { 0 };
	_float3						m_vRot = {};
	_int						m_iCellNum = {};
protected:
	HRESULT Add_Component(_uint iLevelIndex, const _wstring& strPrototypeTag, const _wstring& strComponentTag, CComponent** ppOut, void* pArg = nullptr);


public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END