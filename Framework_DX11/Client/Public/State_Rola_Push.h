#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END


BEGIN(Client)

class CState_Rola_Push :
    public CState
{
public:
    CState_Rola_Push(class CFsm* pFsm, class  CMonster* pOwner);
    virtual ~CState_Rola_Push() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum) override;
    virtual HRESULT Start_State() override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pOwner = { nullptr };
    class CRollingSpike* m_pRollingSpike = { nullptr };

    _uint               m_iCurrentAnimIndex = {};
    _int*               m_pAddDir = { nullptr };
    _uint*               m_pTargetIndex = { nullptr };
    _bool*               m_pARrived = { nullptr };

public:
    static CState_Rola_Push* Create(class CFsm* pFsm, class CMonster* pOwner, _uint iStateNum);
    virtual void Free() override;
};

END