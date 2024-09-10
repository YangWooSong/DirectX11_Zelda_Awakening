#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
class CNavigation;
END


BEGIN(Client)

class CState_Octorok_Walk :
        public CState
{
public:
    CState_Octorok_Walk(class CFsm* pFsm, class  CMonster* pOwner);
    virtual ~CState_Octorok_Walk() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum) override;
    virtual HRESULT Start_State() override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pOwner = { nullptr };
    class CDetector* m_pDetector = { nullptr };

    _uint               m_iCurrentAnimIndex = {};
    _uint               m_iOwnerDir = {};

    _bool               m_bGoStraight = { true };

    _float              m_fSpeed = { 0.f };
    _float              m_fTimer = { 0.f };
    _float3             m_fTargetRot = {};

    class CNavigation* m_pNavigation = { nullptr };
public:
    static CState_Octorok_Walk* Create(class CFsm* pFsm, class CMonster* pOwner, _uint iStateNum);
    virtual void Free() override;
};

END