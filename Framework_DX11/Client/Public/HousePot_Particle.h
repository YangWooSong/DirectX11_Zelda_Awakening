#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CVIBuffer_Point_Instance;
class CModel;
END

BEGIN(Client)

class CHousePot_Particle final : public CGameObject
{
private:
	CHousePot_Particle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CHousePot_Particle(const CHousePot_Particle& Prototype);
	virtual ~CHousePot_Particle() = default;

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
	CModel* m_pModelCom = { nullptr };

private:
	HRESULT Ready_Components();

public:
	static CHousePot_Particle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END