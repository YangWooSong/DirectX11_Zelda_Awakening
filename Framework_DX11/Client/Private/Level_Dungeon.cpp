#include "stdafx.h"
#include "Level_Dungeon.h"
#include "Level_Loading.h"
#include "GameInstance.h"
#include "FreeCamera.h"
#include "PlayerCamera.h"
#include "Link.h"
#include "NavDataObj.h"
#include "Monster.h"
#include "DeguTail_00.h"
#include "Layer.h"
#include "Land.h"
#include "FootSwitch.h"
#include "TreasureBox.h"
#include "ClosedPotDoor.h"
#include "PurpleQuartz.h"
#include "Vegas.h"
#include "OnewayDoorReverse.h"
#include "Rola.h"
#include "SquareBlock.h"
#include "MainUI.h"
#include "MiniMap.h"
#include "MapUI.h"
#include "2DEffects.h"
#include <fstream>
CLevel_Dungeon::CLevel_Dungeon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_Dungeon::Initialize()
{
    if (FAILED(Ready_Lights()))
        return E_FAIL;

    if (FAILED(Ready_LandObjects()))
        return E_FAIL;
    if (FAILED(Ready_Layer_Camera()))
        return E_FAIL;
    if (FAILED(Ready_Layer_BackGround()))
        return E_FAIL;
    if (FAILED(Ready_Layer_Effect()))
        return E_FAIL;

   Read();

   m_pGameInstance->Play_BGM(TEXT("0_Dangeon1_TailCave.wav"), 0.7f);
   m_pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Player(LEVEL_DUNGEON));
   CMainUI* pMainUI = static_cast<CMainUI*>(m_pGameInstance->Find_Object(LEVEL_DUNGEON, TEXT("Layer_MainUI"),0));
   m_pMiniMapUI = static_cast<CMiniMap*>(pMainUI->Get_ChildUI(CMainUI::MAP));
   Safe_AddRef(m_pPlayer);
   Safe_AddRef(m_pMiniMapUI);

   //처음 땅 활성화
   CLayer* pLandLayer = m_pGameInstance->Find_Layer(LEVEL_DUNGEON, TEXT("Layer_Land"));
   for (auto iter = pLandLayer->Get_ObjectList().begin(); iter != pLandLayer->Get_ObjectList().end(); iter++)
   {
	   if (static_cast<CLand*>(*iter)->Get_ListIndex() == m_RoomNumList[0][0])
		   static_cast<CLand*>(*iter)->SetActive(true);
	   else
		   static_cast<CLand*>(*iter)->SetActive(false);
   }

   CLayer* pLayer = m_pGameInstance->Find_Layer(LEVEL_DUNGEON, TEXT("Layer_Fire"));

   for (auto iter = pLayer->Get_ObjectList().begin(); iter != pLayer->Get_ObjectList().end(); iter++)
   {
	   if (static_cast<CGameObject*>(*iter)->Get_RoomNum() == m_iCurRoomNum)
		   static_cast<CGameObject*>(*iter)->SetActive(true);
	   else
		   static_cast<CGameObject*>(*iter)->SetActive(false);
   }

   pMainUI->Active_LevelText();
   pMainUI->Active_FadeIn();

   return S_OK;
}


void CLevel_Dungeon::Update(_float fTimeDelta)
{
	if(m_iCurRoomNum != m_pPlayer->Get_CurRoomNum())
		Change_Room();

	Setting_Gimmick(fTimeDelta);
}

HRESULT CLevel_Dungeon::Render()
{
    SetWindowText(g_hWnd, TEXT("Dungoen 레벨입니다."));
    return S_OK;
}

HRESULT CLevel_Dungeon::Ready_Lights()
{
	/* 게임플레이 레벨에 필요한 광원을 준비한다. */
	LIGHT_DESC			LightDesc{};

	ZeroMemory(&LightDesc, sizeof LightDesc);
	LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.5f, 0.5f, 0.5f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
		return E_FAIL;

    return S_OK;
}

HRESULT CLevel_Dungeon::Ready_Layer_Camera()
{
    //CFreeCamera::CAMERA_FREE_DESC		Desc{};

    //Desc.fSensor = 0.2f;
    //Desc.vEye = _float4(0.f, 10.f, -10.f, 1.f);
    //Desc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
    //Desc.fFovy = XMConvertToRadians(60.0f);
    //Desc.fAspect = (_float)g_iWinSizeX / (_float)g_iWinSizeY;
    //Desc.fNear = 0.1f;
    //Desc.fFar = 1000.f;
    //Desc.fSpeedPerSec = 30.f;
    //Desc.fRotationPerSec = XMConvertToRadians(90.0f);

    //if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON, TEXT("Layer_Camera"), TEXT("Prototype_GameObject_FreeCamera"), &Desc)))
    //    return E_FAIL;


	CPlayerCamera::CAMERA_PLAYER_DESC	Desc{};

	Desc.vEye = _float4(0.f, 20.f, -11.f, 1.f);
	Desc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	Desc.fFovy = XMConvertToRadians(31.f);
	Desc.fAspect = (_float)g_iWinSizeX / (_float)g_iWinSizeY;
	Desc.fNear = 0.1f;
	Desc.fFar = 1000.f;
	Desc.fSpeedPerSec = 30.f;
	Desc.fRotationPerSec = XMConvertToRadians(90.0f);
	Desc.fDefaultAngle = 60.f;
	Desc.fOffest = _float3(0.0f, 20.0f, -11.f);
	Desc.fSpeed = 4.f;
	Desc.pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Find_Player(LEVEL_DUNGEON));
	Desc.bFollowPlayer = false;

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON, TEXT("Layer_Camera"), TEXT("Prototype_GameObject_PlayerCamera"), &Desc)))
		return E_FAIL;

	m_pPlayerCam = dynamic_cast<CPlayerCamera*>(m_pGameInstance->Find_Camera(LEVEL_DUNGEON));

    return S_OK;
}

HRESULT CLevel_Dungeon::Ready_Layer_BackGround()
{
    return S_OK;
}

HRESULT CLevel_Dungeon::Ready_Layer_Effect()
{
    return S_OK;
}

HRESULT CLevel_Dungeon::Ready_LandObjects()
{
	CPlayer::PLAYER_DESC PlayerDesc{};
    PlayerDesc.vPosition = _float3(-0.032f, 0.048f, -5.052f);
    PlayerDesc.LevelIndex = LEVEL_DUNGEON;
	PlayerDesc.iStartCellNum = 1;
    if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON, TEXT("Layer_Player"), TEXT("Prototype_GameObject_Player_Link"), &PlayerDesc)))
        return E_FAIL;

	CNavDataObj::NAVOBJ_DESC NavDes{};
	NavDes.iLevelNum = LEVEL_DUNGEON;
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON, TEXT("Layer_NavDataObj"), TEXT("Prototype_GameObject_NavDataObj"), &NavDes)))
		return E_FAIL;

	CGameObject::GAMEOBJECT_DESC ObjectDesc{};
	ObjectDesc.eType = CGameObject::NONANIM_OBJ;
	ObjectDesc.iRoomNum = 2;
	ObjectDesc.vPosition = _float3(-18.75f, 10.f, 0.69f);
	ObjectDesc.vScale = _float3(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON, TEXT("Layer_SmallKey"), TEXT("Prototype_GameObject_SmallKey"), &ObjectDesc)))
		return E_FAIL;

	ObjectDesc.eType = CGameObject::NONANIM_OBJ;
	ObjectDesc.iRoomNum = 13;
	ObjectDesc.vPosition = _float3(45.f, 2.f, 62.f);
	ObjectDesc.vScale = _float3(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON, TEXT("Layer_ConchHorn"), TEXT("Prototype_GameObject_ConchHorn"), &ObjectDesc)))
		return E_FAIL;

	CSquareBlock::SQUARE_DESC SquareDesc{};
	ObjectDesc.eType = CGameObject::NONANIM_OBJ;
	ObjectDesc.iRoomNum = 14;
	ObjectDesc.vPosition = _float3(-17.22f, 0.f, 48.72f);
	ObjectDesc.vScale = _float3(1.f, 1.f, 1.f);
	SquareDesc.bActiveCollider = true;
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON, TEXT("Layer_SquareBlock_Gimmick"), TEXT("Prototype_GameObject_SquareBlock"), &ObjectDesc)))
		return E_FAIL;

	CMainUI::MAINUI_DESC MainDesc{};
	MainDesc.iLevelIndex = LEVEL_DUNGEON;
	MainDesc.fSizeX = g_iWinSizeX;
	MainDesc.fSizeY = g_iWinSizeY;
	MainDesc.fX = g_iWinSizeX / 2;
	MainDesc.fY = g_iWinSizeY / 2;

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON, TEXT("Layer_MainUI"),
		TEXT("Prototype_GameObject_MainUI"), &MainDesc)))
		return E_FAIL;

    return S_OK;
}

