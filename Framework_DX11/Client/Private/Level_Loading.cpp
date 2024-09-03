#include "stdafx.h"
#include "..\Public\Level_Loading.h"

#include "Loader.h"
#include "Level_Logo.h"
#include "Level_MarinHouse.h"
#include "Level_Field.h"
#include "Level_Test.h"
#include "Level_Dungeon.h"

#include "BackGround.h"
#include "GameInstance.h"

CLevel_Loading::CLevel_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel { pDevice, pContext }
{
}

HRESULT CLevel_Loading::Initialize(LEVELID eNextLevelID)
{
	m_eNextLevelID = eNextLevelID;

	m_pLoader = CLoader::Create(m_pDevice, m_pContext, m_eNextLevelID);
	if (nullptr == m_pLoader)
		return E_FAIL;	
		

	if (FAILED(Ready_Layer_BackGround()))
		return E_FAIL;

	return S_OK;
}

void CLevel_Loading::Update(_float fTimeDelta)
{
	

	if (true == m_pLoader->isFinished())
	{
		CLevel*			pNewLevel = { nullptr };

		switch (m_eNextLevelID)
		{
		case LEVEL_LOGO:
			pNewLevel = CLevel_Logo::Create(m_pDevice, m_pContext);
			break;
		case LEVEL_MARINHOUSE:
			pNewLevel = CLevel_MarinHouse::Create(m_pDevice, m_pContext);
			break;
		case LEVEL_FIELD:
			pNewLevel = CLevel_Field::Create(m_pDevice, m_pContext);
			break;
		case LEVEL_TEST:
			pNewLevel = CLevel_Test::Create(m_pDevice, m_pContext);
			break;
		case LEVEL_DUNGEON:
			pNewLevel = CLevel_Dungeon::Create(m_pDevice, m_pContext);
			break;
		}

		if (nullptr == pNewLevel)
			return;

		if (FAILED(m_pGameInstance->Change_Level(m_eNextLevelID, pNewLevel)))
			return;
	}	
}

HRESULT CLevel_Loading::Render()
{
	m_pLoader->Draw_LoadingText();

	return S_OK;
}

HRESULT CLevel_Loading::Ready_Layer_BackGround()
{
	CBackGround::BACKGROUND_DESC Desc{ };

	Desc.eType = CBackGround::LOADING_ICON;
	Desc.fSizeX = 60;
	Desc.fSizeY = 60;
	Desc.fX = g_iWinSizeX - g_iWinSizeX*0.2;
	Desc.fY = g_iWinSizeY - g_iWinSizeY*0.3;

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_LOADING, TEXT("Layer_BackGround"),
		TEXT("Prototype_GameObject_BackGround"), &Desc)))
		return E_FAIL;

	Desc.eType = CBackGround::LOADING_BACKGROUND;
	Desc.fSizeX = g_iWinSizeX;
	Desc.fSizeY = g_iWinSizeY;
	Desc.fX = g_iWinSizeX >> 1;
	Desc.fY = g_iWinSizeY >> 1;

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_LOADING, TEXT("Layer_BackGround"),
		TEXT("Prototype_GameObject_BackGround"), &Desc)))
		return E_FAIL;

	

	return S_OK;
}

CLevel_Loading * CLevel_Loading::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVELID eNextLevelID)
{
	CLevel_Loading*		pInstance = new CLevel_Loading(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevelID)))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_Loading"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Loading::Free()
{
	__super::Free();

	Safe_Release(m_pLoader);
}
