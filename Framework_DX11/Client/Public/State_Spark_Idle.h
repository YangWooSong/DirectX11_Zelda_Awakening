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

    _float3 m_vTargetPos[4] = { _float3(-5.f,0.f,5.f), _float3(-5.f,5.f,5.f), _float3(-10.f,5.f,5.f), _float3(-10.f, 0.f,5.f) };
    _uint   m_iTargetIndex = { 0 };
public:
    static CState_Spark_Idle* Create(class CFsm* pFsm, class CMonster* pOwner, _uint iStateNum);
    virtual void Free() override;
};

END