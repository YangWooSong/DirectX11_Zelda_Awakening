#pragma once

#include "VIBuffer_Instancing.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Model_Instance final : public CVIBuffer_Instancing
{
public:
	typedef struct : public CVIBuffer_Instancing::INSTANCE_DESC
	{
		_uint iNumVertices;
		_uint iNumIndices;
		ID3D11Buffer* pVB;
		ID3D11Buffer* pIB;

	}MODEL_INSTANCE_DESC;

private:
	CVIBuffer_Model_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Model_Instance(const CVIBuffer_Model_Instance& Prototype);
	virtual ~CVIBuffer_Model_Instance() = default;

public:
	virtual HRESULT Initialize_Prototype(void* pArg);
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual void Spread(_float fTimeDelta) override;
	virtual void Drop(_float fTimeDelta) override;
	virtual void Stay(_float fTimeDelta) override;
	void PurpleQuartz_Spread(_float fTimeDelta);
	void Grass_Spread(_float fTimeDelta, _float fFallTime, _float fFallSpeed);
	void Lawn_Spread(_float fTimeDelta, _float fFallTime, _float fFallSpeed);

private:
	_float fTimer = { 0.f };

public:
	static CVIBuffer_Model_Instance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg = nullptr);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};


END