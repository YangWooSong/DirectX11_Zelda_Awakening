#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
class CTransform;
END


BEGIN(Client)

class CState_DeguTail_Hurt :
    public CState
{
public:
    CState_DeguTail_Hurt(class CFsm* pFsm, class  CMonster* pOwner);
    virtual ~CState_DeguTail_Hurt() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum) override;
    virtual HRESULT Start_State() override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pOwner = { nullptr };
    class CDeguTail_00* m_pDeguOwner = { nullptr };

    _uint               m_iCurrentAnimIndex = {};
    _uint               m_iHitAnimIndex = {};
    _uint               m_iSpinAnimIndex = {};

    _float              m_fTimer = { 0.f };
    _float              m_fBlinkTimer = { 0.f };

    CTransform* m_pOwnerTransform = { nullptr };
public:
    static CState_DeguTail_Hurt* Create(class CFsm* pFsm, class CMonster* pOwner, _uint iStateNum);
    virtual void Free() override;
};

END

