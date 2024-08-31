#include "..\Public\Navigation.h"
#include "Cell.h"

#include "Shader.h"
#include "GameInstance.h"
#include<filesystem>
#include<fstream>
#include <locale>
#include <codecvt>
#include "Transform.h"
#include "Cell.h"

_float4x4 CNavigation::m_WorldMatrix = {};

CNavigation::CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
{
}

CNavigation::CNavigation(const CNavigation& Prototype)
	: CComponent{ Prototype }
	, m_Cells{ Prototype.m_Cells }

#ifdef _DEBUG
	, m_pShader{ Prototype.m_pShader }
#endif
{
	for (auto& pCell : m_Cells)
		Safe_AddRef(pCell);

#ifdef _DEBUG
	Safe_AddRef(m_pShader);
#endif
}

HRESULT CNavigation::Initialize_Prototype(const _wstring& strNavigationDataFile)
{
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

	// wstring을 string으로 변환
	std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
	std::string narrowStr = converter.to_bytes(strNavigationDataFile);

	// 변환된 narrow string을 const char 배열로 복사
	const char* cFile = narrowStr.c_str();

	ifstream fin(cFile, ios::in | ios::binary);


	//	fin.open("../Bin/Map_Data.txt");
	if (!fin.is_open())    // 파일 열었다면
	{
		return E_FAIL;
	}

	_uint iCellCout = { 0 };
	//getline(fin, line);
	//LayerCout = std::stoi(line);
	fin.read(reinterpret_cast<char*>(&iCellCout), sizeof(iCellCout));

	_float3 vPos[3] = {};

	for (int i = 0; i < iCellCout; ++i)
	{
		_uint iCellType = { };
		fin.read(reinterpret_cast<char*>(&iCellType), sizeof(iCellType));

		_uint iRoomNum = { };
		fin.read(reinterpret_cast<char*>(&iRoomNum), sizeof(iRoomNum));

		for (int j = 0; j < 3; ++j)
		{
			fin.read(reinterpret_cast<char*>(&vPos[j].x), sizeof(_float));
			fin.read(reinterpret_cast<char*>(&vPos[j].y), sizeof(_float));
			fin.read(reinterpret_cast<char*>(&vPos[j].z), sizeof(_float));
		}

		CCell* pCell = CCell::Create(m_pDevice, m_pContext, vPos, m_Cells.size());
		if (nullptr == pCell)
			return E_FAIL;

		pCell->Set_CellType(iCellType);
		pCell->Set_RoomNum(iRoomNum);

		m_Cells.emplace_back(pCell);
	}
	
	fin.close();

	if (FAILED(SetUp_Neighbors()))
		return E_FAIL;

#ifdef _DEBUG

	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Cell.hlsl"), VTXPOS::Elements, VTXPOS::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;

#endif

	return S_OK;
}

HRESULT CNavigation::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return S_OK;

	NAVIGATION_DESC* pDesc = static_cast<NAVIGATION_DESC*>(pArg);

	m_iCurrentCellIndex = pDesc->iCurrentIndex;
	m_iOwnerType = pDesc->iOwnerType;

	return S_OK;
}

void CNavigation::Update(_fmatrix TerrainWorldMatrix)
{
	XMStoreFloat4x4(&m_WorldMatrix, TerrainWorldMatrix);
}

_bool CNavigation::isMove(_fvector vPosition)
{
	//네비게이션으로 이동하는 오브젝트의 좌표를 비교하려는 모델의 로컬로 변환
	_vector		vLocalPos = XMVector3TransformCoord(vPosition, XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix)));

	_int			iNeighborIndex = { -1 };

	/* 원래 있던 삼각형 안에서 움직인거야. */
	if (true == m_Cells[m_iCurrentCellIndex]->isIn(vLocalPos, &iNeighborIndex,&m_vOutLine))
	{
		return true;
	}

	/* 원래 있던 삼각형을 벗어난거야. */
	else
	{
		/* 나간쪽에 이웃이 있다라면. */
		if (-1 != iNeighborIndex)
		{
			while (true)
			{
				if (-1 == iNeighborIndex)
					return false;

				if(m_iOwnerType != PLAYER && m_Cells[iNeighborIndex]->Get_CellType() != CCell::CELL_FLOOR)
					return false;
				else
					if (true == m_Cells[iNeighborIndex]->isIn(vLocalPos, &iNeighborIndex, &m_vOutLine))
						break;
			}

			m_iPreCellIndex = m_iCurrentCellIndex;
			m_iCurrentCellIndex = iNeighborIndex;
			m_iCurrentCelltype = m_Cells[iNeighborIndex]->Get_CellType();
			return true;
		}

		/* 나간쪽에 이웃이 없다라면. */
		else
			return false;
	}
}

