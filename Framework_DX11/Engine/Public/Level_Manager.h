#pragma once

#include "Base.h"

/* 1. 현재 할당한 레벨의 주소를 들고 있는다. */
/* 2. 현재 할당한 레벨을 갱신한다. */
/* 3. 새로운 레벨로 교체 시 기존 레벨을 파괴해주낟. + 기존 레벨용 자원(리소스들, 객체들) 을 삭제한다. */

BEGIN(Engine)

class CLevel_Manager final : public CBase
{
private:
	CLevel_Manager();
	virtual ~CLevel_Manager() = default;

public:
	class CLevel* Get_CurLevel() { return m_pCurrentLevel; }
	_uint Get_CurLevelIndex() { return m_iLevelIndex; }
	_uint Get_PreLevelIndex() { return m_iPreLevelIndex; }

public:
	HRESULT Initialize();
	void Update(_float fTimeDelta);
	HRESULT Render();
	HRESULT Change_Level(_uint iLevelIndex, class CLevel* pNextLevel);

private:
	class CLevel*			m_pCurrentLevel = { nullptr };
	class CGameInstance*	m_pGameInstance = { nullptr };

	_uint					m_iLevelIndex = {};
	_uint					m_iPreLevelIndex = {};

public:
	static CLevel_Manager* Create();
	virtual void Free() override;
};

END