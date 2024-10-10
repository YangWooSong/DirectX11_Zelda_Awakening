#include "..\Public\Frustum.h"
#include "GameInstance.h"

CFrustum::CFrustum()
	: m_pGameInstance{ CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CFrustum::Initialize()
{
	//투영 스페이스 좌표를 기준으로 8개를 찍는다.
	//카메라에 가까운 쪽 (작은 네모 면)
	m_vPoints[0] = _float3(-1.f, 1.f, 0.f);
	m_vPoints[1] = _float3(1.f, 1.f, 0.f);
	m_vPoints[2] = _float3(1.f, -1.f, 0.f);
	m_vPoints[3] = _float3(-1.f, -1.f, 0.f);

	//카메라에서 먼 쪽 (큰 네모 면)
	m_vPoints[4] = _float3(-1.f, 1.f, 1.f);
	m_vPoints[5] = _float3(1.f, 1.f, 1.f);
	m_vPoints[6] = _float3(1.f, -1.f, 1.f);
	m_vPoints[7] = _float3(-1.f, -1.f, 1.f);

	return S_OK;
}

void CFrustum::Update()
{
	_matrix			ViewMatrixInv, ProjMatrixInv;

	ViewMatrixInv = m_pGameInstance->Get_Transform_Inverse_Matrix(CPipeLine::D3DTS_VIEW);
	ProjMatrixInv = m_pGameInstance->Get_Transform_Inverse_Matrix(CPipeLine::D3DTS_PROJ);

	_float3			vPoints[8];

	for (size_t i = 0; i < 8; i++)
	{
		//절두체의 6점을 WorldSpace로 옮긴다.
		XMStoreFloat3(&vPoints[i], XMVector3TransformCoord(XMLoadFloat3(&m_vPoints[i]), ProjMatrixInv));
		XMStoreFloat3(&vPoints[i], XMVector3TransformCoord(XMLoadFloat3(&vPoints[i]), ViewMatrixInv));
	}

	//평면을 만들어 저장
	Make_Plane(vPoints, m_vPlanes_InWorldSpace);
}

_bool CFrustum::isIn_WorldSpace(_fvector vPosition, _float fRadius)
{
	//면 하나라도 밖에 있으면 false
	for (size_t i = 0; i < 6; i++)
	{
		//결과가 0 == 면 위에 있다, 양수 == 밖에 있다, 음수 == 뒤에 있다, 해는 면과의 거리
		//인자로 받은 범위(fRadius)보다 거리가 멀면 false ==> 밖에 있으니 그리지 않겠다.
		if (fRadius < XMVectorGetX(XMPlaneDotCoord(XMLoadFloat4(&m_vPlanes_InWorldSpace[i]), vPosition)))
			return false;
	}

	return true;
}

void CFrustum::Make_Plane(const _float3* pPoints, _float4* pPlanes)
{
	/*XMPlaneFromPointNormal()*/

	/* +x */
	XMStoreFloat4(&pPlanes[0], XMPlaneFromPoints(XMLoadFloat3(&pPoints[1]), XMLoadFloat3(&pPoints[5]), XMLoadFloat3(&pPoints[6])));
	/* -x */
	XMStoreFloat4(&pPlanes[1], XMPlaneFromPoints(XMLoadFloat3(&pPoints[4]), XMLoadFloat3(&pPoints[0]), XMLoadFloat3(&pPoints[3])));

	/* +y */
	XMStoreFloat4(&pPlanes[2], XMPlaneFromPoints(XMLoadFloat3(&pPoints[4]), XMLoadFloat3(&pPoints[5]), XMLoadFloat3(&pPoints[1])));
	/* -y */
	XMStoreFloat4(&pPlanes[3], XMPlaneFromPoints(XMLoadFloat3(&pPoints[3]), XMLoadFloat3(&pPoints[2]), XMLoadFloat3(&pPoints[6])));

	/* +z */
	XMStoreFloat4(&pPlanes[4], XMPlaneFromPoints(XMLoadFloat3(&pPoints[5]), XMLoadFloat3(&pPoints[4]), XMLoadFloat3(&pPoints[7])));
	/* -z */
	XMStoreFloat4(&pPlanes[5], XMPlaneFromPoints(XMLoadFloat3(&pPoints[0]), XMLoadFloat3(&pPoints[1]), XMLoadFloat3(&pPoints[2])));
}

CFrustum* CFrustum::Create()
{
	CFrustum* pInstance = new CFrustum();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Cloned : CFrustum"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFrustum::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);
}
