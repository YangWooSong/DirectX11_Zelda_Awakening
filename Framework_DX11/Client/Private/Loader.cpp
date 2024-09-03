#include "stdafx.h"
#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
#include "..\Public\Loader.h"

#include "Terrain.h"
#include "BackGround.h"
#include "GameInstance.h"

#include <locale>
#include <codecvt>

#include "NonAnimModel.h"
#include "AnimModel.h"

#include"NavDataObj.h"

#include "Link.h"
#include "FreeCamera.h"
#include "PlayerCamera.h"
#include "Land.h"

#include "HousePot.h"
#include "Bed.h"
#include "Sword.h"
#include "Shield.h"

#include "Lawn.h"
#include "Grass.h"
#include "Tree.h"
#include "Weathercock.h"
#include "TailLockStatue.h"
#include "TailCaveShutter.h"
#include "SeaUrchin.h"
#include "Octorok.h"
#include "OctorokRock.h"

#include "Rola.h"
#include "Spark.h"
#include "Pawn.h"
#include "Togezo.h"
#include "Kuribo.h"
#include "Vegas.h"

#include "DeguTail_00.h"
#include "CollapseTile.h"

CLoader::CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
	, m_pGameInstance{ CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}



_uint APIENTRY LoadingMain(void* pArg)
{
	CoInitializeEx(nullptr, 0);

	CLoader*		pLoader = static_cast<CLoader*>(pArg);

	if(FAILED(pLoader->Loading()))
		return 1;
		
	return 0;
}


HRESULT CLoader::Initialize(LEVELID eNextLevelID)
{
	m_eNextLevelID = eNextLevelID;

	InitializeCriticalSection(&m_CriticalSection);

	Ready_Resources_For_Loading();

	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, LoadingMain, this, 0, nullptr);
	if (0 == m_hThread)
		return E_FAIL;


	return S_OK;
}

HRESULT CLoader::Loading()
{
	EnterCriticalSection(&m_CriticalSection);

	HRESULT			hr = {};

	switch (m_eNextLevelID)
	{
	case LEVEL_LOGO:
		hr = Ready_Resources_For_LogoLevel();
		break;
	case LEVEL_MARINHOUSE:
		hr = Ready_Resources_For_MarinHouse();
		break;
	case LEVEL_TEST:
		hr = Ready_Resources_For_Test();
		break;
	case LEVEL_FIELD:
		hr = Ready_Resources_For_Field();
		break;
	case LEVEL_DUNGEON:
		hr = Ready_Resources_For_Dungeon();
		break;
	}

	LeaveCriticalSection(&m_CriticalSection);

	return hr;
}

void CLoader::Draw_LoadingText()
{
	SetWindowText(g_hWnd, m_szLoadingText);
}

