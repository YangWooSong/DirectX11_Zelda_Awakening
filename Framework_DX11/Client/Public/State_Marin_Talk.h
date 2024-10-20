#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
class CGameObject;
END

BEGIN(Client)

class CState_Marin_Talk :
    public CState
{
public:
    CState_Marin_Talk(class CFsm* pFsm, class CNPC* pOwner);
    virtual ~CState_Marin_Talk() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum) override;
    virtual HRESULT Start_State() override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CNPC* m_pOwner = { nullptr };
    class CLink* m_pPlayer = { nullptr };
    class CDialogueUI* m_pDialogueUI = { nullptr };

    _uint               m_iCurrentAnimIndex = {};
    _uint               m_iLineNum = {0};
    
public:
    static CState_Marin_Talk* Create(class CFsm* pFsm, class CNPC* pOwner, _uint iStateNum);
    virtual void Free() override;
};

END