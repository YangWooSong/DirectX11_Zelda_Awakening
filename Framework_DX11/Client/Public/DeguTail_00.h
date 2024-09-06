#pragma once
#include "Client_Defines.h"
#include "Monster.h"

BEGIN(Engine)
class CShader;
class CModel;
class CFsm;
END

BEGIN(Client)
class CDeguTail_00 :
    public CMonster
{
public:
	enum PARTID { PART_BODY1, PART_BODY2, PART_BODY3, PART_TAIL, PART_END };
	enum DEGUTAIL_STATE { WALK, GUARD, HURT, DEAD, APPEAR, IDLE, STATE_END };
private:
	CDeguTail_00(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDeguTail_00(const CDeguTail_00& Prototype);
	virtual ~CDeguTail_00() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Components();
	HRESULT Ready_PartObjects();
	HRESULT Ready_State();

public:
	void Add_Vec_Matrix();	
	void Vec_PopBackp() { m_MParentWorldMarix.pop_back(); }
	vector<_matrix>& Get_Parent_WorlMatrix_Vector(){return  m_MParentWorldMarix;}

	void Set_Angry(_bool bAngry) { m_bAngry = bAngry; }
	_bool Get_Angry() { return m_bAngry; }

	void Kill_Parts(_float fTimeDelta);

private:
	vector<_matrix> m_MParentWorldMarix;
	_bool			m_bAngry = { false };
	_float fTimer = { 0.f };
	_int m_iPartIndex = { PART_END };
public:
	static CDeguTail_00* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};


END