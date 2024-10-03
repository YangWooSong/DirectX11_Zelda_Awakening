#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_ToolShopkeeper_Attack :
    public CState
{
public:
    CState_ToolShopkeeper_Attack(class CFsm* pFsm, class CNPC* pOwner);
    virtual ~CState_ToolShopkeeper_Attack() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum) override;
    virtual HRESULT Start_State() override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CNPC* m_pOwner = { nullptr };

    _uint               m_iCurrentAnimIndex = {};
    _uint               m_iBeamStAnimIndex = {};
    _uint               m_iBeamLpAnimIndex = {};

public:
    static CState_ToolShopkeeper_Attack* Create(class CFsm* pFsm, class CNPC* pOwner, _uint iStateNum);
    virtual void Free() override;
};

END