#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
class CNavigation;
END

BEGIN(Client)

class CState_Link_Jump :
    public CState
{
public:
    CState_Link_Jump(class CFsm* pFsm, class CPlayer* pPlayer);
    virtual ~CState_Link_Jump() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum) override;
    virtual HRESULT Start_State() override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CPlayer* m_pPlayer = { nullptr };

    _uint               m_iCurrentAnimIndex = {};
    _uint               m_iJumpAnimIndex = {};
    _uint               m_iLandAnimIndex= {};

    _float              m_fJumpMax = { 4.f };
    _float              m_fJumpAmount = { 0.1f};
    _float              m_fJumpStartY = { 0.f};
    _float              m_fMoveSpeed = { 0.f};

    _bool               m_bJump = { false };
    _bool               m_bStart= { true };

    class CItem* m_pPlayer_ItemA = { nullptr };
    class CItem* m_pPlayer_ItemB = { nullptr };

    class CNavigation* m_pPlayer_Navigation = { nullptr };

private:
    void Jump();
public:
    static CState_Link_Jump* Create(class CFsm* pFsm, class CPlayer* pPlayer, _uint iStateNum);
    virtual void Free() override;
};
END