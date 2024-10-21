#include "stdafx.h"
#include "..\Public\Level_MarinHouse.h"
#include "Level_Loading.h"
#include "GameInstance.h"
#include "FreeCamera.h"
#include "PlayerCamera.h"
#include "AnimModel.h"
#include "Link.h"
#include "NavDataObj.h"
#include "BackGround.h"
#include "MainUI.h"
#include "Teleport.h"
#include "2DEffects.h"

#include <fstream>
CLevel_MarinHouse::CLevel_MarinHouse(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_MarinHouse::Initialize()
{
	if (FAILED(Ready_Lights()))
		return E_FAIL;
	if (FAILED(Ready_Layer_BackGround()))
		return E_FAIL;
	if (FAILED(Ready_LandObjects()))
		return E_FAIL;
	if (FAILED(Ready_Layer_Camera()))
		return E_FAIL;
	
	if (FAILED(Ready_Layer_Effect()))
		return E_FAIL;

	Read();

	m_pGameInstance->Play_BGM(TEXT("0_House_Inside.wav"), 0.8f);
	m_pTeleportObj = static_cast<CTeleport*>(m_pGameInstance->Find_Object(LEVEL_MARINHOUSE, TEXT("Layer_Teleport"), 0));
	
	m_pMainUI = static_cast<CMainUI*>(m_pGameInstance->Find_Object(LEVEL_MARINHOUSE, TEXT("Layer_MainUI"), 0));
	m_pMainUI->Active_FadeIn();

	m_pGameInstance->Set_ShadowLightPos(CPipeLine::D3DTS_SHADOW_LIGHT, XMVectorSet(5.f, 40.f,-20.f,1.f));
	return S_OK;
}

void CLevel_MarinHouse::Update(_float fTimeDelta)
{
	if (m_pTeleportObj->Get_Change_Level() && m_bFadeOut == false)
	{
		m_bFadeOut = true;
		m_pMainUI->Active_FadeOut();
	}

	if (GetKeyState(VK_RETURN) & 0x8000)
	{
		m_pGameInstance->DeletePlayer();
		m_pGameInstance->DeleteActors();
		m_pGameInstance->Stop_BGM();
		m_pGameInstance->Reset_Lights();
		if (FAILED(m_pGameInstance->Change_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_STORE))))
			return;
		return;
	}

	if (m_pMainUI->Get_isFinishFadeOut())
	{
		m_pGameInstance->DeletePlayer();
		m_pGameInstance->DeleteActors();
		m_pGameInstance->Stop_BGM();
		m_pGameInstance->Reset_Lights();
		m_pTeleportObj->Set_Change_Level(false);
		if (FAILED(m_pGameInstance->Change_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, (LEVELID)m_pTeleportObj->Get_NextLevel()))))
			return;
	}
}

HRESULT CLevel_MarinHouse::Render()
{
	SetWindowText(g_hWnd, TEXT("게임플레이레벨입니다."));
	return S_OK;
}

HRESULT CLevel_MarinHouse::Ready_Lights()
{
	/* 게임플레이 레벨에 필요한 광원을 준비한다. */
	LIGHT_DESC			LightDesc{};

	ZeroMemory(&LightDesc, sizeof LightDesc);
	LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
	LightDesc.vDiffuse = _float4(0.6f, 0.6f, 0.6f, 1.f);
	LightDesc.vAmbient = _float4(0.5f, 0.5f, 0.5f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
		return E_FAIL;

	ZeroMemory(&LightDesc, sizeof LightDesc);
	LightDesc.eType = LIGHT_DESC::TYPE_POINT;
	LightDesc.vPosition = _float4(0.f, 0.f, -2.f, 1.f);
	LightDesc.fRange = 5.f;
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.5f, 0.5f, 0.5f, 1.f);
	LightDesc.vSpecular = LightDesc.vDiffuse;

	if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_MarinHouse::Ready_Layer_Camera() 
{
	/*
	CFreeCamera::CAMERA_FREE_DESC		Desc{};

	Desc.fSensor = 0.2f;
	Desc.vEye = _float4(0.f, 10.f, -10.f, 1.f);
	Desc.vAt = _float4(0.f, 10.f, 0.f, 1.f);
	Desc.fFovy = XMConvertToRadians(60.0f);
	Desc.fAspect = (_float)g_iWinSizeX / (_float)g_iWinSizeY;
	Desc.fNear = 0.1f;
	Desc.fFar = 1000.f;
	Desc.fSpeedPerSec = 30.f;
	Desc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_FIELD, TEXT("Layer_Camera"), TEXT("Prototype_GameObject_FreeCamera"), &Desc)))
		return E_FAIL;*/

	CPlayerCamera::CAMERA_PLAYER_DESC	Desc{};

	Desc.vEye = _float4(0.f, 15.f, -14.f, 1.f);
	Desc.vAt = _float4(0.f, 0.f, -8.f, 1.f);
	Desc.fFovy = XMConvertToRadians(30.f);
	Desc.fAspect = (_float)g_iWinSizeX / (_float)g_iWinSizeY;
	Desc.fNear = 0.1f;
	Desc.fFar = 1000.f;
	Desc.fSpeedPerSec = 30.f;
	Desc.fRotationPerSec = XMConvertToRadians(90.0f);
	Desc.fOffest = _float3(0.f, 15.f, -14.f);
	Desc.fSpeed = 4.f;
	Desc.pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Find_Player(LEVEL_MARINHOUSE));
	Desc.bFollowPlayer = false;

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_MARINHOUSE, TEXT("Layer_Camera"), TEXT("Prototype_GameObject_PlayerCamera"), &Desc)))
		return E_FAIL;

	return S_OK;
}


