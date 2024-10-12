#pragma once
#include "Client_Defines.h"
#include "2DEffects.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)
class CFire_Big_Effect :
	public C2DEffects
{
private:
	CFire_Big_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFire_Big_Effect(const CFire_Big_Effect& Prototype);
	virtual ~CFire_Big_Effect() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
private:
	_bool m_bActiveChild = { false };

private:
	HRESULT Ready_Child();
public:
	static CFire_Big_Effect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END;
