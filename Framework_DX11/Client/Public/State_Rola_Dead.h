#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END


BEGIN(Client)

class CState_Rola_Dead :
    public CState
{
public:
    CState_Rola_Dead(class CFsm* pFsm, class  CMonster* pOwner);
    virtual ~CState_Rola_Dead() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum) override;
    virtual HRESULT Start_State() override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pOwner = { nullptr };
    class CRollingSpike* m_pRollingSpike = { nullptr };

    _float m_fTimer = { 0.f };
    _bool   m_bPlaySound = { false };
    _uint               m_iCurrentAnimIndex = {};

public:
    static CState_Rola_Dead* Create(class CFsm* pFsm, class CMonster* pOwner, _uint iStateNum);
    virtual void Free() override;
};

END