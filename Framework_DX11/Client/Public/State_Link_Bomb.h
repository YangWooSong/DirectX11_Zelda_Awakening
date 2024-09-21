#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_Link_Bomb :
    public CState
{
public:
    CState_Link_Bomb(class CFsm* pFsm, class CPlayer* pPlayer);
    virtual ~CState_Link_Bomb() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum) override;
    virtual HRESULT Start_State() override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CPlayer* m_pPlayer = { nullptr };

    _bool               m_bPlay_Sound = { false };
    _uint               m_iCurrentAnimIndex = {};
    _float              m_fTimer = { 0.f };
public:
    static CState_Link_Bomb* Create(class CFsm* pFsm, class CPlayer* pPlayer, _uint iStateNum);
    virtual void Free() override;
};

END