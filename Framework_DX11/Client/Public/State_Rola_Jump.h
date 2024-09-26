#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END


BEGIN(Client)

class CState_Rola_Jump :
    public CState
{
public:
    CState_Rola_Jump(class CFsm* pFsm, class  CMonster* pOwner);
    virtual ~CState_Rola_Jump() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum) override;
    virtual HRESULT Start_State() override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pOwner = { nullptr };

    _uint               m_iCurrentAnimIndex = {};
    _uint               m_iJumpStAnimIndex = {};
    _uint               m_iJumpAnimIndex = {};
    _uint               m_iJumpEdAnimIndex = {};
    _uint*              m_pTargetIndex = {nullptr};

    _float              m_fLerpSpeed = { 0.f };
    _float              m_fTurnTimer = { 0.f };
   
    _int                m_iPreTargetPosIndex = { 0 };
    _int*               m_pAddDir = {nullptr};
    _bool               m_bJumpEnd = { false };
    _bool               m_bJumpSound = { false };

    _vector             m_vTargetLook = {};

    _float3     m_vTargetPos[5] = {
        _float3{49.51f, 0.05f, 24.39f},
        _float3{46.46f, 0.05f, 26.39f},
        _float3{44.93f, 0.05f, 24.02f},
        _float3{43.5f, 0.05f, 26.39f},
        _float3{40.6f, 0.05f, 23.72f} };   //왼쪽에서 오른쪽으로 이동

private:
    void Set_TargetPos();
    void Find_ClosestPos();

public:
    static CState_Rola_Jump* Create(class CFsm* pFsm, class CMonster* pOwner, _uint iStateNum);
    virtual void Free() override;
};

END