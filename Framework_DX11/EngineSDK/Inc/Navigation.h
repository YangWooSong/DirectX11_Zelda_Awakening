#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CNavigation final : public CComponent
{
public:
	typedef struct
	{
		_uint			iOwnerType = { TYPEEND };
		_int			iCurrentIndex = { -1 };
	}NAVIGATION_DESC;
	enum SLIDETYPE{SLIDE_FORWARD, SLIDE_BACKWARD, SLIDE_END};
	enum OWNERTYPE{PLAYER, NONPLAYER, TYPEEND};
private:
	CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNavigation(const CNavigation& Prototype);
	virtual ~CNavigation() = default;

public:
	virtual HRESULT Initialize_Prototype(const _wstring& strNavigationDataFile);
	virtual HRESULT Initialize(void* pArg) override;
	void CNavigation::SetUp_OnCell(class CTransform* pTransform, _float fOffset, _float fTimeDelta);

	void Update(_fmatrix TerrainWorldMatrix);

public:
	_bool isMove(_fvector vPosition, _bool bMovecliff = false);
	_bool isMove_in_Room(_fvector vPosition, _int iRoomNum);
	_uint isSlide(_fvector vLook);	//각도 제한으로 슬라이드 유무 확인
	_uint SlideDir(_fvector vLook);	//각도 제한이 유함, 방향만 정한다
	_bool isInTotalCell(_fvector vPosition);
	_vector Culculate_SlidePos(_fvector vLook, _float fSpeed, _float fTimeDelta);
	_vector Calculate_ReflectVec(_fvector vLook, _float fSpeed, _float fTimeDelta);
public:
	_int Get_PreCellIndex() { return m_iPreCellIndex; }
	_int Get_CurrentCellIndex() { return m_iCurrentCellIndex; }
	_vector Get_OutLIne() { return m_vOutLine; }
	_uint Get_CurrentCellType() { return m_iCurrentCelltype; }
	_uint Get_CurrentCellType(_vector vCurPos);

	_int Get_CurrentCell_RoomNum();
	_float3 Get_MiddlePosOfPreCell();

#ifdef _DEBUG
public:
	virtual HRESULT Render() override;
#endif

private:
	vector<class CCell*>				m_Cells;
	_int								m_iCurrentCellIndex = { -1 };	//현재 셀 번호
	_int								m_iPreCellIndex = { -1 };	//이전 셀 번호
	static _float4x4					m_WorldMatrix;
	_vector								m_vOutLine{};				//셀의 어떤 면으로 나갔는지 저장
	_uint								m_iOwnerType = { TYPEEND };
	_uint								m_iCurrentCelltype = { 0 };

#ifdef _DEBUG
private:
	class CShader* m_pShader = { nullptr };
#endif

private:
	HRESULT SetUp_Neighbors();

public:
	static CNavigation* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _wstring& strNavigationDataFile);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END