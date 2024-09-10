#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Instancing :
    public CVIBuffer
{
public:
    typedef struct
    {
        _uint iNumInstance; //그릴 객체 수
        _float3     vCenter;    //객체들을 그리는 큐브 범위의 좌표
        _float3		vPivot;     //퍼질 때 기준이 되는 좌표
        _float3     vRange;     //객체들을 그리는 큐브 사이즈
        _float2     vSize;      //객체 최소, 최대 사이즈
        _float2		vSpeed;     //이동 속도(최소, 최대)
        _float2		vLifeTime;
        _bool		isLoop;

    }INSTANCE_DESC;

protected:
	CVIBuffer_Instancing(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Instancing(const CVIBuffer_Instancing& Prototype);
	virtual ~CVIBuffer_Instancing() = default;

public:
	virtual HRESULT Initialize_Prototype(const INSTANCE_DESC& Desc);
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Bind_Buffers() override;
	virtual HRESULT Render() override;

public:
	virtual void Spread(_float fTimeDelta) = 0;
	virtual void Drop(_float fTimeDelta) = 0;

protected:
	ID3D11Buffer* m_pVBInstance = { nullptr };
	D3D11_BUFFER_DESC			m_InstanceBufferDesc = {};
	D3D11_SUBRESOURCE_DATA		m_InstanceInitialData = {};

	_uint						m_iInstanceStride = { 0 };
	_uint						m_iIndexCountPerInstance = { 0 };
	_uint						m_iNumInstance = { 0 };
	_float3						m_vCenterPos = {};
	_float3						m_vPivotPos = {};
	_float3						m_vRange = {};
	_float2						m_vSize = {};
	_float2						m_vLifeTime = {};
	_bool						m_isLoop = {};
	_float* m_pSpeed = { nullptr };


	void* m_pInstanceVertices = { nullptr };


public:
	virtual CComponent* Clone(void* pArg) = 0;
	virtual void Free() override;
};
END