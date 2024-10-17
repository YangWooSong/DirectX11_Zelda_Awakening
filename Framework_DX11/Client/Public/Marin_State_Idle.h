#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CMarin_State_Idle :
    public CState
{
public:
    CMarin_State_Idle(class CFsm* pFsm, class CNPC* pOwner);
    virtual ~CMarin_State_Idle() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum) override;
    virtual HRESULT Start_State() override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CNPC* m_pOwner = { nullptr };
    class CLink* m_pPlayer = { nullptr };

    _uint               m_iCurrentAnimIndex = {};

private:
    void LookAtPlayer(_float fTimerDelta);

public:
    static CMarin_State_Idle* Create(class CFsm* pFsm, class CNPC* pOwner, _uint iStateNum);
    virtual void Free() override;
};

END