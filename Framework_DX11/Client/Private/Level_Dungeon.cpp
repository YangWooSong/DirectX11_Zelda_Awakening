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

    return S_OK;
}


void CLevel_Dungeon::Update(_float fTimeDelta)
{
}

HRESULT CLevel_Dungeon::Render()
{
    SetWindowText(g_hWnd, TEXT("Dungoen 레벨입니다."));
    return S_OK;
}

HRESULT CLevel_Dungeon::Ready_Lights()
{
    return S_OK;
}

HRESULT CLevel_Dungeon::Ready_Layer_Camera()
{
    CFreeCamera::CAMERA_FREE_DESC		Desc{};

    Desc.fSensor = 0.2f;
    Desc.vEye = _float4(0.f, 10.f, -10.f, 1.f);
    Desc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
    Desc.fFovy = XMConvertToRadians(60.0f);
    Desc.fAspect = (_float)g_iWinSizeX / (_float)g_iWinSizeY;
    Desc.fNear = 0.1f;
    Desc.fFar = 1000.f;
    Desc.fSpeedPerSec = 30.f;
    Desc.fRotationPerSec = XMConvertToRadians(90.0f);

    if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON, TEXT("Layer_Camera"), TEXT("Prototype_GameObject_FreeCamera"), &Desc)))
        return E_FAIL;
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
    CPlayer::PLAYER_DESC PlayerDesc;
    PlayerDesc.vPosition = _float3(0.f, 0.f, 0.f);
    PlayerDesc.LevelIndex = LEVEL_DUNGEON;
    if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON, TEXT("Layer_Player"), TEXT("Prototype_GameObject_Player_Link"), &PlayerDesc)))
        return E_FAIL;

	CNavDataObj::NAVOBJ_DESC NavDes;
	NavDes.iLevelNum = LEVEL_DUNGEON;
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON, TEXT("Layer_NavDataObj"), TEXT("Prototype_GameObject_NavDataObj"), &NavDes)))
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
			else if (iObjectType == CGameObject::ANIM_MONSTER)
				Read_AnimMonster(iObjectType, iObjectListIndex, fPos, fScaled, fRot, strLayerTag, iCellNum);
			else if (iObjectType == CGameObject::NONANIM_OBJ)
				Read_NonAnimObj(iObjectType, iObjectListIndex, fPos, fScaled, fRot, strLayerTag);
			else if (iObjectType == CGameObject::ANIM_OBJ)
				Read_AnimObj(iObjectType, iObjectListIndex, fPos, fScaled, fRot, strLayerTag, iCellNum);
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

HRESULT CLevel_Dungeon::Read_AnimMonster(_int _type, _uint _index, _float3 _fPos, _float3 _fScaled, _float3 _fRot, string _strLyaerTag, _int _iCellNum)
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

	if (_strLyaerTag == "Layer_DeguTail")
	{
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_DeguTail_00"), &pDesc)))
			return E_FAIL;
	}
	return S_OK;
}

HRESULT CLevel_Dungeon::Read_AnimObj(_int _type, _uint _index, _float3 _fPos, _float3 _fScaled, _float3 _fRot, string _strLyaerTag, _int _iCellNum)
{
	CGameObject::GAMEOBJECT_DESC pDesc = { };
	pDesc.eType = static_cast<CGameObject::OBJ_TYPE>(_type);
	pDesc.listIndex = _index;

	pDesc.vPosition = _fPos;
	pDesc.vScale = _fScaled;
	pDesc.vRotation = _fRot;
	pDesc.iCellNum = _iCellNum;

	if (_strLyaerTag == "Layer_Weathercock")
	{
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON, TEXT("Layer_Weathercock"), TEXT("Prototype_GameObject_Weathercock"), &pDesc)))
			return E_FAIL;
	}
	else if (_strLyaerTag == "Layer_Tree")
	{
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON, TEXT("Layer_Tree"), TEXT("Prototype_GameObject_Tree"), &pDesc)))
			return E_FAIL;
	}
	return S_OK;
}

HRESULT CLevel_Dungeon::Read_NonAnimObj(_int _type, _uint _index, _float3 _fPos, _float3 _fScaled, _float3 _fRot, string _strLyaerTag)
{
	CGameObject::GAMEOBJECT_DESC pDesc = { };
	pDesc.eType = static_cast<CGameObject::OBJ_TYPE>(_type);
	pDesc.listIndex = _index;

	pDesc.vPosition = _fPos;
	pDesc.vScale = _fScaled;
	pDesc.vRotation = _fRot;

	if (_strLyaerTag == "Layer_DungeonCollapseTileLv01")
	{
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON, TEXT("Layer_DungeonCollapseTileLv01"), TEXT("Prototype_GameObject_CollapseTile"), &pDesc)))
			return E_FAIL;
	}
	else if (_strLyaerTag == "Layer_DungeonCrackedWallLv01")
	{
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_DUNGEON, TEXT("Layer_DungeonCrackedWallLv01"), TEXT("Prototype_GameObject_CrackedWall"), &pDesc)))
			return E_FAIL;
	}
	return S_OK;
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

}