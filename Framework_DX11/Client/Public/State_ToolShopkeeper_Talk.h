#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
class CGameObject;
END

BEGIN(Client)

class CState_ToolShopkeeper_Talk :
    public CState
{
public:
    CState_ToolShopkeeper_Talk(class CFsm* pFsm, class CNPC* pOwner);
    virtual ~CState_ToolShopkeeper_Talk() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum) override;
    virtual HRESULT Start_State() override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CNPC* m_pOwner = { nullptr };
    class CLink* m_pPlayer = { nullptr };
    class CDialogueUI* m_pDialogueUI = { nullptr };

    CGameObject* m_pCarryObj = { nullptr };

    _uint               m_iCurrentAnimIndex = {};
    _float m_fTimer = { 0.f };

    _int m_iSelectChoiceBtn = { 0 };

private:
    void Set_UI_LineNum();
    void Set_SelectedChoiceBtn();
    void Buy_Item();
 
public:
    static CState_ToolShopkeeper_Talk* Create(class CFsm* pFsm, class CNPC* pOwner, _uint iStateNum);
    virtual void Free() override;
};

END