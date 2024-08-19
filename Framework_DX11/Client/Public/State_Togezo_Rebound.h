#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END


BEGIN(Client)

class CState_Togezo_Rebound :
    public CState
{
public:
    CState_Togezo_Rebound(class CFsm* pFsm, class  CMonster* pOwner);
    virtual ~CState_Togezo_Rebound() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum) override;
    virtual HRESULT Start_State() override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pOwner = { nullptr };

    _uint               m_iCurrentAnimIndex = {};

    _uint               m_iReoundStIndex = { 0 };
    _uint               m_iReoundEdIndex = { 0 };
    _uint               m_iReoundLpIndex = { 0 };

public:
    static CState_Togezo_Rebound* Create(class CFsm* pFsm, class CMonster* pOwner, _uint iStateNum);
    virtual void Free() override;
};

END