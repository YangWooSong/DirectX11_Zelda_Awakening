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

#include "Teleport.h"
#include "HousePot.h"
#include "Bed.h"
#include "Sword.h"
#include "Shield.h"
#include "Player_3D_Effects.h"
#include "Bomb.h"
#include "Marin.h"
#include "Tarin.h"

#include "Grass.h"
#include "Tree.h"
#include "Weathercock.h"
#include "TailLockStatue.h"
#include "TailCaveShutter.h"
#include "SeaUrchin.h"
#include "Octorok.h"
#include "OctorokRock.h"
#include "Detector.h"
#include "SinkingSword.h"
#include "Lupee.h"
#include "GrandmaUlrira.h"

#include "Kuribo.h"

#include "Particle_Explosion.h"
#include "Particle_Snow.h"
#include "Particle_Model.h"
#include "Particle_Image.h"

#include "DeguTail_00.h"
#include "DeguTail_01.h"
#include "DeguTail_04.h"
#include "Pawn.h"
#include "CollapseTile.h"
#include "CrackedWall.h"
#include "OwlStatue.h"
#include "SquareBlock.h"
#include "StoneHinoxRock.h"
#include "SpikeTile.h"
#include "TreasureBox.h"
#include "FootSwitch.h"
#include "SmallKey.h"
#include "ClosedPotDoor.h"
#include "PurpleQuartz.h"
#include "LockBlock.h"
#include "BladeTrap.h"
#include "Lockdoor.h"
#include "Vegas.h"
#include "Rola.h"
#include "Spark.h"
#include "RollingSpike.h"
#include "OnewayDoorReverse.h"
#include "BossDoor.h"
#include "ConchHorn.h"
#include "Togezo.h"

#include "Store_Item.h"
#include "ToolShopkeeper.h"

#include "InteractUI.h"
#include "ItemUI.h"
#include "DialogueUI.h"
#include "ChoiceBtn.h"
#include "MainUI.h"
#include "LupeeUI.h"
#include "HPUI.h"
#include "HeartUI.h"
#include "ItemIconUI.h"
#include "KeyUI.h"
#include "CampusUI.h"
#include "InvenUI.h"
#include "MapBackGround.h"
#include "MiniMap.h"
#include "MapUI.h"
#include "FadeInOut.h"

