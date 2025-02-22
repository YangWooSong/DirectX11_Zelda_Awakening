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
class CPurpleQuartz :
	public CGameObject
{
private:
	CPurpleQuartz(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPurpleQuartz(const CPurpleQuartz& Prototype);
	virtual ~CPurpleQuartz() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_LightDepth() override;
public:
	virtual void OnCollisionEnter(CGameObject* pOther)  override;
	virtual void OnCollisionStay(CGameObject* pOther) override;
	virtual void OnCollisionExit(CGameObject* pOther)  override;

public:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	class CCollider* m_pColliderCom = { nullptr };
	CSound* m_pSoundCom = { nullptr };

	_bool Get_isBreaked() { return m_bBreak; }
private:
	HRESULT Ready_Components();

private:
	CGameObject* m_pParticle = { nullptr };

	_bool m_bBreak = { false };

public:
	static CPurpleQuartz* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END