HRESULT CLevel_Dungeon::Read()
{
	const char cFile[50] = "../Bin/Map_Data/Dungeon.dat";
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
	_uint iRoomNum = {};

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
			fin.read(reinterpret_cast<char*>(&iRoomNum), sizeof(_uint));

			if (strLayerTag == "Layer_Land")
				Read_LandObjects(iObjectType, iObjectListIndex, fPos, fScaled, fRot);
			else if (iObjectType == CGameObject::ANIM_MONSTER)
				Read_AnimMonster(iObjectType, iObjectListIndex, fPos, fScaled, fRot, strLayerTag, iCellNum, iRoomNum);
			else if (iObjectType == CGameObject::NONANIM_OBJ)
				Read_NonAnimObj(iObjectType, iObjectListIndex, fPos, fScaled, fRot, strLayerTag, iRoomNum);
			else if (iObjectType == CGameObject::ANIM_OBJ)
				Read_AnimObj(iObjectType, iObjectListIndex, fPos, fScaled, fRot, strLayerTag, iCellNum, iRoomNum);
		}

		++i;
	}

	fin.close();
	//MSG_BOX(TEXT("파일 읽기를 성공했습니다.."));

	return S_OK;
}

HRESULT CLevel_Dungeon::Read_LandObjects(_int _type, _uint _index, _float3 _fPos, _float3 _fScaled, _float3 _fRot)
{
	CGameObject::GAMEOBJECT_DESC pDesc = { };
	pDesc.eType = static_cast<CGameObject::OBJ_TYPE>(_type);
	pDesc.listIndex = _index;

	pDesc.vPosition = _fPos;
	pDesc.vScale = _fScaled;
	pDesc.vRotation = _fRot;
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON, TEXT("Layer_Land"), TEXT("Prototype_GameObject_Land"), &pDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Dungeon::Read_AnimMonster(_int _type, _uint _index, _float3 _fPos, _float3 _fScaled, _float3 _fRot, string _strLyaerTag, _int _iCellNum, _uint _iRoomNum)
{

	CMonster::MONSTER_DESC pDesc = { };
	//pDesc.eType = static_cast<CGameObject::OBJ_TYPE>(_type);
	//pDesc.listIndex = _index;

	pDesc.vPosition = _fPos;
	pDesc.vScale = _fScaled;
	pDesc.vRotation = _fRot;
	pDesc.LevelIndex = LEVEL_DUNGEON;
	pDesc.iCellNum = _iCellNum;
	pDesc.eType = CGameObject::ANIM_MONSTER;
	pDesc.iRoomNum = _iRoomNum;
	if (_strLyaerTag == "Layer_DeguTail")
	{
		pDesc.vPosition.x -= 0.1f;
		pDesc.iCellNum = 1138;
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON, TEXT("Layer_DeguTail"), TEXT("Prototype_GameObject_DeguTail_00"), &pDesc)))
			return E_FAIL;
	}
	else if (_strLyaerTag == "Layer_Pawn")
	{
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON, TEXT("Layer_Pawn"), TEXT("Prototype_GameObject_Pawn"), &pDesc)))
			return E_FAIL;
	}
	else if (_strLyaerTag == "Layer_Spark")
	{
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Spark"), &pDesc)))
			return E_FAIL;
	}
	else if (_strLyaerTag == "Layer_Vegas")
	{
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON, TEXT("Layer_Vegas"), TEXT("Prototype_GameObject_Vegas"), &pDesc)))
			return E_FAIL;
	}
	else if (_strLyaerTag == "Layer_Rola")
	{
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON, TEXT("Layer_Rola"), TEXT("Prototype_GameObject_Rola"), &pDesc)))
			return E_FAIL;
	}
	else if (_strLyaerTag == "Layer_Togezo")
	{
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON, TEXT("Layer_Togezo"), TEXT("Prototype_GameObject_Togezo"), &pDesc)))
			return E_FAIL;
	}
	return S_OK;
}

HRESULT CLevel_Dungeon::Read_AnimObj(_int _type, _uint _index, _float3 _fPos, _float3 _fScaled, _float3 _fRot, string _strLyaerTag, _int _iCellNum, _uint _iRoomNum)
{
	CGameObject::GAMEOBJECT_DESC pDesc = { };
	pDesc.eType = static_cast<CGameObject::OBJ_TYPE>(_type);
	pDesc.listIndex = _index;

	pDesc.vPosition = _fPos;
	pDesc.vScale = _fScaled;
	pDesc.vRotation = _fRot;
	pDesc.iCellNum = _iCellNum;
	pDesc.iRoomNum = _iRoomNum;
	if (_strLyaerTag == "Layer_SpikeTile")
	{
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_SpikeTile"), &pDesc)))
			return E_FAIL;
	}
	else if (_strLyaerTag == "Layer_TreasureBox")
	{
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON, TEXT("Layer_TreasureBox"), TEXT("Prototype_GameObject_TreasureBox"), &pDesc)))
			return E_FAIL;
	}
	else if (_strLyaerTag == "Layer_FootSwitch")
	{
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON, TEXT("Layer_FootSwitch"), TEXT("Prototype_GameObject_FootSwitch"), &pDesc)))
			return E_FAIL;
	}
	else if (_strLyaerTag == "Layer_ClosedPotDoor")
	{
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON, TEXT("Layer_ClosedPotDoor"), TEXT("Prototype_GameObject_ClosedPotDoor"), &pDesc)))
			return E_FAIL;
	}
	else if (_strLyaerTag == "Layer_BladeTrap")
	{
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_BladeTrap"), &pDesc)))
			return E_FAIL;
	}
	else if (_strLyaerTag == "Layer_LockDoor")
	{
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON, TEXT("Layer_LockDoor"), TEXT("Prototype_GameObject_LockDoor"), &pDesc)))
			return E_FAIL;
	}
	else if (_strLyaerTag == "Layer_RollingSpike_Anim")
	{
		_fPos.x = 48.f;
		pDesc.vPosition = _fPos;
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON, TEXT("Layer_RollingSpike"), TEXT("Prototype_GameObject_RollingSpike"), &pDesc)))
			return E_FAIL;
	}
	else if (_strLyaerTag == "Layer_OnewayDoorReverse")
	{
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON, TEXT("Layer_OnewayDoorReverse"), TEXT("Prototype_GameObject_OnewayDoorReverse"), &pDesc)))
			return E_FAIL;
	}
	else if (_strLyaerTag == "Layer_BossDoor")
	{
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON, TEXT("Layer_BossDoor"), TEXT("Prototype_GameObject_BossDoor"), &pDesc)))
			return E_FAIL;
	}
	return S_OK;
}

