#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_Link_Shield_Walk :
    public CState
{
public:
    CState_Link_Shield_Walk(class CFsm* pFsm, class CPlayer* pPlayer);
    virtual ~CState_Link_Shield_Walk() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum) override;
    virtual HRESULT Start_State() override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CPlayer* m_pPlayer = { nullptr };

    _uint               m_iCurrentAnimIndex = {};
    _uint               m_iRightAnimIndex = {};
    _uint               m_iLeftAnimIndex = {};
    _uint               m_iFrontAnimIndex = {};
    _uint               m_iBackAnimIndex = {};

    _float              m_fPlayerSpeed = { 0.f };

public:
    static CState_Link_Shield_Walk* Create(class CFsm* pFsm, class CPlayer* pPlayer, _uint iStateNum);
    virtual void Free() override;
};

END