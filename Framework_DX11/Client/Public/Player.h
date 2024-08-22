#pragma once

#include "Client_Defines.h"
#include "ContainerObject.h"

BEGIN(Engine)
class CShader;
class CModel;
class CFsm;
END

BEGIN(Client)

class CPlayer : public CContainerObject
{
public:
	enum PARTID { PART_EFFEXT, PART_END };
	enum PLAYER_DIR{FRONT, LEFT,LEFTUFRONT, LEFTBACK, BACK, RIGHT,RIGHTFRONT,RIGHTBACK, PLAYER_DIR_END};
	typedef struct : public CGameObject::GAMEOBJECT_DESC
	{
		_uint LevelIndex;
	}PLAYER_DESC;
	

public:
	class CModel*			Get_Model() { return m_pModelCom; }

	_float					Get_MoveSpeed() { return m_fMoveSpeed; }
	void					Set_MoveSpeed(_float fSpeed) { m_fMoveSpeed = fSpeed; }

protected:
	CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer(const CPlayer& Prototype);
	virtual ~CPlayer() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	HRESULT		Ready_PartObjects();

	void		Move_Dir(_fvector vDir, _float fSpeed, _float fTimeDelta);
	void		Go_Straight(_float fTimeDelta, _float fSpeed);
	void		Go_Backward(_float fTimeDelta, _float fSpeed);
	void		Go_Right(_float fTimeDelta, _float fSpeed);
	void		Go_Left(_float fTimeDelta, _float fSpeed);

	void		Go_World_Straight(_float fTimeDelta, _float fSpeed = 0.f);
	void		Go_World_Backward(_float fTimeDelta, _float fSpeed = 0.f);
	void		Go_World_Right(_float fTimeDelta, _float fSpeed = 0.f);
	void		Go_World_Left(_float fTimeDelta, _float fSpeed = 0.f);

	void		Go_Stair_Down(_float fTimeDelta, _float fSpeed = 0.f);
	void		Go_Stair_Up(_float fTimeDelta, _float fSpeed = 0.f);

	void		Change_State(const _uint iState);
	void		SetUp_NextAnimation(_uint iNextAnimationIndex, _float fChangeDuration = 0.2f, _bool _bLoop = false);
	_bool       Get_IsEnd_CurrentAnimation();

	_vector		Calculate_Direction_Straight();

	void		Set_Player_Dir(_uint _Dir) { m_ePlayer_Dir = _Dir; }
	_uint		Get_Player_Dir() { return m_ePlayer_Dir; }

	_vector		Get_Position() { return m_pTransformCom->Get_State(CTransform::STATE_POSITION); }
	_uint		Get_LevelIndex() { return m_iLevelIndex; }

	_uint		Get_Player_State();

	const _float4x4*    Get_BoneMatrix_Ptr(const _char* pBoneName) const;

protected:
	class CModel*		m_pModelCom = { nullptr };
	class CShader*		m_pShaderCom = { nullptr };
	class CFsm*			m_pFsmCom = { nullptr };

protected:
	_float				m_fMoveSpeed = { 3.f};
	_float				m_vTargetPos = {};

	_uint				m_ePlayer_Dir = { PLAYER_DIR_END };
	_uint				m_iLevelIndex = { LEVEL_END };
protected:
	HRESULT Ready_Components();

public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END