#include "Glow_Effect.h"
#include "Cross_Effect.h"
#include "Smoke_Effect.h"
#include "Flash_Effect.h"
#include "Ripple_Effect.h"
#include "MonsterDied_Effect.h"
#include "BombExplosion.h"
#include "3D_Effects.h"
#include "Player_ItemGet_Effect.h"
#include "Player_Charge_Slash.h"
#include "Halo_Effect.h"
#include "Ring_Effect.h"
#include "TailDungeonOpen.h"
#include "Spark_Effect.h"
#include "CubeHalfDome.h"
#include "Fire_Big_Effect.h"
#include "Fire_Plane_Effect.h"
#include "Fire_Small_Effect.h"
#include "Lightning_Effect.h"
#include "LockBlockEffect.h"
#include "Grad.h"
#include "QuestUI.h"
#include "ItemExplainUI.h"
#include "FootDust.h"
#include "Rola_Hand_Effect.h"
#include "Hand_Effect.h"


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
	case LEVEL_STORE:
		hr = Ready_Resources_For_Store();
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
	m_pGameInstance->LoadSoundFile("Sounds");

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

	Ready_Textures_For_static();

	lstrcpy(m_szLoadingText, TEXT("모델을(를) 로딩중입니다."));

	Ready_Models_For_Static();

	lstrcpy(m_szLoadingText, TEXT("사운드을(를) 로딩중입니다."));

	lstrcpy(m_szLoadingText, TEXT("셰이더을(를) 로딩중입니다."));

	/* For. Prototype_Component_Shader_VtxModelInstance */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxModelInstance"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxModelInstance.hlsl"), VTXMODELINSTANCE::Elements, VTXMODELINSTANCE::iNumElements))))
		return E_FAIL;

	/* For. Prototype_Component_Shader_VtxRectInstance */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxRectInstance"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxRectInstance.hlsl"), VTXRECTINSTANCE::Elements, VTXRECTINSTANCE::iNumElements))))
		return E_FAIL;


	/* For. Prototype_Component_Shader_VtxPointInstance */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPointInstance"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPointInstance.hlsl"), VTXPOINTINSTANCE::Elements, VTXPOINTINSTANCE::iNumElements))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("파티클을(를) 로딩중입니다."));

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Model_Instance"),
		CVIBuffer_Model_Instance::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_PointInstance"),
		CVIBuffer_Point_Instance::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("객체원형을(를) 로딩중입니다."));
	
	Ready_Prototype_For_MarinHouse();
	Ready_Prototype_For_Store();
	Ready_Prototype_For_Field();
	Ready_Prototype_For_Dungeon();

	/* For. Prototype_GameObject_NavDataObj*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_NavDataObj"),
		CNavDataObj::Create(m_pDevice, m_pContext))))
		return E_FAIL;

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

	lstrcpy(m_szLoadingText, TEXT("사운드을(를) 로딩중입니다."));

	lstrcpy(m_szLoadingText, TEXT("파티클을(를) 로딩중입니다."));


	lstrcpy(m_szLoadingText, TEXT("객체원형을(를) 로딩중입니다."));

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

	CVIBuffer_Instancing::INSTANCE_DESC			ParticleDesc{};



	lstrcpy(m_szLoadingText, TEXT("셰이더을(를) 로딩중입니다."));

	/* For. Prototype_Component_VIBuffer_Particle_Explosion */
	ZeroMemory(&ParticleDesc, sizeof ParticleDesc);

	ParticleDesc.iNumInstance = 3000;
	ParticleDesc.vCenter = _float3(64.f, 20.f, 64.f);
	ParticleDesc.vRange = _float3(128.f, 1.f, 128.f);
	ParticleDesc.vSize = _float2(0.1f, 0.3f);
	ParticleDesc.vPivot = _float3(0.f, 0.f, 0.f);
	ParticleDesc.vSpeed = _float2(1.f, 3.f);
	ParticleDesc.vLifeTime = _float2(4.f, 8.f);
	ParticleDesc.isLoop = true;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Particle_Explosion"),
		CVIBuffer_Rect_Instance::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	ParticleDesc.iNumInstance = 50;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Model_Instance"),
		CVIBuffer_Model_Instance::Create(m_pDevice, m_pContext))))
		return E_FAIL;


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

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));



	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Ready_Resources_For_Store()
{
	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다."));

	lstrcpy(m_szLoadingText, TEXT("모델을(를) 로딩중입니다."));

	Ready_Models_For_Store();

	lstrcpy(m_szLoadingText, TEXT("셰이더을(를) 로딩중입니다."));

	lstrcpy(m_szLoadingText, TEXT("사운드을(를) 로딩중입니다."));

	lstrcpy(m_szLoadingText, TEXT("네비게이션을(를) 로딩중입니다."));

	/* For.Prototype_Component_Navigation */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STORE, TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/Nav_Data/Store_Nav_Data.dat")))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("객체원형을(를) 로딩중입니다."));

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Ready_Textures_For_static()
{
	/* For. Prototype_Component_Texture_House_Background */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_House_Background"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Zelda/room_back_01.dds"), 1))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_OpenBox */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_OpenBox"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Zelda/Button/InteractBT_%d.dds"), 4))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_ItemIcon */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_ItemIcon"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Zelda/Icon/TreasureBoxIcon_%d.dds"), 10))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Particle */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Particle"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Snow/Snow.png"), 1))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Dialogue */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Dialogue"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Zelda/Dialogue_%d.dds"), 3))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_ChoiceBtn */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_ChoiceBtn"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Zelda/Button/ChoiceBT_%d.dds"), 2))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_ChoiceBtn_Selected */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_ChoiceBtn_Selected"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Zelda/Button/ChoiceBT_Selected_%d.dds"), 2))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_LupeeUI*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_LupeeUI"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Zelda/UI/Rupee_00.dds"), 1))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_HeartUI*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_HeartUI"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Zelda/UI/HP/Hp_%d.dds"), 2))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_ItemIconUI*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_ItemIconUI"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Zelda/UI/ItemIcon/Item_%d.dds"), 5))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Map_Background*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Map_Background"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Zelda/Map/Map_Background.png"), 1))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_DgnMapGrid*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_DgnMapGrid"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Zelda/Map/DgnMapGrid.png"), 1))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_MapReveal*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MapReveal"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Zelda/Map/Reveal/Map_Reveal_%d.dds"), 27))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_MapMask*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MapMask"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Zelda/Map/Mask/Map_Mask_%d.dds"), 27))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Effect_Glow*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Glow"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Zelda/Effect/glow_00.dds"), 1))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Effect_point_Glow*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_point_Glow"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Zelda/Effect/point_Glow.dds"), 1))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Effect_Cross_Color*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Cross_Color"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Zelda/Effect/glow_01.dds"), 1))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Effect_Smoke*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Smoke"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Zelda/Effect/smoke_00.dds"), 1))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Effect_Dissolve*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Dissolve"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Zelda/Effect/dissolve_02.dds"), 1))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_ChoiceBtn_Selected */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Cross_Mini"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Zelda/Effect/cross_00.dds"), 1))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Effect_Flash*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Flash"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Zelda/Effect/flash_%d.dds"), 4))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Effect_Ripple*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Ripple"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Zelda/Effect/ripple_%d.dds"), 2))))
		return E_FAIL;
	
	/* For. Prototype_Component_Texture_Effect_fire_small_01*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_fire_small_01"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Zelda/Effect/fire_small_01.png"), 1))))
		return E_FAIL;
	
	/* For. Prototype_Component_Texture_Black*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Black"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Zelda/Fade_%d.dds"), 2))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Grad*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Grad"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Zelda/Effect/grad_02.dds"), 1))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_QuestGrad*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_QuestGrad"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Zelda/Effect/grad_00.png"), 1))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_smoke_pattern_02*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_smoke_pattern_02"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Zelda/Effect/smoke_pattern_02.dds"), 1))))
		return E_FAIL;

	return S_OK;
}


HRESULT CLoader::Ready_Models_For_Static()
{
	_matrix		PreTransformMatrix = XMMatrixIdentity();
	PreTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));

	/* For. Prototype_Component_Model_Link*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Link"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Link/Link.dat", PreTransformMatrix))))
		return E_FAIL;
#pragma region AnimObject
	/* For. Prototype_Component_Model_Bomb*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Bomb"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Obj/Bomb/Bomb.dat"))))
		return E_FAIL;
#pragma endregion

#pragma region NonAnimObj
	/* For. Prototype_Component_Model_Obj_HousePot*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Obj_HousePot"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/ModelData/NonAnim/Obj/HousePot/HousePot.dat"))))
		return E_FAIL;

#pragma endregion

#pragma region PARTICLE
	/* For. Prototype_Component_Model_Purplequartz_Particle*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Purplequartz_Particle"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/ModelData/NonAnim/Particle/purplequartz_particle/purplequartz_particle.dat"))))
		return E_FAIL;

	/* For. Prototype_Component_Model_HousePot_Particle*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_HousePot_Particle"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/ModelData/NonAnim/Particle/pot_fragment/pot_fragment.dat"))))
		return E_FAIL;

	/* For. Prototype_Component_Model_OctoRockRock_Particle*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_OctoRockRock_Particle"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/ModelData/NonAnim/Particle/OctorockRock_Particle/stone_00.dat"))))
		return E_FAIL;

	/* For. Prototype_Component_Model_Grass_Particle*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Grass_Particle"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/ModelData/NonAnim/Particle/leaf/leaf.dat"))))
		return E_FAIL;

	/* For. Prototype_Component_Model_cut_lawn_Particle*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_cut_lawn_Particle"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/ModelData/NonAnim/Particle/cut_lawn/cut_lawn.dat"))))
		return E_FAIL;

#pragma endregion

#pragma region EFFECTS

	/* For. Prototype_Component_Model_hitflash_00*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_hitflash_00"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/ModelData/NonAnim/Effect/hitflash_00/hitflash_00.dat", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_halo_00*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_halo_00"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/ModelData/NonAnim/Effect/halo_00/halo_00.dat"))))
		return E_FAIL;

	/* For. Prototype_Component_Model_ring_00*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_ring_00"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/ModelData/NonAnim/Effect/ring_00/ring_00.dat"))))
		return E_FAIL;

	/* For. Prototype_Component_Model_cubehalfdome_00*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_cubehalfdome_00"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/ModelData/NonAnim/Effect/cubehalfdome_00/cubehalfdome_00.dat"))))
		return E_FAIL;

	/* For. Prototype_Component_Model_slash*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_slash"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/ModelData/NonAnim/Effect/slash/slash.dat", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_slash*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_thunder_02"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/ModelData/NonAnim/Effect/thunder_02/thunder_02.dat"))))
		return E_FAIL;

	/* For. Prototype_Component_Model_fireplane_00*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_fireplane_00"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/ModelData/NonAnim/Effect/fireplane_00/fireplane_00.dat"))))
		return E_FAIL;

	/* For. Prototype_Component_Model_rollcut_00*/ 
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_rollcut_00"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/ModelData/NonAnim/Effect/rollcut_00/rollcut_00.dat", PreTransformMatrix))))
		return E_FAIL;
	
	/* For. Prototype_Component_Model_rollcut_01*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_rollcut_01"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/ModelData/NonAnim/Effect/rollcut_01/rollcut_01.dat", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_LupeeUI*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Lightning"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Zelda/Effect/T_EFF_Lightning_05_M.dds"), 1))))
		return E_FAIL;

#pragma endregion
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

	/* For. Prototype_Component_Model_MarinHouse*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MARINHOUSE, TEXT("Prototype_Component_Level_MarinHouse"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/ModelData/NonAnim/Level/MarinHouse/MarinHouse.dat"))))
		return E_FAIL;

	/* For. Prototype_Component_Model_Obj_Bed*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MARINHOUSE, TEXT("Prototype_Component_Model_Obj_Bed"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Obj/Bed/Bed.dat"))))
		return E_FAIL;

	/* For. Prototype_Component_Model_NPC_Marin*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MARINHOUSE, TEXT("Prototype_Component_Model_NPC_Marin"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/NPC/Marin/Marin.dat", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_NPC_Tarin*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MARINHOUSE, TEXT("Prototype_Component_Model_NPC_Tarin"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/NPC/Tarin/Tarin.dat"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Ready_Models_For_Field()
{
	_matrix		PreTransformMatrix = XMMatrixIdentity();

	PreTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));

	/* For. Prototype_Component_Model_NPC_GrandmaUlrira*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_FIELD, TEXT("Prototype_Component_Model_NPC_GrandmaUlrira"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/NPC/GrandmaUlrira/GrandmaUlrira.dat", PreTransformMatrix))))
		return E_FAIL;

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

	/* For. Prototype_Component_Model_SinkingSword*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_FIELD, TEXT("Prototype_Component_Model_SinkingSword"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/ModelData/NonAnim/Obj/Objects/SinkingSword/SinkingSword.dat", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_Lupee*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_FIELD, TEXT("Prototype_Component_Model_Lupee"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/ModelData/NonAnim/Obj/Objects/RupeeGreen/RupeeGreen.dat", PreTransformMatrix))))
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

	/* For. Prototype_Component_Model_Kuribo*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TEST, TEXT("Prototype_Component_Model_Kuribo"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Monster/Kuribo/Kuribo.dat"))))
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

	/* For. Prototype_Component_Model_DeguTail02*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_DUNGEON, TEXT("Prototype_Component_Model_DeguTail02"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Monster/DeguTail/DeguTail02.dat", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_DeguTail03*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_DUNGEON, TEXT("Prototype_Component_Model_DeguTail03"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Monster/DeguTail/DeguTail03.dat", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_Pawn*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_DUNGEON, TEXT("Prototype_Component_Model_Pawn"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Monster/Pawn/Pawn.dat", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_Spark*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_DUNGEON, TEXT("Prototype_Component_Model_Spark"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Monster/Spark/Spark.dat"))))
		return E_FAIL;

	/* For. Prototype_Component_Model_Vegas0*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_DUNGEON, TEXT("Prototype_Component_Model_Vegas0"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Monster/Vegas/Vegas0.dat"))))
		return E_FAIL;

	/* For. Prototype_Component_Model_Rola*/

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_DUNGEON, TEXT("Prototype_Component_Model_Rola"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Monster/Rola/Rola.dat", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_Togezo*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_DUNGEON, TEXT("Prototype_Component_Model_Togezo"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Monster/Togezo/Togezo.dat", PreTransformMatrix))))
		return E_FAIL;

