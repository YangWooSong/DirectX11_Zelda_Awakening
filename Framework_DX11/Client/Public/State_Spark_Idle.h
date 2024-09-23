#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END


BEGIN(Client)

class CState_Spark_Idle :
    public CState
{
public:
    CState_Spark_Idle(class CFsm* pFsm, class  CMonster* pOwner);
    virtual ~CState_Spark_Idle() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum) override;
    virtual HRESULT Start_State() override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pOwner = { nullptr };

    _uint               m_iCurrentAnimIndex = {};

    _float3 m_vTargetPos1[4] = { _float3(-5.f,0.f,5.f), _float3(-5.f,5.f,5.f), _float3(-10.f,5.f,5.f), _float3(-10.f, 0.f,5.f) };
    _float3 m_vTargetPos2[4] = { _float3(-5.f,0.f,5.f), _float3(-5.f,5.f,5.f), _float3(-10.f,5.f,5.f), _float3(-10.f, 0.f,5.f) };

    _float3 m_vTargetPos3[4] = { _float3(-2.11f, 0.5f, 27.91f), _float3(-2.11f, 0.5f, 24.41f), _float3(1.11f, 0.5f, 24.41f), _float3(1.11f, 0.5f, 27.91f) };
    _float3 m_vTargetPos4[4] = { _float3(2.5f, 0.5f, 20.0f), _float3(5.5f, 0.5f, 20.0f), _float3(5.5f,0.5f,23.0f), _float3(2.5f, 0.5f, 23.0f) };

    _float3 m_vTargetPos5[4] = { _float3(26.5f,0.5f,26.0f), _float3(26.5f,0.5f,22.0f), _float3(32.f,0.5f,22.0f), _float3(32.f, 0.5f, 26.0f) };
    _float3 m_vTargetPos6[4] = { _float3(24.5f,0.5f,27.86f), _float3(24.5f,0.5f,19.86f), _float3(32.3f,0.5f,19.86f), _float3(32.3f, 0.5f,27.86f) };

    vector<_float3*> m_vTargetPos = { m_vTargetPos1 , m_vTargetPos2 ,m_vTargetPos3 ,m_vTargetPos4, m_vTargetPos5, m_vTargetPos6 };

    _uint   m_iTargetIndex = { 0 };
    _int m_iMonsterNum = { 0 };
public:
    static CState_Spark_Idle* Create(class CFsm* pFsm, class CMonster* pOwner, _uint iStateNum);
    virtual void Free() override;
};

END