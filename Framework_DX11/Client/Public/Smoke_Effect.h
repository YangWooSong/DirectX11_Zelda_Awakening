#pragma once
#include "Client_Defines.h"
#include "2DEffects.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)
class CSmoke_Effect :
	public C2DEffects
{
private:
	CSmoke_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSmoke_Effect(const CSmoke_Effect& Prototype);
	virtual ~CSmoke_Effect() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CTexture* m_pDissolveTextureCom = { nullptr };

private:
	_float m_fDissovle = { 1.f };
	_bool m_bDissolve = { false };
	_bool m_bReset = { false };
	_float4 m_fOutLineColor = {};
private:
	HRESULT Ready_Components();

public:
	static CSmoke_Effect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END;