HRESULT CLevel_Dungeon::Read_NonAnimObj(_int _type, _uint _index, _float3 _fPos, _float3 _fScaled, _float3 _fRot, string _strLyaerTag, _uint _iRoomNum)
{
	CGameObject::GAMEOBJECT_DESC pDesc = { };
	pDesc.eType = static_cast<CGameObject::OBJ_TYPE>(_type);
	pDesc.listIndex = _index;

	pDesc.vPosition = _fPos;
	pDesc.vScale = _fScaled;
	pDesc.vRotation = _fRot;
	pDesc.iRoomNum = _iRoomNum;

	if (_strLyaerTag == "Layer_DungeonCollapseTileLv01")
	{
		pDesc.iRoomNum = 12;
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON, TEXT("Layer_DungeonCollapseTileLv01"), TEXT("Prototype_GameObject_CollapseTile"), &pDesc)))
			return E_FAIL;
	}
	else if (_strLyaerTag == "Layer_DungeonCrackedWallLv01")
	{
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON, TEXT("Layer_DungeonCrackedWallLv01"), TEXT("Prototype_GameObject_CrackedWall"), &pDesc)))
			return E_FAIL;
	}
	else if (_strLyaerTag == "Layer_DungeonOwlStatue")
	{
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON, TEXT("Layer_DungeonOwlStatue"), TEXT("Prototype_GameObject_OwlStatue"), &pDesc)))
			return E_FAIL;
	}
	else if (_strLyaerTag == "Layer_SquareBlock")
	{
		CSquareBlock::SQUARE_DESC pSqureDesc = {  };
		pSqureDesc.bActiveCollider = false;
		pSqureDesc.eType = static_cast<CGameObject::OBJ_TYPE>(_type);
		pSqureDesc.listIndex = _index;

		pSqureDesc.vPosition = _fPos;
		pSqureDesc.vScale = _fScaled;
		pSqureDesc.vRotation = _fRot;
		pSqureDesc.iRoomNum = _iRoomNum;
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON, TEXT("Layer_SquareBlock"), TEXT("Prototype_GameObject_SquareBlock"), &pSqureDesc)))
			return E_FAIL;
	}
	else if (_strLyaerTag == "Layer_StoneHinoxRock")
	{
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON, TEXT("Layer_StoneHinoxRock"), TEXT("Prototype_GameObject_StoneHinoxRock"), &pDesc)))
			return E_FAIL;
	}
	else if (_strLyaerTag == "Layer_HousePot")
	{
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON, TEXT("Layer_HousePot"), TEXT("Prototype_GameObject_HousePot"), &pDesc)))
			return E_FAIL;
	}
	else if (_strLyaerTag == "Layer_PurpleQuartz")
	{
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON, TEXT("Layer_PurpleQuartz"), TEXT("Prototype_GameObject_PurpleQuartz"), &pDesc)))
			return E_FAIL;
	}
	else if (_strLyaerTag == "Layer_LockBlock")
	{
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON, TEXT("Layer_LockBlock"), TEXT("Prototype_GameObject_LockBlock"), &pDesc)))
			return E_FAIL;
	}	
	else if (_strLyaerTag == "Layer_AppleRed")
	{
		C2DEffects::EFFECT_DESC EffectDesc = {};
		EffectDesc.iEffectType = FIRE_BIG_EFFECT;
		EffectDesc.iLevelIndex = LEVEL_DUNGEON;
		EffectDesc.vPosition = _fPos;
		EffectDesc.iRoomNum = _iRoomNum;
		EffectDesc.vScale = _fScaled;
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON, TEXT("Layer_Fire"), TEXT("Prototype_GameObject_Fire_Big_Effect"), &EffectDesc)))
			return E_FAIL;
	}
	return S_OK;
}

