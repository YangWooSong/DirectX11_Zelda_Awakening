#pragma once
#include "Client_Defines.h"
#include "2DEffects.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)
class CSpark_Effect :
	public C2DEffects
{
private:
	CSpark_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSpark_Effect(const CSpark_Effect& Prototype);
	virtual ~CSpark_Effect() = default;

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
	static CSpark_Effect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END;
