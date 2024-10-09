#pragma once
#include "Client_Defines.h"
#include "2DEffects.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)
class CBombExplosion :
	public C2DEffects
{
private:
	CBombExplosion(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBombExplosion(const CBombExplosion& Prototype);
	virtual ~CBombExplosion() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CGameObject* m_pParticle = { nullptr };
	vector<CGameObject*>	m_Child_List;

	_bool m_bActiveChild = { false };
private:
	HRESULT Ready_Child();
public:
	static CBombExplosion* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END;
