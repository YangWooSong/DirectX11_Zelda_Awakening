#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CVIBuffer_Model_Instance;
class CModel;
END

BEGIN(Client)

class CParticle_Model final : public CGameObject
{
public:
	enum PARTICLE_TYPE{PURPLEQUARTZ, HOUSEPOT, ROCK, TYPE_END};

	typedef struct : public CGameObject::GAMEOBJECT_DESC
	{
		_uint iParticleType = 0;
	} MODEL_PARTICLE_DESC;
private:
	CParticle_Model(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CParticle_Model(const CParticle_Model& Prototype);
	virtual ~CParticle_Model() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_Model_Instance* m_pVIBufferCom = { nullptr };
	CModel* m_pModelCom = { nullptr };

private:
	_uint m_iParticleType = { TYPE_END };

private:
	HRESULT Ready_Components();

public:
	static CParticle_Model* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END