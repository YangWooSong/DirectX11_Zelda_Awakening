#pragma once
#include "Client_Defines.h"
#include "2DEffects.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)
class CLockBlockEffect :
	public C2DEffects
{
private:
	CLockBlockEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLockBlockEffect(const CLockBlockEffect& Prototype);
	virtual ~CLockBlockEffect() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
private:
	_bool m_bActiveChild = { false };
	_float m_fTimer = { 0.f };
private:
	HRESULT Ready_Child();
public:
	static CLockBlockEffect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END;
