#include "..\Public\VIBuffer_Model_Instance.h"
//#include <cmath>
#include "GameInstance.h"

CVIBuffer_Model_Instance::CVIBuffer_Model_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer_Instancing{ pDevice, pContext }
{
}

CVIBuffer_Model_Instance::CVIBuffer_Model_Instance(const CVIBuffer_Model_Instance& Prototype)
	: CVIBuffer_Instancing{ Prototype }
{
}

HRESULT CVIBuffer_Model_Instance::Initialize_Prototype(const CVIBuffer_Instancing::INSTANCE_DESC& Desc)
{
	if (FAILED(__super::Initialize_Prototype(Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CVIBuffer_Model_Instance::Initialize(void* pArg)
{
	MODEL_INSTANCE_DESC* pDesc = static_cast<MODEL_INSTANCE_DESC*>(pArg);

	m_iNumVertexBuffers = 2;
	m_iNumVertices = pDesc->iNumVertices;
	m_iVertexStride = sizeof(VTXMESH); // 여기 정보 잘못되어있었음
	m_iNumIndices = pDesc->iNumIndices;
	m_iIndexStride = 4;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;	// 여기 16비트로 되어있었음
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_iInstanceStride = sizeof(VTXMODELINSTANCE);
	m_iIndexCountPerInstance = m_iNumIndices;

#pragma region VERTEX_BUFFER

	m_pVB = pDesc->pVB;
	Safe_AddRef(m_pVB);
#pragma endregion

#pragma region INDEX_BUFFER

	m_pIB = pDesc->pIB;
	Safe_AddRef(m_pIB);
#pragma endregion

#pragma region INSTANCE_BUFFER

	ZeroMemory(&m_InstanceBufferDesc, sizeof m_InstanceBufferDesc);
	m_InstanceBufferDesc.ByteWidth = m_iInstanceStride * m_iNumInstance;
	m_InstanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC; /* 동적버퍼로 생성한다. */
	m_InstanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_InstanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_InstanceBufferDesc.MiscFlags = 0;
	m_InstanceBufferDesc.StructureByteStride = m_iInstanceStride;

	m_pInstanceVertices = new VTXMODELINSTANCE[m_iNumInstance];
	ZeroMemory(m_pInstanceVertices, sizeof(VTXMODELINSTANCE) * m_iNumInstance);

	VTXMODELINSTANCE* pInstanceVertices = static_cast<VTXMODELINSTANCE*>(m_pInstanceVertices);

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		_float	fScale = m_pGameInstance->Get_Random(m_vSize.x, m_vSize.y);

		//회전하면 큰일 남
		pInstanceVertices[i].vRight = _float4(fScale, 0.f, 0.f, 0.f);
		pInstanceVertices[i].vUp = _float4(0.f, fScale, 0.f, 0.f);
		pInstanceVertices[i].vLook = _float4(0.f, 0.f, fScale, 0.f);
		pInstanceVertices[i].vTranslation = _float4(
			m_pGameInstance->Get_Random(m_vCenterPos.x - m_vRange.x * 0.5f, m_vCenterPos.x + m_vRange.x * 0.5f),
			m_pGameInstance->Get_Random(m_vCenterPos.y - m_vRange.y * 0.5f, m_vCenterPos.y + m_vRange.y * 0.5f),
			m_pGameInstance->Get_Random(m_vCenterPos.z - m_vRange.z * 0.5f, m_vCenterPos.z + m_vRange.z * 0.5f),
			1.f);

		pInstanceVertices[i].vLifeTime = _float2(m_pGameInstance->Get_Random(m_vLifeTime.x, m_vLifeTime.y), 0.0f);
	}

	ZeroMemory(&m_InstanceInitialData, sizeof m_InstanceInitialData);
	m_InstanceInitialData.pSysMem = m_pInstanceVertices;

#pragma endregion

#pragma region INSTANCE_BUFFER

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

#pragma endregion

	return S_OK;
}

void CVIBuffer_Model_Instance::Spread(_float fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE	SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTXMODELINSTANCE* pVertices = static_cast<VTXMODELINSTANCE*>(SubResource.pData);

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		//이동
		_vector		vMoveDir = XMVectorSetW(XMLoadFloat4(&pVertices[i].vTranslation) - XMLoadFloat3(&m_vPivotPos), 0.f);
		
		XMStoreFloat4(&pVertices[i].vTranslation,
			XMLoadFloat4(&pVertices[i].vTranslation) + XMVector3Normalize(vMoveDir) * m_pSpeed[i] * fTimeDelta);

		///////////////회전
		
		float fYaw = 0.f;
		float fPitch = 0.f;
		float fRoll = 0.f;
		// 회전 각도 (Yaw, Pitch, Roll 값 조정 가능)
		
		fYaw = fTimeDelta * m_pSpeed[i];  // Y축 회전 각도
		fPitch = fTimeDelta * m_pSpeed[i];  // X축 회전 각도
		fRoll = fTimeDelta * m_pSpeed[i];  // Z축 회전 각도

		if (i % 3 == 0)
		{
			fPitch = -fTimeDelta * m_pSpeed[i];
		}

		// 회전 행렬 생성
		XMMATRIX matRotationYaw = XMMatrixRotationY(fYaw);
		XMMATRIX matRotationPitch = XMMatrixRotationX(fPitch);
		XMMATRIX matRotationRoll = XMMatrixRotationZ(fRoll);

		// 회전 행렬 적용 (Yaw -> Pitch -> Roll 순서)
		XMMATRIX matRotation = matRotationYaw * matRotationPitch * matRotationRoll;

		// 각 인스턴스의 Look, Right, Up 벡터에 회전 적용
		XMVECTOR vRight = XMLoadFloat4(&pVertices[i].vRight);
		XMVECTOR vUp = XMLoadFloat4(&pVertices[i].vUp);
		XMVECTOR vLook = XMLoadFloat4(&pVertices[i].vLook);

		vRight = XMVector3TransformNormal(vRight, matRotation);
		vUp = XMVector3TransformNormal(vUp, matRotation);
		vLook = XMVector3TransformNormal(vLook, matRotation);

		XMStoreFloat4(&pVertices[i].vRight, vRight);
		XMStoreFloat4(&pVertices[i].vUp, vUp);
		XMStoreFloat4(&pVertices[i].vLook, vLook);

	

		//y축 제한
		if (pVertices[i].vTranslation.y <= 0)
			pVertices[i].vTranslation.y = 0;
		///////////////////////
		pVertices[i].vLifeTime.y += fTimeDelta;

		//lifetime 넘기면
		if (true == m_isLoop && pVertices[i].vLifeTime.y >= pVertices[i].vLifeTime.x)
		{
			pVertices[i].vTranslation = static_cast<VTXMODELINSTANCE*>(m_pInstanceVertices)[i].vTranslation;
			pVertices[i].vLifeTime.y = 0.f;
		}
	}

	m_pContext->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_Model_Instance::Drop(_float fTimeDelta)
{
}

void CVIBuffer_Model_Instance::PurpleQuartz_Spread(_float fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE	SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTXMODELINSTANCE* pVertices = static_cast<VTXMODELINSTANCE*>(SubResource.pData);

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		//이동
		_vector		vMoveDir = XMVectorSetW(XMLoadFloat4(&pVertices[i].vTranslation) - XMLoadFloat3(&m_vPivotPos), 0.f);

		XMStoreFloat4(&pVertices[i].vTranslation,
			XMLoadFloat4(&pVertices[i].vTranslation) + XMVector3Normalize(vMoveDir) * m_pSpeed[i] * fTimeDelta);

		///////////////회전

		float fYaw = 0.f;
		float fPitch = 0.f;
		float fRoll = 0.f;
		// 회전 각도 (Yaw, Pitch, Roll 값 조정 가능)

		fYaw = fTimeDelta * m_pSpeed[i];  // Y축 회전 각도
		fPitch = fTimeDelta * m_pSpeed[i];  // X축 회전 각도
		fRoll = fTimeDelta * m_pSpeed[i];  // Z축 회전 각도

		if (i % 3 == 0)
		{
			fPitch = -fTimeDelta * m_pSpeed[i];
		}

		// 회전 행렬 생성
		XMMATRIX matRotationYaw = XMMatrixRotationY(fYaw);
		XMMATRIX matRotationPitch = XMMatrixRotationX(fPitch);
		XMMATRIX matRotationRoll = XMMatrixRotationZ(fRoll);

		// 회전 행렬 적용 (Yaw -> Pitch -> Roll 순서)
		XMMATRIX matRotation = matRotationYaw * matRotationPitch * matRotationRoll;

		// 각 인스턴스의 Look, Right, Up 벡터에 회전 적용
		XMVECTOR vRight = XMLoadFloat4(&pVertices[i].vRight);
		XMVECTOR vUp = XMLoadFloat4(&pVertices[i].vUp);
		XMVECTOR vLook = XMLoadFloat4(&pVertices[i].vLook);

		vRight = XMVector3TransformNormal(vRight, matRotation);
		vUp = XMVector3TransformNormal(vUp, matRotation);
		vLook = XMVector3TransformNormal(vLook, matRotation);

		XMStoreFloat4(&pVertices[i].vRight, vRight);
		XMStoreFloat4(&pVertices[i].vUp, vUp);
		XMStoreFloat4(&pVertices[i].vLook, vLook);



		//y축 제한
		if (pVertices[i].vTranslation.y <= 0)
			pVertices[i].vTranslation.y = 0;
		///////////////////////
		pVertices[i].vLifeTime.y += fTimeDelta;

		//lifetime 넘기면
		if (true == m_isLoop && pVertices[i].vLifeTime.y >= pVertices[i].vLifeTime.x)
		{
			pVertices[i].vTranslation = static_cast<VTXMODELINSTANCE*>(m_pInstanceVertices)[i].vTranslation;
			pVertices[i].vLifeTime.y = 0.f;
		}
	}

	m_pContext->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_Model_Instance::HousePot_Spread(_float fTimeDelta, _float3 vTarget)
{

	D3D11_MAPPED_SUBRESOURCE	SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTXMODELINSTANCE* pVertices = static_cast<VTXMODELINSTANCE*>(SubResource.pData);

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		//이동
		_vector		vMoveDir = -XMLoadFloat3(&vTarget);

		XMStoreFloat4(&pVertices[i].vTranslation,
			XMLoadFloat4(&pVertices[i].vTranslation) + XMVector3Normalize(vMoveDir) * m_pSpeed[i] * fTimeDelta);

		///////////////회전

		float fYaw = XMConvertToRadians( m_pGameInstance->Get_Random(0.f, 360.f));
		float fPitch = XMConvertToRadians(m_pGameInstance->Get_Random(0.f, 360.f));
		float fRoll = XMConvertToRadians(m_pGameInstance->Get_Random(0.f, 360.f));
		// 회전 각도 (Yaw, Pitch, Roll 값 조정 가능)
		//fTimeDelta * m_pSpeed[i]
		fYaw = fTimeDelta * m_pSpeed[i];  // Y축 회전 각도
		fPitch = fTimeDelta * m_pSpeed[i];  // X축 회전 각도
		fRoll = fTimeDelta * m_pSpeed[i];  // Z축 회전 각도

		if (i % 3 == 0)
		{
			fPitch = -fTimeDelta * m_pSpeed[i];
		}

		// 회전 행렬 생성
		XMMATRIX matRotationYaw = XMMatrixRotationY(fYaw);
		XMMATRIX matRotationPitch = XMMatrixRotationX(fPitch);
		XMMATRIX matRotationRoll = XMMatrixRotationZ(fRoll);

		// 회전 행렬 적용 (Yaw -> Pitch -> Roll 순서)
		XMMATRIX matRotation = matRotationYaw * matRotationPitch * matRotationRoll;

		// 각 인스턴스의 Look, Right, Up 벡터에 회전 적용
		XMVECTOR vRight = XMLoadFloat4(&pVertices[i].vRight);
		XMVECTOR vUp = XMLoadFloat4(&pVertices[i].vUp);
		XMVECTOR vLook = XMLoadFloat4(&pVertices[i].vLook);

		vRight = XMVector3TransformNormal(vRight, matRotation);
		vUp = XMVector3TransformNormal(vUp, matRotation);
		vLook = XMVector3TransformNormal(vLook, matRotation);

		XMStoreFloat4(&pVertices[i].vRight, vRight);
		XMStoreFloat4(&pVertices[i].vUp, vUp);
		XMStoreFloat4(&pVertices[i].vLook, vLook);



		//y축 제한
		if (pVertices[i].vTranslation.y <= 0)
			pVertices[i].vTranslation.y = 0;
		///////////////////////
		pVertices[i].vLifeTime.y += fTimeDelta;

		//lifetime 넘기면
		if (true == m_isLoop && pVertices[i].vLifeTime.y >= pVertices[i].vLifeTime.x)
		{
			pVertices[i].vTranslation = static_cast<VTXMODELINSTANCE*>(m_pInstanceVertices)[i].vTranslation;
			pVertices[i].vLifeTime.y = 0.f;
		}
	}

	m_pContext->Unmap(m_pVBInstance, 0);
}




CVIBuffer_Model_Instance* CVIBuffer_Model_Instance::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const CVIBuffer_Instancing::INSTANCE_DESC& Desc)
{
	CVIBuffer_Model_Instance* pInstance = new CVIBuffer_Model_Instance(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(Desc)))
	{
		MSG_BOX(TEXT("Failed to Created : CVIBuffer_Model_Instance"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CComponent* CVIBuffer_Model_Instance::Clone(void* pArg)
{
	CVIBuffer_Model_Instance* pInstance = new CVIBuffer_Model_Instance(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CVIBuffer_Model_Instance"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Model_Instance::Free()
{
	__super::Free();

}
