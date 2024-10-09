#pragma once
#include "Client_Defines.h"
#include "3D_Effects.h"

BEGIN(Engine)
class CShader;
class CModel;
class CTexture;
END
BEGIN(Client)
class CHalo_Effect :
    public C3D_Effects
{
private:
	CHalo_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CHalo_Effect(const CHalo_Effect& Prototype);
	virtual ~CHalo_Effect() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CTexture* m_pHaloTexture = { nullptr };
	CTexture* m_pRainbowTexture = { nullptr };

private:
	_float m_fTexMove = { 0.f };
private:
	HRESULT Ready_Components();
public:
	static C3D_Effects* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END