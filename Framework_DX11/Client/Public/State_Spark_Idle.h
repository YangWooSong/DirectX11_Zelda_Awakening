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

    _float3 m_vTargetPos1[14] = 
    { _float3(-12.76f, 0.f,50.29f), _float3(-12.76f, 0.f, 51.74f), _float3(-9.65f, 0.f, 51.74f), _float3(-9.65f, 0.f,50.39f) , _float3(-8.1f, 0.f,50.39f), 
      _float3(-8.1f, 0.f, 47.286f), _float3(-11.16f, 0.f, 47.286f), _float3(-11.16f, 0.f, 48.68f), _float3(-11.16f, 0.f, 47.286f) , _float3(-14.35f, 0.f,47.286f),
      _float3(-14.35f, 0.f, 48.63f), _float3(-15.79f, 0.f, 48.63f), _float3(-15.79f, 0.f, 51.63f), _float3(-12.76f, 0.f, 51.74f) 
    };
    _float3 m_vTargetPos2[8] = 
    { _float3(-3.71f,0.f,50.32f), _float3(-3.71f,0.f,51.62f), _float3(-0.76f,0.f,51.62f), _float3(-0.76f, 0.f,48.77f),
        _float3(-2.26f,0.f,48.77f), _float3(-2.26f,0.f,47.23f), _float3(-5.21f,0.f,47.23f), _float3(-5.21f, 0.f,50.13f)
    };
    _float3 m_vTargetPos3[8] = 
    { _float3(2.2f,0.f,47.29f), _float3(2.2f,0.f,48.74f), _float3(5.25f,0.f,48.74f), _float3(5.25f, 0.f,45.75f) ,
         _float3(3.85f,0.f,45.75f), _float3(3.85f,0.f,44.29f), _float3(0.9f,0.f,44.29f), _float3(0.9f, 0.f,47.f)
    };

    _float3 m_vTargetPos4[4] = { _float3(-2.11f, 0.5f, 27.91f), _float3(-2.11f, 0.5f, 24.41f), _float3(1.11f, 0.5f, 24.41f), _float3(1.11f, 0.5f, 27.91f) };
    _float3 m_vTargetPos5[4] = { _float3(2.5f, 0.5f, 20.0f), _float3(5.5f, 0.5f, 20.0f), _float3(5.5f,0.5f,23.0f), _float3(2.5f, 0.5f, 23.0f) };

    _float3 m_vTargetPos6[4] = { _float3(26.5f,0.5f,26.0f), _float3(26.5f,0.5f,22.0f), _float3(32.f,0.5f,22.0f), _float3(32.f, 0.5f, 26.0f) };
    _float3 m_vTargetPos7[6] = { _float3(24.5f,0.5f,27.86f), _float3(24.5f,0.5f,19.86f), _float3(35.37f,0.5f,19.86f), _float3(32.37f, 0.5f,19.86f), _float3(32.37f, 0.5f,27.66f), _float3(35.27f, 0.5f,27.66f) };

    vector<_float3*> m_vTargetPos = { m_vTargetPos1 , m_vTargetPos2 ,m_vTargetPos3 ,m_vTargetPos4, m_vTargetPos5, m_vTargetPos6, m_vTargetPos7 };
    _int m_TargetListSize[7] = { 14,8,8,4,4,4,6 };


    _uint   m_iTargetIndex = { 0 };
    _int m_iMonsterNum = { 0 };

    class CLink* m_pPlayer = { nullptr };
public:
    static CState_Spark_Idle* Create(class CFsm* pFsm, class CMonster* pOwner, _uint iStateNum);
    virtual void Free() override;
};

END