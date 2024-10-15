#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_Link_Get_Item :
    public CState
{
public:
    CState_Link_Get_Item(class CFsm* pFsm, class CPlayer* pPlayer);
    virtual ~CState_Link_Get_Item() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum) override;
    virtual HRESULT Start_State() override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CPlayer* m_pPlayer = { nullptr };
    class CPlayerCamera* m_pCamera = { nullptr };
    class CDialogueUI* m_pDialogueUI = { nullptr };

    _uint               m_iCurrentAnimIndex = {};
    _uint               m_iAnimStIndex = { 0 };
    _uint               m_iAnimLpIndex = { 0 };
    _uint               m_iAnimEdIndex = { 0 };

    _float              m_fLoopTimer = { 0.f };

    _bool m_bSoundPlay = { false };

public:
    static CState_Link_Get_Item* Create(class CFsm* pFsm, class CPlayer* pPlayer, _uint iStateNum);
    virtual void Free() override;
};

END