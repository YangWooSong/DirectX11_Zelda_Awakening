#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END


BEGIN(Client)

class CState_Link_Slash_Hold :
    public CState
{
public:
    CState_Link_Slash_Hold(class CFsm* pFsm, class CPlayer* pPlayer);
    virtual ~CState_Link_Slash_Hold() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum) override;
    virtual HRESULT Start_State() override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CPlayer* m_pPlayer = { nullptr };

    _uint               m_iCurrentAnimIndex = {};
    _uint               m_iSlash_Hold_b_AnimIndex = {};
    _uint               m_iSlash_Hold_f_AnimIndex = {};
    _uint               m_iSlash_Hold_l_AnimIndex = {};
    _uint               m_iSlash_Hold_r_AnimIndex = {};
    _uint               m_iSlash_Hold_lp_AnimIndex = {};
    _uint               m_iSlash_Hold_ed_AnimIndex = {};

    _float              m_fPlayerSpeed = { 0.f };
    _uint               m_iPlayerDir = {};
public:
    static CState_Link_Slash_Hold* Create(class CFsm* pFsm, class CPlayer* pPlayer, _uint iStateNum);
    virtual void Free() override;
};

END