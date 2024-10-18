#pragma once
#include "Client_Defines.h"
#include "2DEffects.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)
class CGrad :
	public C2DEffects
{
private:
	CGrad(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGrad(const CGrad& Prototype);
	virtual ~CGrad() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_float m_fTurnAnlge = { 0.f };
private:
	HRESULT Ready_Components();
	void Lerp_Size(_float fTimeDelta);	//커졌다 작아짐(한번)

public:
	static CGrad* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END;
