#pragma once

#include "Client_Defines.h"
#include "ContainerObject.h"

BEGIN(Engine)
class CShader;
class CModel;
class CFsm;
class CNavigation;
class CCollider;
class CSound;
END

BEGIN(Client)

class CMonster :
    public CContainerObject
{
public:
	typedef struct : public CGameObject::GAMEOBJECT_DESC
	{
		_uint LevelIndex;
	}MONSTER_DESC;
public:
	enum MONSTER_DIR { FRONT, BACK, LEFT, RIGHT, MONSTER_DIR_END};
protected:
	CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonster(const CMonster& Prototype);
	virtual ~CMonster() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void OnCollisionEnter(CGameObject* pOther) {};
	virtual void OnCollisionStay(CGameObject* pOther) {};
	virtual void OnCollisionExit(CGameObject* pOther) {};
public:
	CModel* Get_Model() { return m_pModelCom; }
	CCollider* Get_Collider() { return m_pColliderCom; }

public:
	void		Go_Straight(_float fTimeDelta, _float fSpeed, CNavigation* pNavigation = nullptr, _bool* bMove = nullptr);
	void		Go_Straight_in_Room(_float fTimeDelta, _float fSpeed, CNavigation* pNavigation = nullptr);
	void		Go_Straight_in_Room_Reverse(_float fTimeDelta, _float fSpeed, _bool* bReflect = nullptr, CNavigation* pNavigation = nullptr , _int* iStopCount = nullptr);

	void		Change_State(const _uint iState);
	void		SetUp_NextAnimation(_uint iNextAnimationIndex, _float fChangeDuration = 0.2f, _bool _bLoop = false, _uint iStartFrame = 0);
	void		Set_AnimationSpeed(_uint _AnimationIndex, _double _dAnimSpeed);
	_bool       Get_IsEnd_CurrentAnimation();

	_float					Get_MoveSpeed() { return m_fMoveSpeed; }
	void					Set_MoveSpeed(_float fSpeed) { m_fMoveSpeed = fSpeed; }

	_uint		Get_Monster_Dir() { return m_iDir; }
	void		Set_Monster_Dir(_uint _iDir) { m_iDir = _iDir; }
	
	void		Set_NewRandom_Dir();
	_float3     Get_Pos() 
	{
		_float3 pos = {};
		XMStoreFloat3(&pos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		 return pos;
	}

	_float	Get_Distance(_vector _Pos1, _vector _Pos2);
	_vector Get_Pos_vector();
	CNavigation* Get_NavigationCom() { return m_pNavigationCom; }
	CFsm* Get_Fsm() { return m_pFsmCom; }
	CSound* Get_Sound() { return m_pMonsterSoundCom; }

	_int Get_CurrentCellNum(); 

	_int Get_Hp() { return m_iHp; }
	void Minus_Hp() { m_iHp--; }
	void Plus_Hp() { m_iHp++; }

protected:
	HRESULT Ready_Components();

protected:
	CModel* m_pModelCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CFsm* m_pFsmCom = { nullptr };
	CNavigation* m_pNavigationCom = { nullptr };
	CCollider* m_pColliderCom = { nullptr };
	CSound* m_pMonsterSoundCom = { nullptr };
	CGameObject* m_pEffect = { nullptr };
	CGameObject* m_p3D_Effect = { nullptr };
protected:
	_float				m_fMoveSpeed = { 3.f };
	_uint				m_iDir = { BACK };	
	_uint				m_iLevelIndex = { LEVEL_END };
	_int				m_iHp = { 0 };

public:
	static CMonster* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END