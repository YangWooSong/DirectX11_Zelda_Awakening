#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
class CCollider;
class CSound;
END


BEGIN(Client)
class CTreasureBox :
	public CGameObject
{
private:
	CTreasureBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTreasureBox(const CTreasureBox& Prototype);
	virtual ~CTreasureBox() = default;

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

public:
	_bool Get_IsOpened() { return m_bOpened; }
	_bool Get_bShow() { return m_bShow; }
	void Set_bShow(_bool bShow) { m_bShow = bShow; }
public:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CCollider* m_pColliderCom = { nullptr };
	CSound* m_pSoundCom = { nullptr };

private:
	HRESULT Ready_Components();
	void Change_PlayerUI_TextureNum();
	void Set_Item();
	void Play_Alarm();

private:
	_int		m_iCurrentAnimIndex = { 0 };
	_bool		m_bOpened = { false };
	_bool		m_bCangePlayerState = { false };
	_bool		m_bPlayAlarm = { false };
	_bool		m_bShow = { false };
	_bool		m_bShowAction = { false };
	_bool		m_bAdd = { false };
	_bool		m_bCameraAction = { false };
	_bool		m_bGimmickSound = { false };
	_bool		m_bBGMSound = { false };

	_float		m_fOpenTimer = { 0.f };
	_float		m_fBrightness = { 15.f };
	_float		m_fCameraTimer = { 0.f };

	_uint		m_iItemIndex = { 0 };

private:
	void Set_Brightness(_float fTimeDelta);
public:
	static CTreasureBox* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END