_uint CNavigation::isSlide(_fvector vLook)
{
	_float fAngle = m_Cells[m_iCurrentCellIndex]->Culculate_InputAngle(vLook, m_vOutLine);

	if ((fAngle >= 0.f && fAngle < 20.f))
	{
		return SLIDE_FORWARD;
	}
	else if ((fAngle > 160.f && fAngle <= 180.f))
		return SLIDE_BACKWARD;
	return SLIDE_END;
}

_bool CNavigation::isInTotalCell(_fvector vPosition)
{
	_vector		vLocalPos = XMVector3TransformCoord(vPosition, XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix)));
	_int			iNeighborIndex = { -1 };

	for (size_t i = 0; i < m_Cells.size(); ++i)
	{
		if (true == m_Cells[i]->isIn(vLocalPos, &iNeighborIndex, &m_vOutLine))
			return true;
	}
	return false;
}


_vector CNavigation::Culculate_SlidePos(_fvector vLook, _float fSpeed, _float fTimeDelta)
{
	return  m_Cells[m_iCurrentCellIndex]->Culculate_SlidePos(vLook, fSpeed, fTimeDelta, m_vOutLine);
}

void CNavigation::SetUp_OnCell(CTransform* pTransform, _float fOffset, _float fTimeDelta)
{
	if (m_iCurrentCellIndex < 0 || m_iCurrentCellIndex >= m_Cells.size() || m_Cells[m_iCurrentCellIndex]->Get_CellType() == CCell::CELL_JUMP || m_Cells[m_iCurrentCellIndex]->Get_CellType() == CCell::CELL_JUMP)
		return;

	_vector      vLocalPos = XMVector3TransformCoord(pTransform->Get_State(CTransform::STATE_POSITION), XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix)));

	_vector vPointA = m_Cells[m_iCurrentCellIndex]->Get_Point(CCell::POINT_A);
	_vector vPointB = m_Cells[m_iCurrentCellIndex]->Get_Point(CCell::POINT_B);
	_vector vPointC = m_Cells[m_iCurrentCellIndex]->Get_Point(CCell::POINT_C);

	// 각각 거리를 구함
	_float fDistanceA = XMVectorGetX(XMVector3Length(vPointA - vLocalPos));
	_float fDistanceB = XMVectorGetX(XMVector3Length(vPointB - vLocalPos));
	_float fDistanceC = XMVectorGetX(XMVector3Length(vPointC - vLocalPos));

	_float fMaxDistance = max(max(fDistanceA, fDistanceB), fDistanceC);
    _float fMinDistance = min(min(fDistanceA, fDistanceB), fDistanceC);
	_float fMidDistance = 0.f;

	_float fMaxFarY = 0.f;
	_float fMidFarY = 0.f;
	_float fMinFarY = 0.f;

	if (fMaxDistance != fDistanceA && fMinDistance != fDistanceA)
	{
		fMidDistance = fDistanceA;
		fMidFarY = XMVectorGetY(vPointA);
	}
	else if (fMaxDistance != fDistanceB && fMinDistance != fDistanceB)
	{
		fMidDistance = fDistanceB;
		fMidFarY = XMVectorGetY(vPointB);
	}
	else if (fMaxDistance != fDistanceC && fMinDistance != fDistanceC)
	{
		fMidDistance = fDistanceC;
		fMidFarY = XMVectorGetY(vPointC);
	}

	if (fMaxDistance == fDistanceA)
	{
		fMaxFarY = XMVectorGetY(vPointA);
	}
	else if (fMaxDistance == fDistanceB)
	{
		fMaxFarY = XMVectorGetY(vPointB);
	}
	else if (fMaxDistance == fDistanceC)
	{
		fMaxFarY = XMVectorGetY(vPointC);
	}

	if (fMinDistance == fDistanceA)
	{
		fMinFarY = XMVectorGetY(vPointA);
	}
	else if (fMinDistance == fDistanceB)
	{
		fMinFarY = XMVectorGetY(vPointB);
	}
	else if (fMinDistance == fDistanceC)
	{
		fMinFarY = XMVectorGetY(vPointC);
	}

	// 총 거리의 합
	_float fTotalDistance = fMaxDistance + fMinDistance + fMidDistance;

	_float fNewY = (fMaxDistance * fMinFarY + fMidDistance * fMidFarY + fMinDistance * fMaxFarY) / fTotalDistance;

	_vector vNewLocalPos = XMVectorSetY(vLocalPos, fNewY + fOffset);
	vLocalPos = XMVectorLerp(vLocalPos, vNewLocalPos, 8.f * fTimeDelta);

	_vector vWorldPos = XMVector3TransformCoord(vLocalPos, XMLoadFloat4x4(&m_WorldMatrix));

	pTransform->Set_State(CTransform::STATE_POSITION, vWorldPos);

}