HRESULT CLevel_MarinHouse::Ready_Layer_BackGround()
{
	CBackGround::BACKGROUND_DESC Desc{ };

	Desc.eType = CBackGround::HOUSE_BACKGROUND;
	Desc.fSizeX = g_iWinSizeX * 1.1f;
	Desc.fSizeY = g_iWinSizeY * 1.5f;
	Desc.fX = g_iWinSizeX  / 2;
	Desc.fY = g_iWinSizeY * 0.3f;

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_MARINHOUSE, TEXT("Layer_BackGround"),
		TEXT("Prototype_GameObject_BackGround"), &Desc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CLevel_MarinHouse::Ready_Layer_Effect()
{
	return S_OK;
}

HRESULT CLevel_MarinHouse::Ready_LandObjects()
{

	CPlayer::PLAYER_DESC PlayerDesc{};
	PlayerDesc.vPosition = _float3(-0.3f, 0.f, 1.f);
	PlayerDesc.LevelIndex = LEVEL_MARINHOUSE;
	PlayerDesc.iStartCellNum = 0;
	PlayerDesc.vRotation = _float3(0.f, 90.f, 0.f);
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_MARINHOUSE, TEXT("Layer_Player"), TEXT("Prototype_GameObject_Player_Link"), &PlayerDesc)))
		return E_FAIL;

	CGameObject::GAMEOBJECT_DESC pDesc = { };
	pDesc.eType = static_cast<CGameObject::OBJ_TYPE>(CGameObject::ANIM_NPC);
	pDesc.vPosition = _float3(2.f, 0.f, 1.f);
	pDesc.vScale = _float3(1.f, 1.f, 1.f);
	pDesc.vRotation = _float3(0.f, 0.f, 0.f);
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_MARINHOUSE, TEXT("Layer_NPC"), TEXT("Prototype_GameObject_Marin"), &pDesc)))
		return E_FAIL;

	pDesc.vPosition = _float3(-4.5f, 0.f, -1.f);
	pDesc.vScale = _float3(1.f, 1.f, 1.f);
	pDesc.vRotation = _float3(0.f, 45.f, 0.f);
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_MARINHOUSE, TEXT("Layer_NPC"), TEXT("Prototype_GameObject_Tarin"), &pDesc)))
		return E_FAIL;

	CNavDataObj::NAVOBJ_DESC NavDes{};
	NavDes.iLevelNum = LEVEL_MARINHOUSE; 
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_MARINHOUSE, TEXT("Layer_NavDataObj"), TEXT("Prototype_GameObject_NavDataObj"), &NavDes)))
		return E_FAIL;

	CMainUI::MAINUI_DESC MainDesc{};
	MainDesc.iLevelIndex = LEVEL_MARINHOUSE;
	MainDesc.fSizeX = g_iWinSizeX;
	MainDesc.fSizeY = g_iWinSizeY;
	MainDesc.fX = g_iWinSizeX / 2;
	MainDesc.fY = g_iWinSizeY / 2;

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_MARINHOUSE, TEXT("Layer_MainUI"),
		TEXT("Prototype_GameObject_MainUI"), &MainDesc)))
		return E_FAIL;

	CTeleport::TELEPORT_DESC TeleportDesc{};
	TeleportDesc.iNextLevelIndex = LEVEL_FIELD;
	TeleportDesc.vPosition = {0.f, 0.5f, -4.2f};
	TeleportDesc.vScale = {1.5f,0.5f,0.5f};

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_MARINHOUSE, TEXT("Layer_Teleport"),
		TEXT("Prototype_GameObject_Teleport"), &TeleportDesc)))
		return E_FAIL;

	C2DEffects::EFFECT_DESC effectDesc = {};
	effectDesc.fColor = { 1.1f,1.f,1.f,0.2f };
	effectDesc.iLevelIndex = LEVEL_MARINHOUSE;
	effectDesc.iEffectType = { WINDOW_LIGHT };
	effectDesc.vPosition = { 1.95f, 1.8f, 5.f };
	effectDesc.vScale = { 0.8f,1.2f,0.1f };
	effectDesc.vRotation = { 0.f,0.f,-4.f };
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_MARINHOUSE, TEXT("Layer_Effect"),
		TEXT("Prototype_GameObject_Grad"), &effectDesc)))
		return E_FAIL;

	effectDesc.vPosition = { -1.95f, 1.85f, 5.f };
	effectDesc.vScale = { 0.8f,0.9f,0.1f };
	effectDesc.vRotation = { 8.f,0.f,0.f };
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_MARINHOUSE, TEXT("Layer_Effect"),
		TEXT("Prototype_GameObject_Grad"), &effectDesc)))
		return E_FAIL;

	effectDesc.fColor = { 1.1f,1.f,1.f,0.25f };
	effectDesc.vPosition = { -5.84f, 2.f, 0.f };
	effectDesc.vScale = { 0.7f,1.3f,0.1f };
	effectDesc.vRotation = { 0.f,-89.f,0.f };
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_MARINHOUSE, TEXT("Layer_Effect"),
		TEXT("Prototype_GameObject_Grad"), &effectDesc)))
		return E_FAIL;

	effectDesc.fColor = { 1.1f,1.f,1.f,0.25f };
	effectDesc.vPosition = { 5.84f, 2.f, 0.f };
	effectDesc.vScale = { 0.7f,1.3f,0.1f };
	effectDesc.vRotation = { 0.f,89.f,0.f };
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_MARINHOUSE, TEXT("Layer_Effect"),
		TEXT("Prototype_GameObject_Grad"), &effectDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_MarinHouse::Read()
{
	const char cFile[50] = "../Bin/Map_Data/MarinHouse.dat";
	ifstream fin(cFile, ios::in | ios::binary);
	
	if (!fin.is_open())    // 파일 열었다면
	{
		MSG_BOX(TEXT("파일 읽기를 실패했어요.."));
		return E_FAIL;
	}

	string line;
	int LayerCout = { 0 };
	fin.read(reinterpret_cast<char*>(&LayerCout), sizeof(LayerCout));

	string strLayerTag = {};
	std::string::size_type iStrSize = { 0 };
	_uint iObjectCount = 0;
	int i = 0;
	int iObjectType = 0;
	int iObjectListIndex = 0;
	_float3 fPos = {};
	_float3 fScaled = {};
	_float3 fRot = {};
	_int iCellNum = {};
	while (i < LayerCout)
	{
		fin.read(reinterpret_cast<char*>(&iStrSize), sizeof(iStrSize));
		strLayerTag.resize(iStrSize);
		fin.read(&strLayerTag[0], iStrSize);
		fin.read(reinterpret_cast<char*>(&iObjectCount), sizeof(iObjectCount));

		for (int j = 0; j < iObjectCount; ++j)
		{
			fin.read(reinterpret_cast<char*>(&iObjectType), sizeof(iObjectType));
			fin.read(reinterpret_cast<char*>(&iObjectListIndex), sizeof(iObjectListIndex));

			fin.read(reinterpret_cast<char*>(&fPos.x), sizeof(_float));
			fin.read(reinterpret_cast<char*>(&fPos.y), sizeof(_float));
			fin.read(reinterpret_cast<char*>(&fPos.z), sizeof(_float));

			fin.read(reinterpret_cast<char*>(&fScaled.x), sizeof(_float));
			fin.read(reinterpret_cast<char*>(&fScaled.y), sizeof(_float));
			fin.read(reinterpret_cast<char*>(&fScaled.z), sizeof(_float));


			fin.read(reinterpret_cast<char*>(&fRot.x), sizeof(_float));
			fin.read(reinterpret_cast<char*>(&fRot.y), sizeof(_float));
			fin.read(reinterpret_cast<char*>(&fRot.z), sizeof(_float));

			fin.read(reinterpret_cast<char*>(&iCellNum), sizeof(_int));


			if (strLayerTag == "Layer_Land")
				Read_LandObjects(iObjectType, iObjectListIndex, fPos, fScaled, fRot);
			else if (iObjectType == CGameObject::NONANIM_OBJ)
				Read_NonAnimObj(iObjectType, iObjectListIndex, fPos, fScaled, fRot, strLayerTag);
			else if (iObjectType == CGameObject::ANIM_OBJ)
				Read_AnimObj(iObjectType, iObjectListIndex, fPos, fScaled, fRot, strLayerTag);

		}

		++i;
	}

	fin.close();
	//MSG_BOX(TEXT("파일 읽기를 성공했습니다.."));

	return S_OK;
}

