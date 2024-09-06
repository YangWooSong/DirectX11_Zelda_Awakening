#include "VIBuffer_Instancing.h"

CVIBuffer_Instancing::CVIBuffer_Instancing(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CVIBuffer{ pDevice, pContext }
{
}

CVIBuffer_Instancing::CVIBuffer_Instancing(const CVIBuffer_Instancing& Prototype)
	: CVIBuffer{ Prototype }
	, m_InstanceBufferDesc{ Prototype.m_InstanceBufferDesc }
	, m_InstanceInitialData{ Prototype.m_InstanceInitialData }
	, m_iNumInstance{ Prototype.m_iNumInstance }
	, m_iIndexCountPerInstance{ Prototype.m_iIndexCountPerInstance }
	, m_iInstanceStride{ Prototype.m_iInstanceStride }
	, m_vCenterPos{ Prototype.m_vCenterPos }
	, m_vRange{ Prototype.m_vRange }
	, m_pInstanceVertices{ Prototype.m_pInstanceVertices }

{
}

HRESULT CVIBuffer_Instancing::Initialize_Prototype(const INSTANCE_DESC& Desc)
{
	m_iNumInstance = Desc.iNumInstance;
	m_vCenterPos = Desc.vCenter;
	m_vRange = Desc.vRange;
	m_vSize = Desc.vSize;

	return S_OK;
}

HRESULT CVIBuffer_Instancing::Initialize(void* pArg)
{
	if (FAILED(m_pDevice->CreateBuffer(&m_InstanceBufferDesc, &m_InstanceInitialData, &m_pVBInstance)))
		return E_FAIL;

    return S_OK;
}

HRESULT CVIBuffer_Instancing::Bind_Buffers()
{
	//일반 VB, 행렬을 가진 VB 바인드
	ID3D11Buffer* pVertexBuffers[] = {
			m_pVB,
			m_pVBInstance,
	};

	_uint		iVertexStrides[] = {
		m_iVertexStride,
		m_iInstanceStride,
	};

	_uint		iOffsets[] = {
		0,
		0,
	};

	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iVertexStrides, iOffsets);
	m_pContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);
	m_pContext->IASetPrimitiveTopology(m_eTopology);

	return S_OK;
}

HRESULT CVIBuffer_Instancing::Render()
{
	m_pContext->DrawIndexedInstanced(m_iIndexCountPerInstance, m_iNumInstance, 0, 0, 0);

	return S_OK;
}

void CVIBuffer_Instancing::Free()
{
	__super::Free();

	if (false == m_isCloned)
	{
		Safe_Delete_Array(m_pInstanceVertices);
	}

	Safe_Release(m_pVBInstance);
}
