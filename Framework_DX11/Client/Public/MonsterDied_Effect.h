#pragma once
#include "Client_Defines.h"
#include "2DEffects.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)
class CMonsterDied_Effect :
	public C2DEffects
{
private:
	CMonsterDied_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonsterDied_Effect(const CMonsterDied_Effect& Prototype);
	virtual ~CMonsterDied_Effect() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CGameObject* m_pParticle = { nullptr };
	_float m_fActiveTimer = { 0.f };
private:
	HRESULT Ready_Child();
public:
	static CMonsterDied_Effect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END;