void CLevel_Dungeon::Change_Room()
{
	m_iCurRoomNum = m_pPlayer->Get_CurRoomNum();

	//도착했던 방인지 검사, 도착한 적 없으면 리스트에 넣음
	_bool bArrived = { false };
	for (auto& iter : m_ArrivedRoomNum)
	{
		if (m_iCurRoomNum == iter)
			bArrived = true;
	}

	if(bArrived == false)
	{
		m_ArrivedRoomNum.push_back(m_iCurRoomNum);
		Set_MiniMap(m_iCurRoomNum);
	}

	//기믹 세팅
	CLayer* pLayer = { nullptr };

	_float3 vNewCamPos = { m_CameraRoomPos[m_iCurRoomNum - 1].x, m_CameraRoomPos[m_iCurRoomNum - 1].y, m_CameraRoomPos[m_iCurRoomNum - 1].z };

	if (m_iCurRoomNum == 7 || m_iCurRoomNum == 14 || m_iCurRoomNum == 16)
	{
		m_pPlayerCam->Set_FollowPlayer(true);
	}
	else
	{
		m_pPlayerCam->Set_FollowPlayer(false);
		m_pPlayerCam->Set_TargetToOtherPos(XMLoadFloat3(&vNewCamPos));
	}

#pragma region Land
	pLayer = m_pGameInstance->Find_Layer(LEVEL_DUNGEON, TEXT("Layer_Land"));

	for (auto iter = pLayer->Get_ObjectList().begin(); iter != pLayer->Get_ObjectList().end(); iter++)
	{
		switch (m_iCurRoomNum)
		{
		case 7:
			if (static_cast<CLand*>(*iter)->Get_ListIndex() == m_RoomNumList[m_iCurRoomNum - 1][0]
				|| static_cast<CLand*>(*iter)->Get_ListIndex() == m_RoomNumList[m_iCurRoomNum - 1][1]
				|| static_cast<CLand*>(*iter)->Get_ListIndex() == m_RoomNumList[m_iCurRoomNum - 1][2]
				|| static_cast<CLand*>(*iter)->Get_ListIndex() == m_RoomNumList[m_iCurRoomNum - 1][3]
				|| static_cast<CLand*>(*iter)->Get_ListIndex() == m_RoomNumList[m_iCurRoomNum - 1][4]
				|| static_cast<CLand*>(*iter)->Get_ListIndex() == m_RoomNumList[m_iCurRoomNum - 1][5])
				static_cast<CLand*>(*iter)->SetActive(true);
			else
				static_cast<CLand*>(*iter)->SetActive(false);
			break;
		case 14:
			if (static_cast<CLand*>(*iter)->Get_ListIndex() == m_RoomNumList[m_iCurRoomNum - 1][0] 
				|| static_cast<CLand*>(*iter)->Get_ListIndex() == m_RoomNumList[m_iCurRoomNum - 1][1])
				static_cast<CLand*>(*iter)->SetActive(true);
			else
				static_cast<CLand*>(*iter)->SetActive(false);
			break;
		case 16:
			if (static_cast<CLand*>(*iter)->Get_ListIndex() == m_RoomNumList[m_iCurRoomNum - 1][0]
				|| static_cast<CLand*>(*iter)->Get_ListIndex() == m_RoomNumList[m_iCurRoomNum - 1][1]
				|| static_cast<CLand*>(*iter)->Get_ListIndex() == m_RoomNumList[m_iCurRoomNum - 1][2])
				static_cast<CLand*>(*iter)->SetActive(true);
			else
				static_cast<CLand*>(*iter)->SetActive(false);
			break;
		default:
			if (static_cast<CLand*>(*iter)->Get_ListIndex() == m_RoomNumList[m_iCurRoomNum - 1][0])
				static_cast<CLand*>(*iter)->SetActive(true);
			else
				static_cast<CLand*>(*iter)->SetActive(false);
			break;
		}
	}
#pragma endregion

#pragma region Monster
	//SpikeTile 포함

	pLayer = m_pGameInstance->Find_Layer(LEVEL_DUNGEON, TEXT("Layer_Monster"));

	for (auto iter = pLayer->Get_ObjectList().begin(); iter != pLayer->Get_ObjectList().end(); iter++)
	{
		if (static_cast<CGameObject*>(*iter)->Get_RoomNum() == m_iCurRoomNum)
			static_cast<CGameObject*>(*iter)->SetActive(true);
		else
			static_cast<CGameObject*>(*iter)->SetActive(false);
	}

	pLayer = m_pGameInstance->Find_Layer(LEVEL_DUNGEON, TEXT("Layer_Pawn"));

	for (auto iter = pLayer->Get_ObjectList().begin(); iter != pLayer->Get_ObjectList().end(); iter++)
	{
		if (static_cast<CGameObject*>(*iter)->Get_RoomNum() == m_iCurRoomNum)
			static_cast<CGameObject*>(*iter)->SetActive(true);
		else
			static_cast<CGameObject*>(*iter)->SetActive(false);
	}
	
	pLayer = m_pGameInstance->Find_Layer(LEVEL_DUNGEON, TEXT("Layer_Vegas"));

	for (auto iter = pLayer->Get_ObjectList().begin(); iter != pLayer->Get_ObjectList().end(); iter++)
	{
		if (static_cast<CGameObject*>(*iter)->Get_RoomNum() == m_iCurRoomNum)
			static_cast<CGameObject*>(*iter)->SetActive(true);
		else
			static_cast<CGameObject*>(*iter)->SetActive(false);
	}

	pLayer = m_pGameInstance->Find_Layer(LEVEL_DUNGEON, TEXT("Layer_Rola"));

	for (auto iter = pLayer->Get_ObjectList().begin(); iter != pLayer->Get_ObjectList().end(); iter++)
	{
		if (static_cast<CGameObject*>(*iter)->Get_RoomNum() == m_iCurRoomNum)
			static_cast<CGameObject*>(*iter)->SetActive(true);
		else
			static_cast<CGameObject*>(*iter)->SetActive(false);
	}
	
	pLayer = m_pGameInstance->Find_Layer(LEVEL_DUNGEON, TEXT("Layer_Togezo"));

	for (auto iter = pLayer->Get_ObjectList().begin(); iter != pLayer->Get_ObjectList().end(); iter++)
	{
		if (static_cast<CGameObject*>(*iter)->Get_RoomNum() == m_iCurRoomNum)
			static_cast<CGameObject*>(*iter)->SetActive(true);
		else
			static_cast<CGameObject*>(*iter)->SetActive(false);
	}
#pragma endregion

#pragma region NonAnimObj
	pLayer = m_pGameInstance->Find_Layer(LEVEL_DUNGEON, TEXT("Layer_DungeonCollapseTileLv01"));

	for (auto iter = pLayer->Get_ObjectList().begin(); iter != pLayer->Get_ObjectList().end(); iter++)
	{
		if (static_cast<CGameObject*>(*iter)->Get_RoomNum() == m_iCurRoomNum)
			static_cast<CGameObject*>(*iter)->SetActive(true);
		else
			static_cast<CGameObject*>(*iter)->SetActive(false);
	}

	pLayer = m_pGameInstance->Find_Layer(LEVEL_DUNGEON, TEXT("Layer_DungeonCrackedWallLv01"));

	for (auto iter = pLayer->Get_ObjectList().begin(); iter != pLayer->Get_ObjectList().end(); iter++)
	{
		if (static_cast<CGameObject*>(*iter)->Get_RoomNum() == m_iCurRoomNum)
			static_cast<CGameObject*>(*iter)->SetActive(true);
		else
			static_cast<CGameObject*>(*iter)->SetActive(false);
	}

	pLayer = m_pGameInstance->Find_Layer(LEVEL_DUNGEON, TEXT("Layer_DungeonOwlStatue"));

	for (auto iter = pLayer->Get_ObjectList().begin(); iter != pLayer->Get_ObjectList().end(); iter++)
	{
		if (static_cast<CGameObject*>(*iter)->Get_RoomNum() == m_iCurRoomNum)
			static_cast<CGameObject*>(*iter)->SetActive(true);
		else
			static_cast<CGameObject*>(*iter)->SetActive(false);
	}

	pLayer = m_pGameInstance->Find_Layer(LEVEL_DUNGEON, TEXT("Layer_SquareBlock"));

	for (auto iter = pLayer->Get_ObjectList().begin(); iter != pLayer->Get_ObjectList().end(); iter++)
	{
		if (static_cast<CGameObject*>(*iter)->Get_RoomNum() == m_iCurRoomNum)
			static_cast<CGameObject*>(*iter)->SetActive(true);
		else
			static_cast<CGameObject*>(*iter)->SetActive(false);
	}

	pLayer = m_pGameInstance->Find_Layer(LEVEL_DUNGEON, TEXT("Layer_StoneHinoxRock"));

	for (auto iter = pLayer->Get_ObjectList().begin(); iter != pLayer->Get_ObjectList().end(); iter++)
	{
		if (static_cast<CGameObject*>(*iter)->Get_RoomNum() == m_iCurRoomNum)
			static_cast<CGameObject*>(*iter)->SetActive(true);
		else
			static_cast<CGameObject*>(*iter)->SetActive(false);
	}

	pLayer = m_pGameInstance->Find_Layer(LEVEL_DUNGEON, TEXT("Layer_HousePot"));

	for (auto iter = pLayer->Get_ObjectList().begin(); iter != pLayer->Get_ObjectList().end(); iter++)
	{
		if (static_cast<CGameObject*>(*iter)->Get_RoomNum() == m_iCurRoomNum)
			static_cast<CGameObject*>(*iter)->SetActive(true);
		else
			static_cast<CGameObject*>(*iter)->SetActive(false);
	}

	pLayer = m_pGameInstance->Find_Layer(LEVEL_DUNGEON, TEXT("Layer_PurpleQuartz"));

	for (auto iter = pLayer->Get_ObjectList().begin(); iter != pLayer->Get_ObjectList().end(); iter++)
	{
		if (static_cast<CGameObject*>(*iter)->Get_RoomNum() == m_iCurRoomNum)
		{
			if(static_cast<CPurpleQuartz*>(*iter)->Get_isBreaked() == false)
				static_cast<CGameObject*>(*iter)->SetActive(true);
		}
		else
			static_cast<CGameObject*>(*iter)->SetActive(false);
	}

	pLayer = m_pGameInstance->Find_Layer(LEVEL_DUNGEON, TEXT("Layer_LockBlock"));

	for (auto iter = pLayer->Get_ObjectList().begin(); iter != pLayer->Get_ObjectList().end(); iter++)
	{
		if (static_cast<CGameObject*>(*iter)->Get_RoomNum() == m_iCurRoomNum)
			static_cast<CGameObject*>(*iter)->SetActive(true);
		else
			static_cast<CGameObject*>(*iter)->SetActive(false);
	}

	pLayer = m_pGameInstance->Find_Layer(LEVEL_DUNGEON, TEXT("Layer_ConchHorn"));

	for (auto iter = pLayer->Get_ObjectList().begin(); iter != pLayer->Get_ObjectList().end(); iter++)
	{
		if (static_cast<CGameObject*>(*iter)->Get_RoomNum() == m_iCurRoomNum)
			static_cast<CGameObject*>(*iter)->SetActive(true);
		else
			static_cast<CGameObject*>(*iter)->SetActive(false);
	}

	pLayer = m_pGameInstance->Find_Layer(LEVEL_DUNGEON, TEXT("Layer_SquareBlock_Gimmick"));

	for (auto iter = pLayer->Get_ObjectList().begin(); iter != pLayer->Get_ObjectList().end(); iter++)
	{
		if (static_cast<CGameObject*>(*iter)->Get_RoomNum() == m_iCurRoomNum)
			static_cast<CGameObject*>(*iter)->SetActive(true);
		else
			static_cast<CGameObject*>(*iter)->SetActive(false);
	}
#pragma endregion

#pragma region AnimObj

	pLayer = m_pGameInstance->Find_Layer(LEVEL_DUNGEON, TEXT("Layer_TreasureBox"));

	for (auto iter = pLayer->Get_ObjectList().begin(); iter != pLayer->Get_ObjectList().end(); iter++)
	{
		if (static_cast<CGameObject*>(*iter)->Get_RoomNum() == m_iCurRoomNum && static_cast<CTreasureBox*>(*iter)->Get_bShow())
			static_cast<CGameObject*>(*iter)->SetActive(true);
		else
			static_cast<CGameObject*>(*iter)->SetActive(false);
	}
	
	pLayer = m_pGameInstance->Find_Layer(LEVEL_DUNGEON, TEXT("Layer_FootSwitch"));

	for (auto iter = pLayer->Get_ObjectList().begin(); iter != pLayer->Get_ObjectList().end(); iter++)
	{
		if (static_cast<CGameObject*>(*iter)->Get_RoomNum() == m_iCurRoomNum)
			static_cast<CGameObject*>(*iter)->SetActive(true);
		else
			static_cast<CGameObject*>(*iter)->SetActive(false);
	}

	pLayer = m_pGameInstance->Find_Layer(LEVEL_DUNGEON, TEXT("Layer_ClosedPotDoor"));

	for (auto iter = pLayer->Get_ObjectList().begin(); iter != pLayer->Get_ObjectList().end(); iter++)
	{
		if (static_cast<CGameObject*>(*iter)->Get_RoomNum() == m_iCurRoomNum)
			static_cast<CGameObject*>(*iter)->SetActive(true);
		else
			static_cast<CGameObject*>(*iter)->SetActive(false);
	}
	
	pLayer = m_pGameInstance->Find_Layer(LEVEL_DUNGEON, TEXT("Layer_LockDoor"));

	for (auto iter = pLayer->Get_ObjectList().begin(); iter != pLayer->Get_ObjectList().end(); iter++)
	{
		if (static_cast<CGameObject*>(*iter)->Get_RoomNum() == m_iCurRoomNum)
			static_cast<CGameObject*>(*iter)->SetActive(true);
		else
			static_cast<CGameObject*>(*iter)->SetActive(false);
	}

	pLayer = m_pGameInstance->Find_Layer(LEVEL_DUNGEON, TEXT("Layer_OnewayDoorReverse"));

	for (auto iter = pLayer->Get_ObjectList().begin(); iter != pLayer->Get_ObjectList().end(); iter++)
	{
		if (static_cast<CGameObject*>(*iter)->Get_RoomNum() == m_iCurRoomNum)
			static_cast<CGameObject*>(*iter)->SetActive(true);
		else
			static_cast<CGameObject*>(*iter)->SetActive(false);
	}

	pLayer = m_pGameInstance->Find_Layer(LEVEL_DUNGEON, TEXT("Layer_RollingSpike"));

	for (auto iter = pLayer->Get_ObjectList().begin(); iter != pLayer->Get_ObjectList().end(); iter++)
	{
		if (static_cast<CGameObject*>(*iter)->Get_RoomNum() == m_iCurRoomNum)
			static_cast<CGameObject*>(*iter)->SetActive(true);
		else
			static_cast<CGameObject*>(*iter)->SetActive(false);

	}

	pLayer = m_pGameInstance->Find_Layer(LEVEL_DUNGEON, TEXT("Layer_BossDoor"));

	for (auto iter = pLayer->Get_ObjectList().begin(); iter != pLayer->Get_ObjectList().end(); iter++)
	{
		if (static_cast<CGameObject*>(*iter)->Get_RoomNum() == m_iCurRoomNum)
			static_cast<CGameObject*>(*iter)->SetActive(true);
		else
			static_cast<CGameObject*>(*iter)->SetActive(false);

	}
#pragma endregion

	pLayer = m_pGameInstance->Find_Layer(LEVEL_DUNGEON, TEXT("Layer_Fire"));

	for (auto iter = pLayer->Get_ObjectList().begin(); iter != pLayer->Get_ObjectList().end(); iter++)
	{
		if (static_cast<CGameObject*>(*iter)->Get_RoomNum() == m_iCurRoomNum )
			static_cast<CGameObject*>(*iter)->SetActive(true);
		else
			static_cast<CGameObject*>(*iter)->SetActive(false);
	}

}

