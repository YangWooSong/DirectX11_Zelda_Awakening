#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_Link_Carry :
    public CState
{
public:
    CState_Link_Carry(class CFsm* pFsm, class CPlayer* pPlayer);
    virtual ~CState_Link_Carry() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum) override;
    virtual HRESULT Start_State() override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CPlayer* m_pPlayer = { nullptr };

    _bool               m_bPlay_Sound = { false };
    _uint               m_iCurrentAnimIndex = {};
    _uint               m_iCarryIdleAnimIndex = {};
    _uint               m_iCarryStartAnimIndex = {};
    _uint               m_iCarryWalkAnimIndex = {};
    _float              m_fTimer = { 0.f };
public:
    static CState_Link_Carry* Create(class CFsm* pFsm, class CPlayer* pPlayer, _uint iStateNum);
    virtual void Free() override;
};

END