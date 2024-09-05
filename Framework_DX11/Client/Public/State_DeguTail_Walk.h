#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
class CNavigation;
END


BEGIN(Client)

class CState_DeguTail_Walk :
    public CState
{
public:
    CState_DeguTail_Walk(class CFsm* pFsm, class  CMonster* pOwner);
    virtual ~CState_DeguTail_Walk() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum) override;
    virtual HRESULT Start_State() override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pOwner = { nullptr };
    class CDeguTail_00* m_pOwnerDegu = { nullptr };
    CNavigation* m_pOwnerNavigation = { nullptr };

    _float      m_fTimer = { 0.f };

    _bool       m_bReflect = { true };
    _int           m_iStopCount = { 0 };
    _float            m_iReflectDir = { 1.f };
    _uint               m_iCurrentAnimIndex = {};

public:
    static CState_DeguTail_Walk* Create(class CFsm* pFsm, class CMonster* pOwner, _uint iStateNum);
    virtual void Free() override;
};

END

