#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CVIBuffer_Point_Instance;
class CTexture;
END

BEGIN(Client)

class CParticle_Image final : public CGameObject
{
public:
	enum PARTICLE_TYPE { CROSS_MINI, BOMB_SMOKE, PLAYER_ITEM_GET, TYPE_END };

	typedef struct : public CGameObject::GAMEOBJECT_DESC
	{
		const _float4x4* pParentMatrix = { nullptr };;
		_uint iParticleType = 0;
	} IMAGE_PARTICLE_DESC;
private:
	CParticle_Image(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CParticle_Image(const CParticle_Image& Prototype);
	virtual ~CParticle_Image() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_Point_Instance* m_pVIBufferCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };

private:
	_uint m_iParticleType = { TYPE_END };
	const _float4x4* m_pParentMatrix = { nullptr };
private:
	HRESULT Ready_Components();

public:
	static CParticle_Image* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END