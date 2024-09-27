#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END


BEGIN(Client)

class CState_DeguTail_Appear :
    public CState
{
public:
    CState_DeguTail_Appear(class CFsm* pFsm, class  CMonster* pOwner);
    virtual ~CState_DeguTail_Appear() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum) override;
    virtual HRESULT Start_State() override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pOwner = { nullptr };
    class CPlayerCamera* m_pCamera = { nullptr };

    _uint               m_iCurrentAnimIndex = {};
    _bool               m_bPlaySound = { false };
    _bool               m_bCameraShake = { false };
    _bool               m_bCarmeraSetting = { false };
public:
    static CState_DeguTail_Appear* Create(class CFsm* pFsm, class CMonster* pOwner, _uint iStateNum);
    virtual void Free() override;
};

END

