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
    _uint*                m_pTargetIndex = {nullptr};
    _int*                m_pAddDir = {nullptr};

    _float3     m_vTargetPos[5] = {
        _float3{49.51f, 0.05f, 24.39f},
        _float3{47.46f, 0.05f, 26.39f},
        _float3{44.93f, 0.05f, 24.02f},
        _float3{42.38f, 0.05f, 26.39f},
        _float3{40.55f, 0.05f, 23.72f} };   //왼쪽에서 오른쪽으로 이동

private:
    void Set_TargetPos();

public:
    static CState_Rola_Jump* Create(class CFsm* pFsm, class CMonster* pOwner, _uint iStateNum);
    virtual void Free() override;
};

END