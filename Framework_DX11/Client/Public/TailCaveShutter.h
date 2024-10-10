#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
class CSound;
END

BEGIN(Client)

class CTailCaveShutter :
	public CGameObject
{
private:
	CTailCaveShutter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTailCaveShutter(const CTailCaveShutter& Prototype);
	virtual ~CTailCaveShutter() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	void Set_Open(_bool bOpen) { m_bOpen = bOpen; }
	_bool Get_Open() { return m_bOpen; }
public:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CSound* m_pSoundCom = { nullptr };
private:
	_int m_iCurrentAnimIndex = { 0 };
	_int m_iOpenAnimIndex = { 0 };

	_float m_fOpenedTimer = {0.f};
	_bool m_bOpen = { false };
	_bool m_bChangeAnim = { false };
	_bool m_bCameraSetting = { false };
	_bool m_bCameraBackOrigin = { false };
	_bool m_bPlaySound = { false };
	_bool m_bActiveEffect = { false };
	
	CGameObject* m_pEffect = { nullptr };
private:
	HRESULT Ready_Components();
	void Camera_Set();
	void Camera_Set_to_Origin();
public:
	static CTailCaveShutter* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END