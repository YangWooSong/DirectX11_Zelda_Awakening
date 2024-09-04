#include "..\Public\Cell.h"

#ifdef _DEBUG
#include "VIBuffer_Cell.h"
#endif

CCell::CCell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

_float3 CCell::Get_Cell_MiddlePos()
{
	_float3 vNew = { (m_vPoints[POINT_A].x + m_vPoints[POINT_B].x + m_vPoints[POINT_C].x) / 3.f, (m_vPoints[POINT_A].y + m_vPoints[POINT_B].y + m_vPoints[POINT_C].y) / 3.f, (m_vPoints[POINT_A].z + m_vPoints[POINT_B].z + m_vPoints[POINT_C].z) / 3.f };
	return vNew;
}

HRESULT CCell::Initialize(const _float3* pPoints, _int iIndex)
{
	memcpy(m_vPoints, pPoints, sizeof(_float3) * POINT_END);

	m_iIndex = iIndex;

#ifdef _DEBUG
	m_pVIBuffer = CVIBuffer_Cell::Create(m_pDevice, m_pContext, m_vPoints);
	if (nullptr == m_pVIBuffer)
		return E_FAIL;
#endif

	return S_OK;
}

_bool CCell::Compare_Points(_fvector vSour, _fvector vDest)
{
	if (true == XMVector3Equal(vSour, XMLoadFloat3(&m_vPoints[POINT_A])))
	{
		if (true == XMVector3Equal(vDest, XMLoadFloat3(&m_vPoints[POINT_B])))
			return true;
		if (true == XMVector3Equal(vDest, XMLoadFloat3(&m_vPoints[POINT_C])))
			return true;
	}

	if (true == XMVector3Equal(vSour, XMLoadFloat3(&m_vPoints[POINT_B])))
	{
		if (true == XMVector3Equal(vDest, XMLoadFloat3(&m_vPoints[POINT_C])))
			return true;
		if (true == XMVector3Equal(vDest, XMLoadFloat3(&m_vPoints[POINT_A])))
			return true;
	}

	if (true == XMVector3Equal(vSour, XMLoadFloat3(&m_vPoints[POINT_C])))
	{
		if (true == XMVector3Equal(vDest, XMLoadFloat3(&m_vPoints[POINT_A])))
			return true;
		if (true == XMVector3Equal(vDest, XMLoadFloat3(&m_vPoints[POINT_B])))
			return true;
	}

	return false;
}

_bool CCell::isIn(_fvector vPosition, _int* pNeighborIndex, _vector* vOutLine)
{
	for (size_t i = 0; i < LINE_END; i++)
	{
		_vector		vSour, vDest;

		vSour = XMVector3Normalize(vPosition - XMLoadFloat3(&m_vPoints[i]));

		_vector		vLine = XMLoadFloat3(&m_vPoints[(i + 1) % 3]) - XMLoadFloat3(&m_vPoints[i]);
		vDest = XMVectorSet(XMVectorGetZ(vLine) * -1.f, 0.f, XMVectorGetX(vLine), 0.f);

		if (0 < XMVectorGetX(XMVector3Dot(vSour, vDest)))
		{
			*vOutLine = vLine;
			*pNeighborIndex = m_iNeighborIndices[i];
			return false;
		}

	}

	return true;
}

_float CCell::Culculate_InputAngle(_fvector vLook, _fvector vOutLine)
{
	_vector vLine = vOutLine;

	_vector vLookInverse = XMVectorNegate(vLook);

	_float fRadianAngle = XMVectorGetX(XMVector3AngleBetweenNormals(XMVector3Normalize(vLine), XMVector3Normalize(vLookInverse)));
	_float fDegreeAngle = XMConvertToDegrees(fRadianAngle);

	return fDegreeAngle;
}

_vector CCell::Culculate_SlidePos(_fvector vLook, _float fSpeed, _float fTimeDelta, _fvector vOutLine)
{
	_vector vSour = XMVector3Normalize(vLook) * fSpeed * fTimeDelta;
	_vector vLine = vOutLine;
	_vector vDest = XMVector3Normalize(-XMVectorSet(XMVectorGetZ(vLine), XMVectorGetY(vLine), XMVectorGetX(vLine), 0.f));
	_float dotProduct = XMVectorGetX(XMVector3Dot(vSour, vDest));
	_vector vProjection = XMVectorScale(vDest, dotProduct) * -1;
	_vector vNew = vProjection + vSour;

	return vNew;
}

_vector CCell::Calculate_ReflectVec(_fvector vLook, _float fSpeed, _float fTimeDelta, _fvector vOutLine)
{
	_vector vLinePerpendicular;

	vLinePerpendicular = XMVector3Normalize(-XMVectorSet(XMVectorGetZ(vOutLine), XMVectorGetY(vOutLine), XMVectorGetX(vOutLine), 0.f));

	return XMVector3Normalize(vLook + (XMVector3Dot(-vLook, vLinePerpendicular) * vLinePerpendicular * 2));
}

void CCell::CompareAndChange(_float3 _comparePos, _float3 _changePos)
{
	for (size_t i = 0; i < POINT_END; ++i)
	{
		if (true == XMVector3Equal(XMLoadFloat3(&_comparePos), XMLoadFloat3(&m_vPoints[i])))
		{
			m_vPoints[i] = _changePos;

#ifdef _DEBUG
			Safe_Release(m_pVIBuffer);
#endif
#ifdef _DEBUG
			m_pVIBuffer = CVIBuffer_Cell::Create(m_pDevice, m_pContext, m_vPoints);
			if (nullptr == m_pVIBuffer)
				return;
#endif
		}
	}
}

#ifdef _DEBUG

HRESULT CCell::Render()
{
	m_pVIBuffer->Bind_Buffers();

	m_pVIBuffer->Render();

	return S_OK;
}
#endif

CCell* CCell::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* pPoints, _int iIndex)
{
	CCell* pInstance = new CCell(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pPoints, iIndex)))
	{
		MSG_BOX(TEXT("Failed to Created : CCell"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CCell::Free()
{
	__super::Free();

#ifdef _DEBUG
	Safe_Release(m_pVIBuffer);
#endif

	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
}