HRESULT CLevel_MarinHouse::Read_Nav()
{

	//const char cFile[20] = "../Bin/Nav_Data.dat";
	//ifstream fin(cFile, ios::in | ios::binary);

	////	fin.open("../Bin/Map_Data.txt");
	//if (!fin.is_open())    // 파일 열었다면
	//{
	//	MSG_BOX(TEXT("Nav 파일 읽기를 실패했어요.."));
	//	return;
	//}

	//_uint iCellCout = { 0 };
	////getline(fin, line);
	////LayerCout = std::stoi(line);
	//fin.read(reinterpret_cast<char*>(&iCellCout), sizeof(iCellCout));

	//_float3 vPos = {};

	//for (int i = 0; i < iCellCout; ++i)
	//{
	//	_uint iCellType = { };
	//	fin.read(reinterpret_cast<char*>(&iCellType), sizeof(iCellType));
	//	m_pNavigationController->Set_CellType(iCellType);

	//	_uint iRoomNum = { };
	//	fin.read(reinterpret_cast<char*>(&iRoomNum), sizeof(iRoomNum));
	//	m_pNavigationController->Set_CellType(iRoomNum);

	//	for (int j = 0; j < 3; ++j)
	//	{
	//		fin.read(reinterpret_cast<char*>(&vPos.x), sizeof(_float));
	//		fin.read(reinterpret_cast<char*>(&vPos.y), sizeof(_float));
	//		fin.read(reinterpret_cast<char*>(&vPos.z), sizeof(_float));

	//		m_pNavigationController->Add_Point(XMLoadFloat3(&vPos));
	//	}
	//}

	//fin.close();
	//MSG_BOX(TEXT("파일 읽기를 성공했습니다.."));

	return S_OK;
}