HRESULT CLoader::Ready_Resources_For_Loading()
{
	/* For. Prototype_GameObject_BackGround */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BackGround"),
		CBackGround::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Loading */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Loading"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Zelda/LoadingScreen.dds"), 1))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_LoadingIcon */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_LoadingIcon"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Zelda/Player_01^q.png"), 1))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Ready_Resources_For_LogoLevel()
{
	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다."));

	/* For. Prototype_Component_Texture_BackGround */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_BackGround"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Zelda/TitleScreen.jpg"), 1))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_LogoKey */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_LogoKey"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Zelda/LogoKey.png"), 1))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_LogoKeyBackGround */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_LogoKeyBackGround"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Zelda/LogoKeyBackGround.png"), 1))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("모델을(를) 로딩중입니다."));


	lstrcpy(m_szLoadingText, TEXT("사운드을(를) 로딩중입니다."));


	lstrcpy(m_szLoadingText, TEXT("객체원형을(를) 로딩중입니다."));
	
	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Ready_Resources_For_MarinHouse()
{
	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다."));

	lstrcpy(m_szLoadingText, TEXT("모델을(를) 로딩중입니다."));

	Ready_Models_For_MarinHouse();
	
	lstrcpy(m_szLoadingText, TEXT("셰이더을(를) 로딩중입니다."));

	lstrcpy(m_szLoadingText, TEXT("네비게이션을(를) 로딩중입니다."));

	/* For.Prototype_Component_Navigation */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MARINHOUSE, TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/Nav_Data/MarinHouse_Nav_Data.dat")))))
		return E_FAIL;

	/* For. Prototype_GameObject_NavDataObj*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_NavDataObj"),
		CNavDataObj::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	lstrcpy(m_szLoadingText, TEXT("사운드을(를) 로딩중입니다."));


	lstrcpy(m_szLoadingText, TEXT("객체원형을(를) 로딩중입니다."));

	/* For. Prototype_GameObject_FreeCamera */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FreeCamera"),
		CFreeCamera::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	/* For. Prototype_GameObject_PlayerCamera */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_PlayerCamera"),
		CPlayerCamera::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_PLayer_Link */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player_Link"),
		CLink::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Land*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Land"),
		CLand::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_HousePot*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_HousePot"),
		CHousePot::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Bed*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Bed"),
		CBed::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Sword*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sword"),
		CSword::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	/* For. Prototype_GameObject_Shield*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Shield"),
		CShield::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	///* For. Prototype_GameObject_NonAnim*/
	//if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_NonAnim"),
	//	CNonAnimModel::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Ready_Resources_For_Test()
{
	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다."));

	/* For. Prototype_Component_Texture_BackGround */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TEST, TEXT("Prototype_Component_Texture_Terrain"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Grass_%d.dds"), 2))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("모델을(를) 로딩중입니다."));

	Ready_Models_For_Test();

	/* For. Prototype_Component_VIBuffer_Terrain*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TEST, TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Height.bmp")))))
		return E_FAIL;

	/* For. Prototype_GameObject_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain"),
		CTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("셰이더을(를) 로딩중입니다."));

	lstrcpy(m_szLoadingText, TEXT("사운드을(를) 로딩중입니다."));

	lstrcpy(m_szLoadingText, TEXT("객체원형을(를) 로딩중입니다."));

	Ready_Prototype_For_Test();

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));
	
	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Ready_Resources_For_Field()
{
	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다."));

	lstrcpy(m_szLoadingText, TEXT("모델을(를) 로딩중입니다."));

	Ready_Models_For_Field();

	lstrcpy(m_szLoadingText, TEXT("셰이더을(를) 로딩중입니다."));

	lstrcpy(m_szLoadingText, TEXT("사운드을(를) 로딩중입니다."));

	lstrcpy(m_szLoadingText, TEXT("네비게이션을(를) 로딩중입니다."));

	/* For.Prototype_Component_Navigation */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_FIELD, TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/Nav_Data/Field_Nav_Data.dat")))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("객체원형을(를) 로딩중입니다."));
	
	Ready_Prototype_For_Field();

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Ready_Resources_For_Dungeon()
{
	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다."));

	lstrcpy(m_szLoadingText, TEXT("모델을(를) 로딩중입니다."));

	Ready_Models_For_Dungeon();

	lstrcpy(m_szLoadingText, TEXT("셰이더을(를) 로딩중입니다."));

	lstrcpy(m_szLoadingText, TEXT("사운드을(를) 로딩중입니다."));

	lstrcpy(m_szLoadingText, TEXT("네비게이션을(를) 로딩중입니다."));

	/* For.Prototype_Component_Navigation */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_DUNGEON, TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/Nav_Data/Dungeon_Nav_Data.dat")))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("객체원형을(를) 로딩중입니다."));

	Ready_Prototype_For_Dungeon();

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

	return S_OK;
}


HRESULT CLoader::Ready_Models_For_MarinHouse()
{
	/* For. Prototype_Component_VIBuffer_Terrain*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MARINHOUSE, TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Height.bmp")))))
		return E_FAIL;

	_matrix		PreTransformMatrix = XMMatrixIdentity();

	PreTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));

	/* For. Prototype_Component_Model_Link*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Link"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Link/Link.dat", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_MarinHouse*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MARINHOUSE, TEXT("Prototype_Component_Level_MarinHouse"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/ModelData/NonAnim/Level/MarinHouse/MarinHouse.dat"))))
		return E_FAIL;

	///* For. Prototype_Component_Model_Obj_Apple*/
 //	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MARINHOUSE, TEXT("Prototype_Component_Model_Obj_Apple"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/ModelData/NonAnim/Obj/AppleRed/AppleRed.dat"))))
	//	return E_FAIL;

	/* For. Prototype_Component_Model_Obj_HousePot*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MARINHOUSE, TEXT("Prototype_Component_Model_Obj_HousePot"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/ModelData/NonAnim/Obj/HousePot/HousePot.dat"))))
		return E_FAIL;

	/* For. Prototype_Component_Model_Obj_Bed*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MARINHOUSE, TEXT("Prototype_Component_Model_Obj_Bed"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Obj/Bed/Bed.dat"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Ready_Models_For_Field()
{
	_matrix		PreTransformMatrix = XMMatrixIdentity();

	PreTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));

#pragma region LAND
	for (int i = 14; i < 84; i++)
	{
		_wstring sPath = TEXT("../Bin/ModelData/NonAnim/Level/Field/Field_");
		_wstring sFBX = TEXT(".dat");
		_wstring finalPath = sPath + m_FieldList[i] + sFBX;
	
		_wstring sTag = TEXT("Prototype_Component_Level_Field_");
		_wstring finalTag = sTag + m_FieldList[i];

		std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
		std::string str = converter.to_bytes(finalPath);

		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_FIELD, finalTag,
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, str.c_str()))))
			return E_FAIL;
	}
#pragma endregion

#pragma region OBJECT
	/* For. Prototype_Component_Model_Lawn*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_FIELD, TEXT("Prototype_Component_Model_Lawn"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/ModelData/NonAnim/Obj/Lawn/Lawn.dat"))))
		return E_FAIL;

	/* For. Prototype_Component_Model_Grass*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_FIELD, TEXT("Prototype_Component_Model_Grass"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/ModelData/NonAnim/Obj/Grass/Grass.dat"))))
		return E_FAIL;

	/* For. Prototype_Component_Model_Tree*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_FIELD, TEXT("Prototype_Component_Model_Tree"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Obj/Tree/Tree.dat"))))
		return E_FAIL;

	/* For. Prototype_Component_Model_Weathercock*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_FIELD, TEXT("Prototype_Component_Model_Weathercock"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Obj/Weathercock/Weathercock.dat"))))
		return E_FAIL;

	/* For. Prototype_Component_Model_TailLockStatue*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_FIELD, TEXT("Prototype_Component_Model_TailLockStatue"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Obj/TailLockStatue/TailLockStatue.dat"))))
		return E_FAIL;

	/* For. Prototype_Component_Model_TailCaveShutter*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_FIELD, TEXT("Prototype_Component_Model_TailCaveShutter"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Obj/TailCaveShutter/TailCaveShutter.dat"))))
		return E_FAIL;

#pragma endregion

#pragma region MONSTER
	/* For. Prototype_Component_Model_SeaUrchin*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_FIELD, TEXT("Prototype_Component_Model_SeaUrchin"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Monster/SeaUrchin/SeaUrchin.dat"))))
		return E_FAIL;

	/* For. Prototype_Component_Model_Octorok*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_FIELD, TEXT("Prototype_Component_Model_Octorok"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Monster/Octorok/Octorok.dat", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_OctorokRock*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_FIELD, TEXT("Prototype_Component_Model_OctorokRock"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/ModelData/NonAnim/Obj/Monster/OctorokRock/OctorokRock.dat"))))
		return E_FAIL;
#pragma endregion
	return S_OK;
}

HRESULT CLoader::Ready_Models_For_Test()
{
	_matrix		PreTransformMatrix = XMMatrixIdentity();

	PreTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));
#pragma region MONSTER

	/* For. Prototype_Component_Model_Rola*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TEST, TEXT("Prototype_Component_Model_Rola"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Monster/Rola/Rola.dat"))))
		return E_FAIL;

	/* For. Prototype_Component_Model_Pawn*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TEST, TEXT("Prototype_Component_Model_Pawn"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Monster/Pawn/Pawn.dat", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_Spark*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TEST, TEXT("Prototype_Component_Model_Spark"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Monster/Spark/Spark.dat"))))
		return E_FAIL;

	/* For. Prototype_Component_Model_Togezo*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TEST, TEXT("Prototype_Component_Model_Togezo"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Monster/Togezo/Togezo.dat", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_Kuribo*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TEST, TEXT("Prototype_Component_Model_Kuribo"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Monster/Kuribo/Kuribo.dat"))))
		return E_FAIL;

	/* For. Prototype_Component_Model_Vegas0*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TEST, TEXT("Prototype_Component_Model_Vegas0"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Monster/Vegas/Vegas0.dat"))))
		return E_FAIL;

	/* For. Prototype_Component_Model_StalfosGreen*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TEST, TEXT("Prototype_Component_Model_StalfosGreen"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Monster/StalfosGreen/StalfosGreen.dat"))))
		return E_FAIL;


	/* For. Prototype_Component_Model_ZolRed*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TEST, TEXT("Prototype_Component_Model_ZolRed"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Monster/ZolRed/ZolRed.dat"))))
		return E_FAIL;

#pragma endregion

	return S_OK;
}

HRESULT CLoader::Ready_Models_For_Dungeon()
{
	_matrix		PreTransformMatrix = XMMatrixIdentity();

	PreTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));

#pragma region LAND

	for (int i = 0; i < 29; i++)
	{
		_wstring sPath = TEXT("../Bin/ModelData/NonAnim/Level/Dungeon/Lv01TailCave_");
		_wstring sFBX = TEXT(".dat");
		_wstring finalPath = sPath + m_DungeonList[i] + sFBX;

		_wstring sTag = TEXT("Prototype_Component_Level_Lv01TailCave_");
		_wstring finalTag = sTag + m_DungeonList[i];

		std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
		std::string str = converter.to_bytes(finalPath);

		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_DUNGEON, finalTag,
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, str.c_str()))))
			return E_FAIL;
	}
#pragma endregion

#pragma region MONSTER
	/* For. Prototype_Component_Model_DeguTail01*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_DUNGEON, TEXT("Prototype_Component_Model_DeguTail01"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Monster/DeguTail/DeguTail01.dat", PreTransformMatrix))))
		return E_FAIL;
#pragma endregion

#pragma region NONANIM_OBJ
	/* For. Prototype_Component_Model_DungeonCollapseTileLv01*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_DUNGEON, TEXT("Prototype_Component_Model_DungeonCollapseTileLv01"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/ModelData/NonAnim/Obj/DungeonCollapseTileLv01/DungeonCollapseTileLv01.dat"))))
		return E_FAIL;
#pragma endregion
	return S_OK;
}

HRESULT CLoader::Ready_Prototype_For_Field()
{
#pragma region OBJECT
	/* For. Prototype_GameObject_Land*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Lawn"),
		CLawn::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Grass*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Grass"),
		CGrass::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Tree*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Tree"),
		CTree::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Weathercock*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Weathercock"),
		CWeathercock::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_TailLockStatue*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TailLockStatue"),
		CTailLockStatue::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_TailCaveShutter*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TailCaveShutter"),
		CTailCaveShutter::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion

#pragma region MONSTER
	/* For. Prototype_GameObject_SeaUrchin*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SeaUrchin"),
		CSeaUrchin::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Octorok*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Octorok"),
		COctorok::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_OctorokRock*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_OctorokRock"),
		COctorokRock::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion
	return S_OK;

}

HRESULT CLoader::Ready_Prototype_For_Test()
{
#pragma region MONSTER
	/* For. Prototype_GameObject_Rola*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Rola"),
		CRola::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Rola*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Spark"),
		CSpark::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Pawn*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Pawn"),
		CPawn::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Togezo*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Togezo"),
		CTogezo::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Kuribo*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Kuribo"),
		CKuribo::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Vegas*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Vegas"),
		CVegas::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion
	return S_OK;
}

HRESULT CLoader::Ready_Prototype_For_Dungeon()
{
	/* For. Prototype_GameObject_DeguTail_00*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_DeguTail_00"),
		CDeguTail_00::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_CollapseTile*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_CollapseTile"),
		CCollapseTile::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	return S_OK;
}

CLoader * CLoader::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, LEVELID eNextLevelID)
{
	CLoader*		pInstance = new CLoader(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevelID)))
	{
		MSG_BOX(TEXT("Failed to Created : CLoader"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLoader::Free()
{
	__super::Free();

	WaitForSingleObject(m_hThread, INFINITE);

	DeleteObject(m_hThread);

	CloseHandle(m_hThread);

	DeleteCriticalSection(&m_CriticalSection);

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
