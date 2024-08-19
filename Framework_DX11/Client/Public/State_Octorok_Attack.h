#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END


BEGIN(Client)

class CState_Octorok_Attack :
    public CState
{
public:
    CState_Octorok_Attack(class CFsm* pFsm, class  CMonster* pOwner);
    virtual ~CState_Octorok_Attack() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum) override;
    virtual HRESULT Start_State() override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    void    Shoot();
private:
    class CMonster* m_pOwner = { nullptr };

    _uint               m_iCurrentAnimIndex = {};
    _uint               m_iAttackStartIndex = { 0 };
    _uint               m_iAttackFinIndex = { 0 };
    _uint               m_iShootCount = { 0 };
public:
    static CState_Octorok_Attack* Create(class CFsm* pFsm, class CMonster* pOwner, _uint iStateNum);
    virtual void Free() override;
};

END