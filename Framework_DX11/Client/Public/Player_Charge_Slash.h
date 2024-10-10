#pragma once
#include "Client_Defines.h"
#include "2DEffects.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)
class CPlayer_Charge_Slash :
	public C2DEffects
{
private:
	CPlayer_Charge_Slash(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer_Charge_Slash(const CPlayer_Charge_Slash& Prototype);
	virtual ~CPlayer_Charge_Slash() = default;

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
	static CPlayer_Charge_Slash* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END;
