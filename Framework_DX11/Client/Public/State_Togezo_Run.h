#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END


BEGIN(Client)

class CState_Togezo_Run :
    public CState
{
public:
    CState_Togezo_Run(class CFsm* pFsm, class  CMonster* pOwner);
    virtual ~CState_Togezo_Run() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum) override;
    virtual HRESULT Start_State() override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pOwner = { nullptr };

    _uint               m_iCurrentAnimIndex = {};
    _uint               m_iRunStAnimIndex = {};
    _uint               m_iRunLpAnimIndex = {};
    _uint               m_iDir = {};

    _bool               m_bMove = { true };

public:
    static CState_Togezo_Run* Create(class CFsm* pFsm, class CMonster* pOwner, _uint iStateNum);
    virtual void Free() override;
};

END