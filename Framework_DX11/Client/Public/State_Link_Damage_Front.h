#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_Link_Damage_Front :
    public CState
{
public:
    CState_Link_Damage_Front(class CFsm* pFsm, class CPlayer* pPlayer);
    virtual ~CState_Link_Damage_Front() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum) override;
    virtual HRESULT Start_State() override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CPlayer* m_pPlayer = { nullptr };

    _uint               m_iCurrentAnimIndex = {};

public:
    static CState_Link_Damage_Front* Create(class CFsm* pFsm, class CPlayer* pPlayer, _uint iStateNum);
    virtual void Free() override;
};

END