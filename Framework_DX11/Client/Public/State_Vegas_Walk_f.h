#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END


BEGIN(Client)

class CState_Vegas_Walk_f :
    public CState
{
public:
    CState_Vegas_Walk_f(class CFsm* pFsm, class  CMonster* pOwner);
    virtual ~CState_Vegas_Walk_f() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum) override;
    virtual HRESULT Start_State() override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pOwner = { nullptr };

    _uint               m_iCurrentAnimIndex = {};

    _float  m_fSpeed = { 1.f };
    _float  m_fTimer = { 0.3f };
public:
    static CState_Vegas_Walk_f* Create(class CFsm* pFsm, class CMonster* pOwner, _uint iStateNum);
    virtual void Free() override;
};

END