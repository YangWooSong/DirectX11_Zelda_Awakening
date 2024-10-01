#include "..\Public\VIBuffer_Instancing.h"

#include "GameInstance.h"

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
	, m_vPivotPos{ Prototype.m_vPivotPos }
	, m_vRange{ Prototype.m_vRange }
	, m_pInstanceVertices{ Prototype.m_pInstanceVertices }
	, m_pSpeed{ Prototype.m_pSpeed }
	, m_vLifeTime{ Prototype.m_vLifeTime }
	, m_isLoop{ Prototype.m_isLoop }
	, m_vSize{ Prototype.m_vSize }

{
}

HRESULT CVIBuffer_Instancing::Initialize_Prototype(void* pArg)
{
	return S_OK;
}

HRESULT CVIBuffer_Instancing::Initialize(void* pArg)
{
	INSTANCE_DESC* pDesc = static_cast<INSTANCE_DESC*>(pArg);

	m_iNumInstance = pDesc->iNumInstance;
	m_vCenterPos = pDesc->vCenter;
	m_vRange = pDesc->vRange;
	m_vSize = pDesc->vSize;
	m_vPivotPos = pDesc->vPivot;
	m_vLifeTime = pDesc->vLifeTime;
	m_isLoop = pDesc->isLoop;

	m_pSpeed = new _float[m_iNumInstance];

	for (size_t i = 0; i < m_iNumInstance; i++)
		m_pSpeed[i] = m_pGameInstance->Get_Random(pDesc->vSpeed.x, pDesc->vSpeed.y);


	return S_OK;
}

HRESULT CVIBuffer_Instancing::Bind_Buffers()
{
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
	
	Safe_Delete_Array(m_pSpeed);
	Safe_Delete_Array(m_pInstanceVertices);
	
	Safe_Release(m_pVBInstance);
}

