#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END


BEGIN(Client)

class CState_Pawn_Walk :
    public CState
{
public:
    CState_Pawn_Walk(class CFsm* pFsm, class  CMonster* pOwner);
    virtual ~CState_Pawn_Walk() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum) override;
    virtual HRESULT Start_State() override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    void Jump();
    void Check_Jump(_float _fTimeDelta);

private:
    class CMonster* m_pOwner = { nullptr };

    _uint               m_iCurrentAnimIndex = {};

    _float3             m_vPlayerPos = {};

    class CPlayer*   m_pPlayer = { nullptr };

public:
    static CState_Pawn_Walk* Create(class CFsm* pFsm, class CMonster* pOwner, _uint iStateNum);
    virtual void Free() override;
};

END