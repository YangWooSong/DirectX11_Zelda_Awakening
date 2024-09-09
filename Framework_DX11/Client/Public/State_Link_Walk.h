#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
class CNavigation;
END

BEGIN(Client)

class CState_Link_Walk :
    public CState
{
public:
    CState_Link_Walk(class CFsm* pFsm, class CPlayer* pPlayer);
    virtual ~CState_Link_Walk() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum) override;
    virtual HRESULT Start_State() override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CPlayer* m_pPlayer = { nullptr };

    _uint               m_iCurrentAnimIndex = {};
    _float              m_fPlayerSpeed = { 0.f };
    _float              m_fPressTime = { 0.f };

    class CItem* m_pPlayer_ItemA = { nullptr };
    class CItem* m_pPlayer_ItemB = { nullptr };

    _uint   m_iPlayerDir = { 0 };

    class CNavigation* m_pPlayer_Navigation = { nullptr };

public:
    static CState_Link_Walk* Create(class CFsm* pFsm, class CPlayer* pPlayer, _uint iStateNum);
    virtual void Free() override;
};

END