#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END


BEGIN(Client)

class CState_Octorok_Idle :
    public CState
{
public:
    CState_Octorok_Idle(class CFsm* pFsm, class  CMonster* pOwner);
    virtual ~CState_Octorok_Idle() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum) override;
    virtual HRESULT Start_State() override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pOwner = { nullptr };

    _uint               m_iCurrentAnimIndex = {};
    _float              m_iTimer = { 0.f };
    _uint               m_iOrigin_Dir = { 0};
public:
    static CState_Octorok_Idle* Create(class CFsm* pFsm, class CMonster* pOwner, _uint iStateNum);
    virtual void Free() override;
};

END

