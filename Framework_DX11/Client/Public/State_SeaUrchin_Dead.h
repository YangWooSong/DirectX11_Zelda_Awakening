#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END


BEGIN(Client)

class CState_SeaUrchin_Dead :
    public CState
{
public:
    CState_SeaUrchin_Dead(class CFsm* pFsm, class  CMonster* pOwner);
    virtual ~CState_SeaUrchin_Dead() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum) override;
    virtual HRESULT Start_State() override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pOwner = { nullptr };
    _vector  m_vNewLook = {};
    _uint               m_iCurrentAnimIndex = {};
    _float  m_fTimer = { 0.f };
public:
    static CState_SeaUrchin_Dead* Create(class CFsm* pFsm, class CMonster* pOwner, _uint iStateNum);
    virtual void Free() override;
};

END