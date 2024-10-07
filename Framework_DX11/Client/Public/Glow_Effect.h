
#pragma once
#include "Client_Defines.h"
#include "2DEffects.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)
class CGlow_Effect :
	public C2DEffects
{
private:
	CGlow_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGlow_Effect(const CGlow_Effect& Prototype);
	virtual ~CGlow_Effect() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	bool m_bSizeDown = { false };

private:
	HRESULT Ready_Components();
	void Lerp_Size(_float fTimeDelta);


public:
	static CGlow_Effect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END;
