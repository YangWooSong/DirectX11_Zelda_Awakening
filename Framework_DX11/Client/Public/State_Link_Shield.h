#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_Link_Shield :
    public CState
{
public:
    CState_Link_Shield(class CFsm* pFsm, class CPlayer* pPlayer);
    virtual ~CState_Link_Shield() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum) override;
    virtual HRESULT Start_State() override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CPlayer* m_pPlayer = { nullptr };

    _uint               m_iCurrentAnimIndex = {};
    _uint               m_iStartAnimIndex = {};
    _uint               m_iLoopAnimIndex = {};
    _uint               m_iEndAnimIndex = {};

public:
    static CState_Link_Shield* Create(class CFsm* pFsm, class CPlayer* pPlayer, _uint iStateNum);
    virtual void Free() override;
};

END