#pragma once
#include "State.h"
#include "GameObject.h"

BEGIN(Engine)
class CFsm;
END


BEGIN(Client)

class CState_Pawn_Idle :
    public CState
{
public:
    CState_Pawn_Idle(class CFsm* pFsm, class  CMonster* pOwner);
    virtual ~CState_Pawn_Idle() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum) override;
    virtual HRESULT Start_State() override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pOwner = { nullptr };

    _uint               m_iCurrentAnimIndex = {};

    CGameObject*   m_pPlayer = { nullptr };
    _vector        m_vPlayerPos = {};

public:
    static CState_Pawn_Idle* Create(class CFsm* pFsm, class CMonster* pOwner, _uint iStateNum);
    virtual void Free() override;
};

END