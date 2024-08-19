#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END


BEGIN(Client)

class CState_Rola_Jump :
    public CState
{
public:
    CState_Rola_Jump(class CFsm* pFsm, class  CMonster* pOwner);
    virtual ~CState_Rola_Jump() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum) override;
    virtual HRESULT Start_State() override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pOwner = { nullptr };

    _uint               m_iCurrentAnimIndex = {};
    _uint               m_iJumpStAnimIndex = {};
    _uint               m_iJumpAnimIndex = {};
    _uint               m_iJumpEdAnimIndex = {};

public:
    static CState_Rola_Jump* Create(class CFsm* pFsm, class CMonster* pOwner, _uint iStateNum);
    virtual void Free() override;
};

END