#ifdef _DEBUG

_float3 CNavigation::Get_MiddlePosOfPreCell()
{
	m_iCurrentCellIndex = m_iPreCellIndex;
	m_iCurrentCelltype = CCell::CELL_FLOOR;
	return m_Cells[m_iPreCellIndex]->Get_Cell_MiddlePos();
}

HRESULT CNavigation::Render()
{

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	_float4		vColor = -1 == m_iCurrentCellIndex ? _float4(0.f, 1.f, 0.f, 1.f) : _float4(1.f, 0.f, 0.f, 1.f);
	_float4x4	WorldMatrix = m_WorldMatrix;

	if (-1 != m_iCurrentCellIndex)
		WorldMatrix._42 += 0.1f;

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &WorldMatrix)))
		return E_FAIL;

	m_pShader->Bind_RawValue("g_vColor", &vColor, sizeof(_float4));

	m_pShader->Begin(0);


	if (-1 == m_iCurrentCellIndex)
	{
		for (auto& pCell : m_Cells)
			pCell->Render();
	}
	else
	{
		m_Cells[m_iCurrentCellIndex]->Render();
	}
	return S_OK;
}

#endif

HRESULT CNavigation::SetUp_Neighbors()
{
	for (auto& pSourCell : m_Cells)
	{
		for (auto& pDestCell : m_Cells)
		{
			if (pSourCell == pDestCell)
				continue;

			if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_A), pSourCell->Get_Point(CCell::POINT_B)))
			{
				pSourCell->Set_Neighbor(CCell::LINE_AB, pDestCell);
			}

			if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_B), pSourCell->Get_Point(CCell::POINT_C)))
			{
				pSourCell->Set_Neighbor(CCell::LINE_BC, pDestCell);
			}

			if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_C), pSourCell->Get_Point(CCell::POINT_A)))
			{
				pSourCell->Set_Neighbor(CCell::LINE_CA, pDestCell);
			}
		}
	}

	return S_OK;
}

CNavigation* CNavigation::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _wstring& strNavigationDataFile)
{
	CNavigation* pInstance = new CNavigation(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(strNavigationDataFile)))
	{
		MSG_BOX(TEXT("Failed to Created : CNavigation"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CComponent* CNavigation::Clone(void* pArg)
{
	CNavigation* pInstance = new CNavigation(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CNavigation"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNavigation::Free()
{
	__super::Free();

	for (auto& pCell : m_Cells)
		Safe_Release(pCell);
	

	m_Cells.clear();

#ifdef _DEBUG
	Safe_Release(m_pShader);
#endif
}