#pragma endregion

#pragma region NONANIM_OBJ
	/* For. Prototype_Component_Model_DungeonCollapseTileLv01*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_DUNGEON, TEXT("Prototype_Component_Model_DungeonCollapseTileLv01"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/ModelData/NonAnim/Obj/DungeonCollapseTileLv01/DungeonCollapseTileLv01.dat"))))
		return E_FAIL;

	/* For. Prototype_Component_Model_DungeonCrackedWallLv01*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_DUNGEON, TEXT("Prototype_Component_Model_DungeonCrackedWallLv01"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/ModelData/NonAnim/Obj/DungeonCrackedWallLv01/DungeonCrackedWallLv01.dat"))))
		return E_FAIL;

	/* For. Prototype_Component_Model_DungeonOwlStatue*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_DUNGEON, TEXT("Prototype_Component_Model_DungeonOwlStatue"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/ModelData/NonAnim/Obj/DungeonOwlStatue/DungeonOwlStatue.dat"))))
		return E_FAIL;

	/* For. Prototype_Component_Model_SquareBlock*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_DUNGEON, TEXT("Prototype_Component_Model_SquareBlock"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/ModelData/NonAnim/Obj/SquareBlock/SquareBlock.dat"))))
		return E_FAIL;

	/* For. Prototype_Component_Model_StoneHinoxRock*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_DUNGEON, TEXT("Prototype_Component_Model_StoneHinoxRock"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/ModelData/NonAnim/Obj/StoneHinoxRock/StoneHinoxRock.dat"))))
		return E_FAIL;

	/* For. Prototype_Component_Model_Obj_HousePot*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_DUNGEON, TEXT("Prototype_Component_Model_Obj_HousePot"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/ModelData/NonAnim/Obj/HousePot/HousePot.dat"))))
		return E_FAIL;

	/* For. Prototype_Component_Model_Obj_SmallKey*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_DUNGEON, TEXT("Prototype_Component_Model_SmallKey"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/ModelData/NonAnim/Obj/Objects/SmallKey/SmallKey.dat"))))
		return E_FAIL;
	
	/* For. Prototype_Component_Model_Obj_PurpleQuartz*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_DUNGEON, TEXT("Prototype_Component_Model_PurpleQuartz"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/ModelData/NonAnim/Obj/PurpleQuartz/PurpleQuartz.dat"))))
		return E_FAIL;

	/* For. Prototype_Component_Model_Obj_LockBlock*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_DUNGEON, TEXT("Prototype_Component_Model_LockBlock"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/ModelData/NonAnim/Obj/LockBlock/LockBlock.dat"))))
		return E_FAIL;

	/* For. Prototype_Component_Model_ConchHorn*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_DUNGEON, TEXT("Prototype_Component_Model_ConchHorn"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/ModelData/NonAnim/Obj/Objects/ConchHorn/ConchHorn.dat"))))
		return E_FAIL;
#pragma endregion

#pragma region ANIM_OBJ
	/* For. Prototype_Component_Model_SpikeTile*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_DUNGEON, TEXT("Prototype_Component_Model_SpikeTile"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Obj/SpikeTile/SpikeTile.dat"))))
		return E_FAIL;

	/* For. Prototype_Component_Model_TreasureBox*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_DUNGEON, TEXT("Prototype_Component_Model_TreasureBox"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Obj/TreasureBox/TreasureBox.dat"))))
		return E_FAIL;

	/* For. Prototype_Component_Model_FootSwitch*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_DUNGEON, TEXT("Prototype_Component_Model_FootSwitch"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Obj/FootSwitch/FootSwitch.dat"))))
		return E_FAIL;

	/* For. Prototype_Component_Model_ClosedPotDoor*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_DUNGEON, TEXT("Prototype_Component_Model_ClosedPotDoor"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Obj/ClosedPotDoor/ClosedPotDoor.dat"))))
		return E_FAIL;

	/* For. Prototype_Component_Model_BladeTrap*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_DUNGEON, TEXT("Prototype_Component_Model_BladeTrap"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Obj/BladeTrap/BladeTrap.dat"))))
		return E_FAIL;

	/* For. Prototype_Component_Model_LockDoor*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_DUNGEON, TEXT("Prototype_Component_Model_LockDoor"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Obj/LockDoor/LockDoor.dat"))))
		return E_FAIL;

	/* For. Prototype_Component_Model_OnewayDoorReverse*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_DUNGEON, TEXT("Prototype_Component_Model_OnewayDoorReverse"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Obj/OnewayDoorReverse/OnewayDoorReverse.dat"))))
		return E_FAIL;

	/* For. Prototype_Component_Model_RollingSpike*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_DUNGEON, TEXT("Prototype_Component_Model_RollingSpike"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Obj/RollingSpike_Anim/RollingSpike_Anim.dat"))))
		return E_FAIL;

	/* For. Prototype_Component_Model_BossDoor*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_DUNGEON, TEXT("Prototype_Component_Model_BossDoor"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Obj/BossDoor/BossDoor.dat"))))
		return E_FAIL;

#pragma endregion
	return S_OK;
}

HRESULT CLoader::Ready_Models_For_Store()
{
	_matrix		PreTransformMatrix = XMMatrixIdentity();

	PreTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));

	/* For. Prototype_Component_Level_Shop*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STORE, TEXT("Prototype_Component_Level_Shop"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/ModelData/NonAnim/Level/Shop/Shop_01A.dat"))))
		return E_FAIL;

	/* For. Prototype_Component_Model_HeartContainer*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STORE, TEXT("Prototype_Component_Model_HeartContainer"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/ModelData/NonAnim/Obj/Objects/HeartContainer/HeartContainer.dat"))))
		return E_FAIL;

	/* For. Prototype_Component_Model_SoldOutPlate*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STORE, TEXT("Prototype_Component_Model_SoldOutPlate"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/ModelData/NonAnim/Obj/SoldOutPlate/SoldOutPlate.dat"))))
		return E_FAIL;

	/* For. Prototype_Component_Model_NPC_ToolShopkeeper*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STORE, TEXT("Prototype_Component_Model_NPC_ToolShopkeeper"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/NPC/ToolShopkeeper/ToolShopkeeper.dat", PreTransformMatrix))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Ready_Prototype_For_MarinHouse()
{
#pragma region Camera
	/* For. Prototype_GameObject_FreeCamera */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FreeCamera"),
		CFreeCamera::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_PlayerCamera */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_PlayerCamera"),
		CPlayerCamera::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion

	/* For. Prototype_GameObject_Marin*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Marin"),
		CMarin::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Tarin*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Tarin"),
		CTarin::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma region Object
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

	/* For. Prototype_GameObject_Player_3D_Effects*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player_3D_Effects"),
		CPlayer_3D_Effects::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Shield*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Shield"),
		CShield::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Bomb*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Bomb"),
		CBomb::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Teleport*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Teleport"),
		CTeleport::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma endregion

#pragma region UI
	/* For. Prototype_GameObject_InteractUI*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_InteractUI"),
		CInteractUI::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_ItemtUI*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ItemtUI"),
		CItemUI::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_DialogueUI*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_DialogueUI"),
		CDialogueUI::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_ChoiceBtn*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ChoiceBtn"),
		CChoiceBtn::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Particle_Expolosion */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Particle_Expolosion"),
		CParticle_Explosion::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_OctorokRock*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Detector"),
		CDetector::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Particle_Model*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Particle_Model"),
		CParticle_Model::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Particle_Image*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Particle_Image"),
		CParticle_Image::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	/* For. Prototype_GameObject_MainUI*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MainUI"),
		CMainUI::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_LupeeUI*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_LupeeUI"),
		CLupeeUI::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_HPUI*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_HPUI"),
		CHPUI::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_HeartUI*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_HeartUI"),
		CHeartUI::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	/* For. Prototype_GameObject_ItemIconUI*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ItemIconUI"),
		CItemIconUI::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_KeyUI*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_KeyUI"),
		CKeyUI::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	/* For. Prototype_GameObject_CampusUI*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_CampusUI"),
		CCampusUI::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_InvenUI*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_InvenUI"),
		CInvenUI::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	/* For. Prototype_GameObject_MiniMap*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MiniMap"),
		CMiniMap::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_MapBackGround*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MapBackGround"),
		CMapBackGround::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_MapUI*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MapUI"),
		CMapUI::Create(m_pDevice, m_pContext))))
		return E_FAIL;	
	
	/* For. Prototype_GameObject_FadeInOut*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FadeInOut"),
		CFadeInOut::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion

#pragma region Effect
	/* For. Prototype_GameObject_Glow_Effect*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Glow_Effect"),
		CGlow_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For. Prototype_GameObject_Cross_Effect*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Cross_Effect"),
		CCross_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For. Prototype_GameObject_Smoke_Effect*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Smoke_Effect"),
		CSmoke_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For. Prototype_GameObject_Flash_Effect*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Flash_Effect"),
		CFlash_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For. Prototype_GameObject_MonsterDied_Effect*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MonsterDied_Effect"),
		CMonsterDied_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	/* For. Prototype_GameObject_BombExplosion*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BombExplosion"),
		CBombExplosion::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_3D_Effects*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_3D_Effects"),
		C3D_Effects::Create(m_pDevice, m_pContext))))
		return E_FAIL;	
	
	/* For. Prototype_GameObject_Player_ItemGet_Effect*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player_ItemGet_Effect"),
		CPlayer_ItemGet::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Player_Charge_Slash*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player_Charge_Slash"),
		CPlayer_Charge_Slash::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	/* For. Prototype_GameObject_Ripple_Effect*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Ripple_Effect"),
		CRipple_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For. Prototype_GameObject_Halo_Effect*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Halo_Effect"),
		CHalo_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For. Prototype_GameObject_Ring_Effect*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Ring_Effect"),
		CRing_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Spark_Effect*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Spark_Effect"),
		CSpark_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	/* For. Prototype_GameObject_CubeHalfDome_Effect*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_CubeHalfDome_Effect"),
		CCubeHalfDome::Create(m_pDevice, m_pContext))))
		return E_FAIL;	
	
	/* For. Prototype_GameObject_Fire_Plane_Effect*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Fire_Plane_Effect"),
		CFire_Plane_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;	
	
	/* For. Prototype_GameObject_Fire_Big_Effect*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Fire_Big_Effect"),
		CFire_Big_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	/* For. Prototype_GameObject_Fire_Small_Effect*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Fire_Small_Effect"),
		CFire_Small_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	/* For. Prototype_GameObject_Grad*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Grad"),
		CGrad::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_QuestUI*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_QuestUI"),
		CQuestUI::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_FootDust*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FootDust"),
		CFootDust::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion
	return S_OK;
}

HRESULT CLoader::Ready_Prototype_For_Field()
{
	/* For. Prototype_GameObject_GrandmaUlrira*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_GrandmaUlrira"),
		CGrandmaUlrira::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma region OBJECT
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
	
	/* For. Prototype_GameObject_SinkingSword*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SinkingSword"),
		CSinkingSword::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Lupee*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Lupee"),
		CLupee::Create(m_pDevice, m_pContext))))
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
	/* For. Prototype_GameObject_TailDungeonOpen*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TailDungeonOpen"),
		CTailDungeonOpen::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	return S_OK;

}

HRESULT CLoader::Ready_Prototype_For_Test()
{
#pragma region MONSTER
	
	/* For. Prototype_GameObject_Kuribo*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Kuribo"),
		CKuribo::Create(m_pDevice, m_pContext))))
		return E_FAIL;


#pragma endregion

	/* For. Prototype_GameObject_Particle_Snow */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Particle_Snow"),
		CParticle_Snow::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	return S_OK;
}

