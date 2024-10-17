#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
class CCollider;
class CFsm;
class CSound;
END

BEGIN(Client)
class CHousePot :
    public CGameObject
{
private:
	CHousePot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CHousePot(const CHousePot& Prototype);
	virtual ~CHousePot() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_LightDepth() override;

public:
	virtual void OnCollisionEnter(CGameObject* pOther) override;
	virtual void OnCollisionStay(CGameObject* pOther) override;
	virtual void OnCollisionExit(CGameObject* pOther) override;

public:
	void Set_Break(_bool bBreak);

public:
	 CShader* m_pShaderCom = { nullptr };
	 CModel* m_pModelCom = { nullptr };
	 CCollider* m_pColliderCom = { nullptr };
	 CSound* m_pSoundCom = { nullptr };

	 CModel* m_pPlayerModelCom = { nullptr };
	 CFsm* m_pPlayerFsmCom = { nullptr };
private:
	HRESULT Ready_Components();

private:
	_bool m_bCarried = { false };
	_bool m_bBreak = { false };
	_float m_fTimer = { 0.f };

	const _float4x4* m_pSocketMatrix = { nullptr };

	CGameObject* m_pPlayer = { nullptr };
	CGameObject* m_pParticle = { nullptr };

public:
	static CHousePot* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};
END
