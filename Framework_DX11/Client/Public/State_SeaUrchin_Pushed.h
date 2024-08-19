#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END


BEGIN(Client)

class CState_SeaUrchin_Pushed :
    public CState
{
public:
    CState_SeaUrchin_Pushed(class CFsm* pFsm, class  CMonster* pOwner);
    virtual ~CState_SeaUrchin_Pushed() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum) override;
    virtual HRESULT Start_State() override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pOwner = { nullptr };

    _uint               m_iCurrentAnimIndex = {};

public:
    static CState_SeaUrchin_Pushed* Create(class CFsm* pFsm, class CMonster* pOwner, _uint iStateNum);
    virtual void Free() override;
};

END