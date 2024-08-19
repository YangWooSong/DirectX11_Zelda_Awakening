#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END


BEGIN(Client)

class CState_Pawn_Jump :
    public CState
{
public:
    CState_Pawn_Jump(class CFsm* pFsm, class  CMonster* pOwner);
    virtual ~CState_Pawn_Jump() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum) override;
    virtual HRESULT Start_State() override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pOwner = { nullptr };

    _uint               m_iCurrentAnimIndex = {};

    _float              m_fJumpMax = { 1.5f };
    _float              m_fJumpAmount = { 0.1f };
    _float              m_fJumpStartY = { 0.f };
    _float              m_fMoveSpeed = { 0.f };

    _bool               m_bJump = { false };
    _bool               m_bStart = { true };

private:
    void Jump();

public:
    static CState_Pawn_Jump* Create(class CFsm* pFsm, class CMonster* pOwner, _uint iStateNum);
    virtual void Free() override;
};

END