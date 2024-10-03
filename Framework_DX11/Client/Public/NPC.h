#pragma once
#include "Client_Defines.h"
#include "ContainerObject.h"


BEGIN(Engine)
class CShader;
class CModel;
class CFsm;
class CCollider;
class CSound;
class CUIObject;
END

BEGIN(Client)
class CNPC :
    public CContainerObject
{
public:
	enum PARTID { PART_END };
	enum UI_ID{DIALOGUE_UI, UI_END};
protected:
	CNPC(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNPC(const CNPC& Prototype);
	virtual ~CNPC() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_LightDepth() override;

public:
	virtual void OnCollisionEnter(CGameObject* pOther) {};
	virtual void OnCollisionStay(CGameObject* pOther) {};
	virtual void OnCollisionExit(CGameObject* pOther) {};

public:
	CModel* Get_Model()	{return m_pModelCom;	}
	CSound* Get_Sound()	{return m_pSoundCom;	}
	CFsm* Get_Fsm() { return m_pFsmCom; }
	CUIObject* Get_UIObject(_uint iIndex) { return m_NPCUI[iIndex]; }
	
	void		Change_State(const _uint iState);
	void		SetUp_NextAnimation(_uint iNextAnimationIndex, _float fChangeDuration = 0.2f, _bool _bLoop = false);
	_bool       Get_IsEnd_CurrentAnimation();

	void		Set_Talk(_bool bTalk) { m_bTalk = bTalk; }
	_bool		Get_Talk() { return m_bTalk; }


protected:
	CModel*		m_pModelCom = { nullptr };
	CShader*	m_pShaderCom = { nullptr };
	CFsm*		m_pFsmCom = { nullptr };
	CCollider*  m_pColliderCom = { nullptr };
	CSound*		m_pSoundCom = { nullptr };

	_bool       m_bTalk = { false };


	vector<CUIObject*> m_NPCUI;

protected:
	HRESULT     Ready_Components();
	HRESULT		Ready_PartObjects();

public:
	static CNPC* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END