HRESULT CLevel_MarinHouse::Read_LandObjects(_int _type, _uint _index, _float3 _fPos, _float3 _fScaled, _float3 _fRot)
{
	CGameObject::GAMEOBJECT_DESC pDesc = { };
	pDesc.eType = static_cast<CGameObject::OBJ_TYPE>(_type);
	pDesc.listIndex = _index;


	pDesc.vPosition = _fPos;
	pDesc.vScale = _fScaled;
	pDesc.vRotation = _fRot;
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_MARINHOUSE, TEXT("Layer_Land"), TEXT("Prototype_GameObject_Land"), &pDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_MarinHouse::Read_AnimObj(_int _type, _uint _index, _float3 _fPos, _float3 _fScaled, _float3 _fRot, string _strLyaerTag)
{
	CGameObject::GAMEOBJECT_DESC pDesc = { };
	pDesc.eType = static_cast<CGameObject::OBJ_TYPE>(_type);
	pDesc.listIndex = _index;

	pDesc.vPosition = _fPos;
	pDesc.vScale = _fScaled;
	pDesc.vRotation = _fRot;

	if (_strLyaerTag == "Layer_Bed")
	{
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_MARINHOUSE, TEXT("Layer_Bed"), TEXT("Prototype_GameObject_Bed"), &pDesc)))
			return E_FAIL;
	}
	return S_OK;
}

HRESULT CLevel_MarinHouse::Read_NonAnimObj(_int _type, _uint _index, _float3 _fPos, _float3 _fScaled, _float3 _fRot, string _strLyaerTag)
{
	CGameObject::GAMEOBJECT_DESC pDesc = { };
	pDesc.eType = static_cast<CGameObject::OBJ_TYPE>(_type);
	pDesc.listIndex = _index;

	pDesc.vPosition = _fPos;
	pDesc.vScale = _fScaled;
	pDesc.vRotation = _fRot;

	if(_strLyaerTag == "Layer_HousePot")
	{
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_MARINHOUSE, TEXT("Layer_HousePot"), TEXT("Prototype_GameObject_HousePot"), &pDesc)))
			return E_FAIL;
	}

	return S_OK;
}

CLevel_MarinHouse * CLevel_MarinHouse::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_MarinHouse*		pInstance = new CLevel_MarinHouse(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_MarinHouse"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_MarinHouse::Free()
{
	__super::Free();

}