HRESULT CLoader::Ready_Prototype_For_Dungeon()
{
#pragma region UI
	
#pragma endregion

#pragma region MONSTER
	/* For. Prototype_GameObject_DeguTail_00*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_DeguTail_00"),
		CDeguTail_00::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_DeguTail_01*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_DeguTail_01"),
		CDeguTail_01::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_DeguTail_04*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_DeguTail_04"),
		CDeguTail_04::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Pawn*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Pawn"),
		CPawn::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Spark*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Spark"),
		CSpark::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Vegas*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Vegas"),
		CVegas::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Rola*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Rola"),
		CRola::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Togezo*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Togezo"),
		CTogezo::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma endregion

#pragma region NONANIM_OBJ
	/* For. Prototype_GameObject_CollapseTile*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_CollapseTile"),
		CCollapseTile::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_CrackedWall*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_CrackedWall"),
		CCrackedWall::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_OwlStatue*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_OwlStatue"),
		COwlStatue::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_SquareBlock*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SquareBlock"),
		CSquareBlock::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_StoneHinoxRock*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_StoneHinoxRock"),
		CStoneHinoxRock::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_SmallKey*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SmallKey"),
		CSmallKey::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_PurpleQuartz*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_PurpleQuartz"),
		CPurpleQuartz::Create(m_pDevice, m_pContext))))
		return E_FAIL;	
	
	/* For. Prototype_GameObject_LockBlock*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_LockBlock"),
		CLockBlock::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_ConchHorn*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ConchHorn"),
		CConchHorn::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion

#pragma region ANIM_OBJ
	/* For. Prototype_GameObject_SpikeTile*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SpikeTile"),
		CSpikeTile::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_TreasureBox*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TreasureBox"),
		CTreasureBox::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_FootSwitch*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FootSwitch"),
		CFootSwitch::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_ClosedPotDoor*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ClosedPotDoor"),
		CClosedPotDoor::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_BladeTrap*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BladeTrap"),
		CBladeTrap::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_LockDoor*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_LockDoor"),
		CLockDoor::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_OnewayDoorReverse*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_OnewayDoorReverse"),
		COnewayDoorReverse::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_RollingSpike*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_RollingSpike"),
		CRollingSpike::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_BossDoor*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BossDoor"),
		CBossDoor::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma endregion

#pragma region EFFECT
	/* For. Prototype_GameObject_Lightning_Effect*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Lightning_Effect"),
		CLightning_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	/* For. Prototype_GameObject_LockBlockEffect*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_LockBlockEffect"),
		CLockBlockEffect::Create(m_pDevice, m_pContext))))
		return E_FAIL;	

	/* For. Prototype_GameObject_Rola_Hand_Effect*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Rola_Hand_Effect"),
		CRola_Hand_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;	
	
	/* For. Prototype_GameObject_Hand_Effect*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Hand_Effect"),
		CHand_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	
#pragma endregion
	return S_OK;
}

HRESULT CLoader::Ready_Prototype_For_Store()
{
	/* For. Prototype_GameObject_Store_Item*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Store_Item"),
		CStore_Item::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_ToolShopkeeper*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ToolShopkeeper"),
		CToolShopkeeper::Create(m_pDevice, m_pContext))))
		return E_FAIL;	
	
	/* For. Prototype_GameObject_ItemExplainUI*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ItemExplainUI"),
		CItemExplainUI::Create(m_pDevice, m_pContext))))
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
