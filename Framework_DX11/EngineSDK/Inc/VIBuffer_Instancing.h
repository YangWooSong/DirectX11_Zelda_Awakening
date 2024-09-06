#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Instancing :
    public CVIBuffer
{
public:
    typedef struct
    {
        _uint iNumInstance; //�׸� ��ü ��
        _float3 vCenter;    //��ü���� �׸��� ť�� ������ ��ǥ
        _float3 vRange;     //��ü���� �׸��� ť�� ������
        _float2 vSize;      //��ü �ּ�, �ִ� ������
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

protected:
    ID3D11Buffer*               m_pVBInstance = { nullptr };
    D3D11_BUFFER_DESC			m_InstanceBufferDesc = {};
    D3D11_SUBRESOURCE_DATA		m_InstanceInitialData = {};

    _uint						m_iInstanceStride = { 0 };
    _uint						m_iIndexCountPerInstance = { 0 };
    _uint						m_iNumInstance = { 0 };
    _float3						m_vCenterPos = {};
    _float3						m_vRange = {};
    _float2						m_vSize = {};

    void* m_pInstanceVertices = { nullptr };

public:
    virtual CComponent* Clone(void* pArg) = 0;
    virtual void Free() override;
};

END