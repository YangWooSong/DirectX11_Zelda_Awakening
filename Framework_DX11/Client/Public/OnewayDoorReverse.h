#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
class CSound;
END


BEGIN(Client)
class COnewayDoorReverse :
	public CGameObject
{
private:
	COnewayDoorReverse(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	COnewayDoorReverse(const COnewayDoorReverse& Prototype);
	virtual ~COnewayDoorReverse() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void OnCollisionEnter(CGameObject* pOther)  override;
	virtual void OnCollisionStay(CGameObject* pOther) override;
	virtual void OnCollisionExit(CGameObject* pOther)  override;

	void Set_Open(_bool bOpen) 
	{ 
		m_bOpend = bOpen; 
		m_fTimer = 0.f; 
		m_bSoundPlay = false;
	}
public:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CSound* m_pSoundCom = { nullptr };

private:
	HRESULT Ready_Components();

	_bool		m_bOpend = { false };
	_bool		m_bSoundPlay = { false };

	_int m_iCurrentAnimIndex = { 0 };
	_int m_iOpenAnimIndex = { 0 };
	_int m_iCloseAnimIndex = { 0 };
	_float m_fTimer = { 1.f };

public:
	static COnewayDoorReverse* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END