void CLevel_Dungeon::Setting_Gimmick(_float fTimeDelta)
{
	CLayer* pLayer = { nullptr };

	switch (m_iCurRoomNum)
	{
	case 2:
	{
		CGameObject* pKey = m_pGameInstance->Find_Object(LEVEL_DUNGEON, TEXT("Layer_SmallKey"), 0);
		if (pKey->Get_Dead() == false)
		{
			int iCountAlive = 0;

			pLayer = m_pGameInstance->Find_Layer(LEVEL_DUNGEON, TEXT("Layer_Pawn"));
			for (auto iter = pLayer->Get_ObjectList().begin(); iter != pLayer->Get_ObjectList().end(); iter++)
			{
				if (static_cast<CGameObject*>(*iter)->Get_RoomNum() == m_iCurRoomNum)
				{
					if (static_cast<CGameObject*>(*iter)->Get_Dead() == false)
						iCountAlive++;
				}
			}

			if (iCountAlive == 0)
				pKey->SetActive(true);
		}
	}
		break;
	case 4:
	{
		CFootSwitch* pSwitch = static_cast<CFootSwitch*>(m_pGameInstance->Find_Object(LEVEL_DUNGEON, TEXT("Layer_FootSwitch"), 0));
		if (pSwitch->Get_IsOn())
		{
			pLayer = m_pGameInstance->Find_Layer(LEVEL_DUNGEON, TEXT("Layer_TreasureBox"));

			for (auto iter = pLayer->Get_ObjectList().begin(); iter != pLayer->Get_ObjectList().end(); iter++)
			{
				if (static_cast<CGameObject*>(*iter)->Get_RoomNum() == m_iCurRoomNum)
				{
					static_cast<CTreasureBox*>(*iter)->Set_bShow(true);
					static_cast<CTreasureBox*>(*iter)->SetActive(true);
				}
			}
		}
	}
		break;
	case 5:
	{
		CClosedPotDoor* pDoor = static_cast<CClosedPotDoor*>(m_pGameInstance->Find_Object(LEVEL_DUNGEON, TEXT("Layer_ClosedPotDoor"), 0));
		if (pDoor->Get_Opend())
		{
			pLayer = m_pGameInstance->Find_Layer(LEVEL_DUNGEON, TEXT("Layer_TreasureBox"));

			for (auto iter = pLayer->Get_ObjectList().begin(); iter != pLayer->Get_ObjectList().end(); iter++)
			{
				if (static_cast<CGameObject*>(*iter)->Get_RoomNum() == m_iCurRoomNum)
				{
					static_cast<CTreasureBox*>(*iter)->Set_bShow(true);
					static_cast<CTreasureBox*>(*iter)->SetActive(true);
				}
			}
		}
	}
		break;
	case 7:
	{
		pLayer = m_pGameInstance->Find_Layer(LEVEL_DUNGEON, TEXT("Layer_PurpleQuartz"));

		_int iAliveCount = 0;

		for (auto iter = pLayer->Get_ObjectList().begin(); iter != pLayer->Get_ObjectList().end(); iter++)
		{
			if (static_cast<CGameObject*>(*iter)->Get_RoomNum() == m_iCurRoomNum)
			{
				if (static_cast<CPurpleQuartz*>(*iter)->Get_isBreaked() == false)
					iAliveCount++;
			}

		}

		if (iAliveCount == 0)
		{
			pLayer = m_pGameInstance->Find_Layer(LEVEL_DUNGEON, TEXT("Layer_TreasureBox"));

			for (auto iter = pLayer->Get_ObjectList().begin(); iter != pLayer->Get_ObjectList().end(); iter++)
			{
				if (static_cast<CGameObject*>(*iter)->Get_CellNum() == 523)
				{
					static_cast<CTreasureBox*>(*iter)->Set_bShow(true);
					static_cast<CTreasureBox*>(*iter)->SetActive(true);
				}
			}
		}
	}
		break;
	case 9:
	{
		_int iTextureNum = -1;
		_int iDemageCount = 0;
		_bool bRestart = false;

		pLayer = m_pGameInstance->Find_Layer(LEVEL_DUNGEON, TEXT("Layer_Vegas"));

		for (auto iter = pLayer->Get_ObjectList().begin(); iter != pLayer->Get_ObjectList().end(); iter++)
		{
			if (static_cast<CVegas*>(*iter)->Get_Fsm()->Get_CurrentState() == CVegas::DAMAGE)
				iDemageCount++;
		}

		if (iDemageCount == 3)	//전제가 다 멈춤 상태인지 확인
		{
			for (auto iter = pLayer->Get_ObjectList().begin(); iter != pLayer->Get_ObjectList().end(); iter++)
			{
				if (iTextureNum == -1)
					iTextureNum = (int) static_cast<CVegas*>(*iter)->Get_TextureNum();
				else
				{
					if ((int) static_cast<CVegas*>(*iter)->Get_TextureNum() != iTextureNum)
						bRestart = true;	//모양이 하나라도 다르면 재시작
				}
			}
		}

		//문양에 따라 초기화 or 죽이기
		if (bRestart)
		{
			for (auto iter = pLayer->Get_ObjectList().begin(); iter != pLayer->Get_ObjectList().end(); iter++)
			{
				static_cast<CVegas*>(*iter)->Get_Fsm()->Change_State(CVegas::IDLE);
			}
		}
		else if (iDemageCount == 3 && bRestart == false)
		{
			for (auto iter = pLayer->Get_ObjectList().begin(); iter != pLayer->Get_ObjectList().end(); iter++)
			{
				static_cast<CVegas*>(*iter)->Get_Fsm()->Change_State(CVegas::DEAD);
			}
		}

		//다 죽으면 상자 활성화
		CVegas* pVegas = static_cast<CVegas*>(m_pGameInstance->Find_Object(LEVEL_DUNGEON, TEXT("Layer_Vegas"), 0));
		if (pVegas->Get_Dead() == true)
		{
			pLayer = m_pGameInstance->Find_Layer(LEVEL_DUNGEON, TEXT("Layer_TreasureBox"));

			for (auto iter = pLayer->Get_ObjectList().begin(); iter != pLayer->Get_ObjectList().end(); iter++)
			{
				if (static_cast<CGameObject*>(*iter)->Get_RoomNum() == m_iCurRoomNum)
				{
					static_cast<CTreasureBox*>(*iter)->Set_bShow(true);
					static_cast<CTreasureBox*>(*iter)->SetActive(true);
				}
			}
		}
	}
		break;
	case 10:
	{
		if (m_bFirstInRoom10)
		{
			if (XMVectorGetX(m_pPlayer->Get_Position()) > 40.f)
			{
				if (m_pPlayer->Get_Fsm()->Get_CurrentState() != CLink::IDLE)
					m_pPlayer->Change_State(CLink::IDLE);

				m_fTimer += fTimeDelta;

				if (m_fTimer > 0.5f)
				{
					m_pPlayerCam->Zoom_In(1.1f, 60.f);

					pLayer = m_pGameInstance->Find_Layer(LEVEL_DUNGEON, TEXT("Layer_OnewayDoorReverse"));

					for (auto iter = pLayer->Get_ObjectList().begin(); iter != pLayer->Get_ObjectList().end(); iter++)
					{
						if (static_cast<CGameObject*>(*iter)->Get_RoomNum() == m_iCurRoomNum)
							static_cast<COnewayDoorReverse*>(*iter)->Set_Open(false);

					}

					m_bFirstInRoom10 = false;
				}
			}
		}
		else
		{


			CRola* m_pRola = static_cast<CRola*>(m_pGameInstance->Find_Object(LEVEL_DUNGEON, TEXT("Layer_Rola"), 0));

			if (m_pRola->Get_Dead() == false)
				m_fTimer += fTimeDelta;

			if (m_pRola->Get_Dead() == false && m_fTimer > 3.f)
			{
				//컷씬 기다리다 줌아웃
				if (m_bCamZoomOut == false)
				{
					m_bCamZoomOut = true;
					m_pPlayerCam->Zoom_Out(1.1f, 60.f);
				}

				//패턴 시작
				CRola* m_pRola = static_cast<CRola*>(m_pGameInstance->Find_Object(LEVEL_DUNGEON, TEXT("Layer_Rola"), 0));
				m_fTimer += fTimeDelta;

				if (m_pRola->Get_Dead() == false && m_fTimer > 1.f && m_bChageBGM == false)
				{
					m_bChageBGM = true;
					m_pGameInstance->Play_BGM(TEXT("0_DungeonBoss_Middle.wav"), 0.8f);
					m_pRola->Change_State(CRola::PUSH);
				}
			}
			else if (m_pRola->Get_Dead()) //Rola죽으면
			{
				m_fRolaDeadTimer += fTimeDelta;

				if (m_bRestartBgm == false && m_fRolaDeadTimer > 1.f)	//문열고 bgm전환
				{
					pLayer = m_pGameInstance->Find_Layer(LEVEL_DUNGEON, TEXT("Layer_OnewayDoorReverse"));

					for (auto iter = pLayer->Get_ObjectList().begin(); iter != pLayer->Get_ObjectList().end(); iter++)
					{
						if (static_cast<CGameObject*>(*iter)->Get_RoomNum() == m_iCurRoomNum)
							static_cast<COnewayDoorReverse*>(*iter)->Set_Open(true);
					}

					m_bRestartBgm = true;
					m_pGameInstance->Play_BGM(TEXT("0_Dangeon1_TailCave"), 0.7f);
				}

				//필요한 연출 끝나고 줌 아웃
				if (m_fRolaDeadTimer > 2.5f && m_bCamZoomOut_Room10 == false)
				{
					m_bCamZoomOut_Room10 = true;
					static_cast<CPlayerCamera*>(m_pGameInstance->Find_Camera(LEVEL_DUNGEON))->Zoom_Out(1.1f, 60.f);
				}
			}
		}
	}
		break;
	case 12:
	{
		CDeguTail_00* pDeguTail = static_cast<CDeguTail_00*>(m_pGameInstance->Find_Object(LEVEL_DUNGEON, TEXT("Layer_DeguTail"), 0));

		if (m_iCurRoomNum == 12)
		{
			if (m_bFirstInRoom12)
			{
				if (XMVectorGetZ(m_pPlayer->Get_Position()) > 43.f)
				{

					m_pPlayer->Change_State(CLink::IDLE);

					m_fTimer += fTimeDelta;

					if (m_fTimer > 0.1f)
					{
						pDeguTail->SetActive(true);
						m_bFirstInRoom12 = false;
					}
				}
			}
		}
		else
			pDeguTail->SetActive(false);
	}
		break;
	case 14:
	{
		CSquareBlock* pBlock = static_cast<CSquareBlock*>(m_pGameInstance->Find_Object(LEVEL_DUNGEON, TEXT("Layer_SquareBlock_Gimmick"), 0));

		if (m_iCurRoomNum == 14)
		{
			if (pBlock->Get_isStopMove() && pBlock->Get_IsPushed())
			{
				pLayer = m_pGameInstance->Find_Layer(LEVEL_DUNGEON, TEXT("Layer_OnewayDoorReverse"));

				for (auto iter = pLayer->Get_ObjectList().begin(); iter != pLayer->Get_ObjectList().end(); iter++)
				{
					if (static_cast<CGameObject*>(*iter)->Get_RoomNum() == m_iCurRoomNum)
					{
						if (static_cast<COnewayDoorReverse*>(*iter)->Get_Opened() == false)
							static_cast<COnewayDoorReverse*>(*iter)->Set_Open(true);
					}

				}
			}
		}

	}
		break;
	case 15:
	{
		pLayer = m_pGameInstance->Find_Layer(LEVEL_DUNGEON, TEXT("Layer_Togezo"));

		_int iAliveCount = 0;

		for (auto iter = pLayer->Get_ObjectList().begin(); iter != pLayer->Get_ObjectList().end(); iter++)
		{
			if (static_cast<CGameObject*>(*iter)->Get_RoomNum() == m_iCurRoomNum)
			{
				if (static_cast<CMonster*>(*iter)->Get_Dead() == false)
					iAliveCount++;
			}

		}

		if (iAliveCount == 0)
		{
			pLayer = m_pGameInstance->Find_Layer(LEVEL_DUNGEON, TEXT("Layer_TreasureBox"));

			for (auto iter = pLayer->Get_ObjectList().begin(); iter != pLayer->Get_ObjectList().end(); iter++)
			{
				if (static_cast<CGameObject*>(*iter)->Get_RoomNum() == 15)
				{
					static_cast<CTreasureBox*>(*iter)->Set_bShow(true);
					static_cast<CTreasureBox*>(*iter)->SetActive(true);
				}
			}
		}
	}
		break;
	default:
		break;
	}
//#pragma region ROOM_2
//	if (m_iCurRoomNum == 2)
//	{
//		CGameObject* pKey = m_pGameInstance->Find_Object(LEVEL_DUNGEON, TEXT("Layer_SmallKey"), 0);
//		if(pKey->Get_Dead() == false)
//		{
//			int iCountAlive = 0;
//
//			pLayer = m_pGameInstance->Find_Layer(LEVEL_DUNGEON, TEXT("Layer_Pawn"));
//			for (auto iter = pLayer->Get_ObjectList().begin(); iter != pLayer->Get_ObjectList().end(); iter++)
//			{
//				if (static_cast<CGameObject*>(*iter)->Get_RoomNum() == m_iCurRoomNum)
//				{
//					if (static_cast<CGameObject*>(*iter)->Get_Dead() == false)
//						iCountAlive++;
//				}
//			}
//
//			if (iCountAlive == 0)
//				pKey->SetActive(true);
//		}
//	}
//	else
//	{
//		CGameObject* pKey = m_pGameInstance->Find_Object(LEVEL_DUNGEON, TEXT("Layer_SmallKey"), 0);
//		pKey->SetActive(false);
//	}
//
//#pragma endregion

//#pragma region ROOM_4
//	if (m_iCurRoomNum == 4)
//	{
//		CFootSwitch* pSwitch = static_cast<CFootSwitch*>(m_pGameInstance->Find_Object(LEVEL_DUNGEON, TEXT("Layer_FootSwitch"), 0));
//		if (pSwitch->Get_IsOn())
//		{
//			pLayer = m_pGameInstance->Find_Layer(LEVEL_DUNGEON, TEXT("Layer_TreasureBox"));
//
//			for (auto iter = pLayer->Get_ObjectList().begin(); iter != pLayer->Get_ObjectList().end(); iter++)
//			{
//				if (static_cast<CGameObject*>(*iter)->Get_RoomNum() == m_iCurRoomNum)
//				{
//					static_cast<CTreasureBox*>(*iter)->Set_bShow(true);
//					static_cast<CTreasureBox*>(*iter)->SetActive(true);
//				}
//			}
//		}
//		
//	}
//#pragma endregion

//#pragma region ROOM_5
//	if (m_iCurRoomNum == 5)
//	{
//		CClosedPotDoor* pDoor = static_cast<CClosedPotDoor*>(m_pGameInstance->Find_Object(LEVEL_DUNGEON, TEXT("Layer_ClosedPotDoor"), 0));
//		if (pDoor->Get_Opend())
//		{
//			pLayer = m_pGameInstance->Find_Layer(LEVEL_DUNGEON, TEXT("Layer_TreasureBox"));
//
//			for (auto iter = pLayer->Get_ObjectList().begin(); iter != pLayer->Get_ObjectList().end(); iter++)
//			{
//				if (static_cast<CGameObject*>(*iter)->Get_RoomNum() == m_iCurRoomNum)
//				{
//					static_cast<CTreasureBox*>(*iter)->Set_bShow(true);
//					static_cast<CTreasureBox*>(*iter)->SetActive(true);
//				}
//			}
//		}
//
//	}
//#pragma endregion

//#pragma region ROOM_7
//	if (m_iCurRoomNum == 7)
//	{
//		pLayer = m_pGameInstance->Find_Layer(LEVEL_DUNGEON, TEXT("Layer_PurpleQuartz"));
//
//		_int iAliveCount = 0;
//
//		for (auto iter = pLayer->Get_ObjectList().begin(); iter != pLayer->Get_ObjectList().end(); iter++)
//		{
//			if (static_cast<CGameObject*>(*iter)->Get_RoomNum() == m_iCurRoomNum)
//			{
//				if (static_cast<CPurpleQuartz*>(*iter)->Get_isBreaked() == false)
//					iAliveCount++;
//			}
//			
//		}
//
//		if (iAliveCount == 0)
//		{
//			pLayer = m_pGameInstance->Find_Layer(LEVEL_DUNGEON, TEXT("Layer_TreasureBox"));
//
//			for (auto iter = pLayer->Get_ObjectList().begin(); iter != pLayer->Get_ObjectList().end(); iter++)
//			{
//				if (static_cast<CGameObject*>(*iter)->Get_CellNum() == 523)
//				{
//					static_cast<CTreasureBox*>(*iter)->Set_bShow(true);
//					static_cast<CTreasureBox*>(*iter)->SetActive(true);
//				}
//			}
//		}
//	}
//#pragma endregion

//#pragma region ROOM_9
//	if (m_iCurRoomNum == 9)
//	{
//		_int iTextureNum = -1;
//		_int iDemageCount = 0;
//		_bool bRestart = false;
//
//		pLayer = m_pGameInstance->Find_Layer(LEVEL_DUNGEON, TEXT("Layer_Vegas"));
//
//		for (auto iter = pLayer->Get_ObjectList().begin(); iter != pLayer->Get_ObjectList().end(); iter++)
//		{
//			if (static_cast<CVegas*>(*iter)->Get_Fsm()->Get_CurrentState() == CVegas::DAMAGE)
//				iDemageCount++;
//		}
//
//		if (iDemageCount == 3)	//전제가 다 멈춤 상태인지 확인
//		{
//			for (auto iter = pLayer->Get_ObjectList().begin(); iter != pLayer->Get_ObjectList().end(); iter++)
//			{
//				if (iTextureNum == -1)
//					iTextureNum = (int) static_cast<CVegas*>(*iter)->Get_TextureNum();
//				else
//				{
//					if ((int) static_cast<CVegas*>(*iter)->Get_TextureNum() != iTextureNum)
//						bRestart = true;	//모양이 하나라도 다르면 재시작
//				}
//			}
//		}
//
//		//문양에 따라 초기화 or 죽이기
//		if (bRestart)
//		{
//			for (auto iter = pLayer->Get_ObjectList().begin(); iter != pLayer->Get_ObjectList().end(); iter++)
//			{
//				static_cast<CVegas*>(*iter)->Get_Fsm()->Change_State(CVegas::IDLE);
//			}
//		}
//		else if(iDemageCount == 3 && bRestart == false)
//		{
//			for (auto iter = pLayer->Get_ObjectList().begin(); iter != pLayer->Get_ObjectList().end(); iter++)
//			{
//				static_cast<CVegas*>(*iter)->Get_Fsm()->Change_State(CVegas::DEAD);
//			}
//		}
//
//		//다 죽으면 상자 활성화
//		CVegas* pVegas = static_cast<CVegas*>(m_pGameInstance->Find_Object(LEVEL_DUNGEON, TEXT("Layer_Vegas"), 0));
//		if (pVegas->Get_Dead() == true)
//		{
//			pLayer = m_pGameInstance->Find_Layer(LEVEL_DUNGEON, TEXT("Layer_TreasureBox"));
//
//			for (auto iter = pLayer->Get_ObjectList().begin(); iter != pLayer->Get_ObjectList().end(); iter++)
//			{
//				if (static_cast<CGameObject*>(*iter)->Get_RoomNum() == m_iCurRoomNum)
//				{
//					static_cast<CTreasureBox*>(*iter)->Set_bShow(true);
//					static_cast<CTreasureBox*>(*iter)->SetActive(true);
//				}
//			}
//		}
//	}
//#pragma endregion

//#pragma region ROOM_10
//	if (m_iCurRoomNum == 10)
//	{
//		if (m_bFirstInRoom10)
//		{
//			if (XMVectorGetX(m_pPlayer->Get_Position()) > 40.f)
//			{
//				if(m_pPlayer->Get_Fsm()->Get_CurrentState() != CLink::IDLE)
//					m_pPlayer->Change_State(CLink::IDLE);
//
//				m_fTimer += fTimeDelta;
//
//				if(m_fTimer > 0.5f)
//				{
//					m_pPlayerCam->Zoom_In(1.1f, 60.f);
//
//					pLayer = m_pGameInstance->Find_Layer(LEVEL_DUNGEON, TEXT("Layer_OnewayDoorReverse"));
//
//					for (auto iter = pLayer->Get_ObjectList().begin(); iter != pLayer->Get_ObjectList().end(); iter++)
//					{
//						if (static_cast<CGameObject*>(*iter)->Get_RoomNum() == m_iCurRoomNum)
//							static_cast<COnewayDoorReverse*>(*iter)->Set_Open(false);
//
//					}
//
//					m_bFirstInRoom10 = false;
//				}
//			}
//		}
//		else
//		{
//			
//
//			CRola* m_pRola = static_cast<CRola*>(m_pGameInstance->Find_Object(LEVEL_DUNGEON, TEXT("Layer_Rola"), 0));
//			
//			if (m_pRola->Get_Dead() == false)
//				m_fTimer += fTimeDelta;
//
//			if (m_pRola->Get_Dead() == false && m_fTimer > 3.f)
//			{
//				//컷씬 기다리다 줌아웃
//				if(m_bCamZoomOut == false)
//				{
//					m_bCamZoomOut = true;
//					m_pPlayerCam->Zoom_Out(1.1f, 60.f);
//				}
//
//				//패턴 시작
//				CRola* m_pRola = static_cast<CRola*>(m_pGameInstance->Find_Object(LEVEL_DUNGEON, TEXT("Layer_Rola"), 0));
//				m_fTimer += fTimeDelta;
//
//				if (m_pRola->Get_Dead() == false && m_fTimer > 1.f && m_bChageBGM == false)
//				{
//					m_bChageBGM = true;
//					m_pGameInstance->Play_BGM(TEXT("0_DungeonBoss_Middle.wav"), 0.8f);
//					m_pRola->Change_State(CRola::PUSH);
//				}
//			}
//			else if(m_pRola->Get_Dead()) //Rola죽으면
//			{
//				m_fRolaDeadTimer += fTimeDelta;
//
//				if(m_bRestartBgm == false && m_fRolaDeadTimer > 1.f)	//문열고 bgm전환
//				{
//					pLayer = m_pGameInstance->Find_Layer(LEVEL_DUNGEON, TEXT("Layer_OnewayDoorReverse"));
//
//					for (auto iter = pLayer->Get_ObjectList().begin(); iter != pLayer->Get_ObjectList().end(); iter++)
//					{
//						if (static_cast<CGameObject*>(*iter)->Get_RoomNum() == m_iCurRoomNum)
//							static_cast<COnewayDoorReverse*>(*iter)->Set_Open(true);
//					}
//
//					m_bRestartBgm = true;
//					m_pGameInstance->Play_BGM(TEXT("0_Dangeon1_TailCave"), 0.7f);
//				}
//
//				//필요한 연출 끝나고 줌 아웃
//				if (m_fRolaDeadTimer > 2.5f && m_bCamZoomOut_Room10 == false)
//				{
//					m_bCamZoomOut_Room10 = true;
//					static_cast<CPlayerCamera*>(m_pGameInstance->Find_Camera(LEVEL_DUNGEON))->Zoom_Out(1.1f, 60.f);
//				}
//			}
//		}
//	}
//#pragma endregion

//#pragma region ROOM_12
//	CDeguTail_00* pDeguTail = static_cast<CDeguTail_00*>(m_pGameInstance->Find_Object(LEVEL_DUNGEON, TEXT("Layer_DeguTail"), 0));
//
//	if (m_iCurRoomNum == 12)
//	{
//		if (m_bFirstInRoom12)
//		{
//			if (XMVectorGetZ(m_pPlayer->Get_Position()) > 43.f)
//			{
//				
//				m_pPlayer->Change_State(CLink::IDLE);
//
//				m_fTimer += fTimeDelta;
//
//				if (m_fTimer > 0.1f)
//				{
//					pDeguTail->SetActive(true);
//					m_bFirstInRoom12 = false;
//				}
//			}
//		}
//	}
//	else
//		pDeguTail->SetActive(false);
//
//#pragma endregion

//#pragma region ROOM_14
//CSquareBlock* pBlock = static_cast<CSquareBlock*>(m_pGameInstance->Find_Object(LEVEL_DUNGEON, TEXT("Layer_SquareBlock_Gimmick"), 0));
//
//if (m_iCurRoomNum == 14)
//{
//	if (pBlock->Get_isStopMove() && pBlock->Get_IsPushed())
//	{
//		pLayer = m_pGameInstance->Find_Layer(LEVEL_DUNGEON, TEXT("Layer_OnewayDoorReverse"));
//
//		for (auto iter = pLayer->Get_ObjectList().begin(); iter != pLayer->Get_ObjectList().end(); iter++)
//		{
//			if (static_cast<CGameObject*>(*iter)->Get_RoomNum() == m_iCurRoomNum)
//			{
//				if (static_cast<COnewayDoorReverse*>(*iter)->Get_Opened() == false)
//					static_cast<COnewayDoorReverse*>(*iter)->Set_Open(true);
//			}
//
//		}
//	}
//}
//
//#pragma endregion

//#pragma region ROOM_15
//
//	if (m_iCurRoomNum == 15)
//	{
//		pLayer = m_pGameInstance->Find_Layer(LEVEL_DUNGEON, TEXT("Layer_Togezo"));
//
//		_int iAliveCount = 0;
//
//		for (auto iter = pLayer->Get_ObjectList().begin(); iter != pLayer->Get_ObjectList().end(); iter++)
//		{
//			if (static_cast<CGameObject*>(*iter)->Get_RoomNum() == m_iCurRoomNum)
//			{
//				if (static_cast<CMonster*>(*iter)->Get_Dead() == false)
//					iAliveCount++;
//			}
//
//		}
//
//		if (iAliveCount == 0)
//		{
//			pLayer = m_pGameInstance->Find_Layer(LEVEL_DUNGEON, TEXT("Layer_TreasureBox"));
//
//			for (auto iter = pLayer->Get_ObjectList().begin(); iter != pLayer->Get_ObjectList().end(); iter++)
//			{
//				if (static_cast<CGameObject*>(*iter)->Get_RoomNum() == 15)
//				{
//					static_cast<CTreasureBox*>(*iter)->Set_bShow(true);
//					static_cast<CTreasureBox*>(*iter)->SetActive(true);
//				}
//			}
//		}
//	}
//
//#pragma endregion
}

void CLevel_Dungeon::Set_MiniMap(_uint iRoomNum)
{
	for (auto& mapUI : m_pMiniMapUI->Get_ChildUIList())
	{
		if (dynamic_cast<CMapUI*>(mapUI) != nullptr)
		{
			if (dynamic_cast<CMapUI*>(mapUI)->Get_RoomNum() == iRoomNum)
				dynamic_cast<CMapUI*>(mapUI)->Set_Reveal();
		}
	}
}


CLevel_Dungeon* CLevel_Dungeon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Dungeon* pInstance = new CLevel_Dungeon(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_Dungeon"));
		Safe_Release(pInstance);
	}

	return pInstance;
}
void CLevel_Dungeon::Free()
{
	__super::Free();

	Safe_Release(m_pPlayer);
	Safe_Release(m_pMiniMapUI);
}