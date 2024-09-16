#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CCell final : public CBase
{
public:
	typedef struct
	{
		_uint iCellType;
		_uint iRoomNum;
	}CELL_DESC;
public:
	enum POINT { POINT_A, POINT_B, POINT_C, POINT_END };
	enum LINE { LINE_AB, LINE_BC, LINE_CA, LINE_END };
	enum CELLTYPE { CELL_FLOOR, CELL_CLIFF, CELL_JUMP, CELLTYPE_END };
private:
	CCell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CCell() = default;

public:
	_vector Get_Point(POINT ePoint) const {
		return XMLoadFloat3(&m_vPoints[ePoint]);
	}

	void Set_Neighbor(LINE eLine, CCell* pNeighbor) {
		m_iNeighborIndices[eLine] = pNeighbor->m_iIndex;
	}

	_float3* Get_All_Points() { return m_vPoints; }

	void Set_CellType(_uint _iType) { m_iCellType = _iType; }
	_uint Get_CellType() { return m_iCellType; }

	void Set_RoomNum(_uint _iNum) { m_iRoomNum = _iNum; }
	_uint Get_RoomNum() { return m_iRoomNum; }

	_float3 Get_Cell_MiddlePos();
public:
	HRESULT Initialize(const _float3* pPoints, _int iIndex);
	_bool Compare_Points(_fvector vSour, _fvector vDest);
	_bool isIn(_fvector vPosition, _int* pNeighborIndex = nullptr, _vector* vOutLine = nullptr);

	_float Culculate_InputAngle(_fvector vLook , _fvector vOutLine);
	_vector Culculate_SlidePos(_fvector vLook,_float fSpeed, _float fTimeDelta, _fvector vOutLine);
	_vector	Calculate_ReflectVec(_fvector vLook, _float fSpeed, _float fTimeDelta, _fvector vOutLine);
	void  CompareAndChange(_float3 _comparePos, _float3 _changePos);
#ifdef _DEBUG
public:
	HRESULT Render();
#endif

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	_int							m_iIndex = {};
	_float3							m_vPoints[POINT_END] = {};
	_int							m_iNeighborIndices[LINE_END] = { -1, -1, -1 };

#ifdef _DEBUG
private:
	class CVIBuffer_Cell* m_pVIBuffer = { nullptr };
#endif

	_uint m_iCellType = { CELLTYPE_END };
	_uint m_iRoomNum = { 0 };

public:
	static CCell* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* pPoints, _int iIndex);
	virtual void Free() override;
};

END