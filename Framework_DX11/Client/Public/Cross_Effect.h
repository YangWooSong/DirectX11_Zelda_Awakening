#pragma once
#include "Client_Defines.h"
#include "2DEffects.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)
class CCross_Effect :
	public C2DEffects
{
private:
	CCross_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCross_Effect(const CCross_Effect& Prototype);
	virtual ~CCross_Effect() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	bool m_bAlphaDown = { false };

private:
	HRESULT Ready_Components();
	void Lerp_Size(_float fTimeDelta);


public:
	static CCross_Effect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END;
