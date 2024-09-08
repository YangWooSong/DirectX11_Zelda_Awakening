#include "stdafx.h"
#include "..\Public\Level_Field.h"
#include "Level_Loading.h"
#include "GameInstance.h"
#include "FreeCamera.h"
#include "PlayerCamera.h"
#include "AnimModel.h"
#include "Link.h"
#include <fstream>
#include "NavDataObj.h"
#include "Monster.h"

CLevel_Field::CLevel_Field(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_Field::Initialize()
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

	m_pGameInstance->Play_SoundRepeat(TEXT("0_Field_Normal.wav"), SOUND_BGM, 0.6f);

	return S_OK;
}

void CLevel_Field::Update(_float fTimeDelta)
{
	if (GetKeyState(VK_RETURN) & 0x8000)
	{
		m_pGameInstance->DeletePlayer();
		m_pGameInstance->DeleteActors();
		m_pGameInstance->Stop_All();
		if (FAILED(m_pGameInstance->Change_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_DUNGEON))))
			return;
	}
}

HRESULT CLevel_Field::Render()
{
	SetWindowText(g_hWnd, TEXT("Field 레벨입니다."));
	return S_OK;
}

HRESULT CLevel_Field::Ready_Lights()
{
	/* 게임플레이 레벨에 필요한 광원을 준비한다. */
	return S_OK;
}

HRESULT CLevel_Field::Ready_Layer_Camera()
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

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_FIELD, TEXT("Layer_Camera"), TEXT("Prototype_GameObject_FreeCamera"), &Desc)))
		return E_FAIL;

	//CPlayerCamera::CAMERA_PLAYER_DESC	Desc{};

	//Desc.vEye = _float4(0.f, 10.f, -10.f, 1.f);
	//Desc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	//Desc.fFovy = XMConvertToRadians(60.0f);
	//Desc.fAspect = g_iWinSizeX / g_iWinSizeY;
	//Desc.fNear = 0.1f;
	//Desc.fFar = 1000.f;
	//Desc.fSpeedPerSec = 30.f;
	//Desc.fRotationPerSec = XMConvertToRadians(90.0f);

	//Desc.fSpeed = 4.f;
	//Desc.pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Find_Player(LEVEL_MARINHOUSE));

	//if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_FIELD, TEXT("Layer_Camera"), TEXT("Prototype_GameObject_PlayerCamera"), &Desc)))
	//	return E_FAIL;

	return S_OK;
}


HRESULT CLevel_Field::Ready_Layer_BackGround()
{
	//if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_BackGround"), TEXT("Prototype_GameObject_Terrain"))))
		//return E_FAIL;

	//if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_BackGround"), TEXT("Prototype_GameObject_Sky"))))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Field::Ready_Layer_Effect()
{
	//for (size_t i = 0; i < 50; i++)
	//{
	//	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_BackGround"), TEXT("Prototype_GameObject_Effect"))))
	//		return E_FAIL;
	//}


	return S_OK;
}

HRESULT CLevel_Field::Ready_LandObjects()
{

	CPlayer::PLAYER_DESC PlayerDesc;
	PlayerDesc.vPosition = _float3(3.f, 0.f, 3.f);
	PlayerDesc.LevelIndex = LEVEL_FIELD;
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_FIELD, TEXT("Layer_Player"), TEXT("Prototype_GameObject_Player_Link"), &PlayerDesc)))
		return E_FAIL;
	
	CNavDataObj::NAVOBJ_DESC NavDes;
	NavDes.iLevelNum = LEVEL_FIELD;
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_FIELD, TEXT("Layer_NavDataObj"), TEXT("Prototype_GameObject_NavDataObj"), &NavDes)))
		return E_FAIL;

	return S_OK;
}

//HRESULT CLevel_Field::Ready_Layer_Player(CLandObject::LANDOBJECT_DESC& LandObjectDesc)
//{
//	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Prototype_GameObject_Player"), &LandObjectDesc)))
//		return E_FAIL;
//
//	return S_OK;
//}

//
//HRESULT CLevel_Field::Ready_Layer_Monster(CLandObject::LANDOBJECT_DESC& LandObjectDesc)
//{
//	for (size_t i = 0; i < 20; i++)
//	{
//		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Monster"), &LandObjectDesc)))
//			return E_FAIL;
//	}
//
//	return S_OK;
//}

HRESULT CLevel_Field::Read()
{
	const char cFile[50] = "../Bin/Map_Data/Field.dat";
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

HRESULT CLevel_Field::Read_LandObjects(_int _type, _uint _index, _float3 _fPos, _float3 _fScaled, _float3 _fRot)
{
	CGameObject::GAMEOBJECT_DESC pDesc = { };
	pDesc.eType = static_cast<CGameObject::OBJ_TYPE>(_type);
	pDesc.listIndex = _index;

	pDesc.vPosition = _fPos;
	pDesc.vScale = _fScaled;
	pDesc.vRotation = _fRot;
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_FIELD, TEXT("Layer_Land"), TEXT("Prototype_GameObject_Land"), &pDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Field::Read_AnimMonster(_int _type, _uint _index, _float3 _fPos, _float3 _fScaled, _float3 _fRot, string _strLyaerTag, _int _iCellNum)
{
	CMonster::MONSTER_DESC pDesc = { };
	//pDesc.eType = static_cast<CGameObject::OBJ_TYPE>(_type);
	//pDesc.listIndex = _index;

	pDesc.vPosition = _fPos;
	pDesc.vScale = _fScaled;
	pDesc.vRotation = _fRot;
	pDesc.LevelIndex = LEVEL_FIELD;
	pDesc.iCellNum = _iCellNum;
	pDesc.eType = CGameObject::ANIM_MONSTER;

	if (_strLyaerTag == "Layer_SeaUrchin")
	{
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_FIELD, TEXT("Layer_SeaUrchin"), TEXT("Prototype_GameObject_SeaUrchin"), &pDesc)))
			return E_FAIL;
	}
	else if (_strLyaerTag == "Layer_Octorok")
	{
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_FIELD, TEXT("Layer_Octorok"), TEXT("Prototype_GameObject_Octorok"), &pDesc)))
			return E_FAIL;
	}
	return S_OK;
}

HRESULT CLevel_Field::Read_AnimObj(_int _type, _uint _index, _float3 _fPos, _float3 _fScaled, _float3 _fRot, string _strLyaerTag, _int _iCellNum)
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
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_FIELD, TEXT("Layer_Weathercock"), TEXT("Prototype_GameObject_Weathercock"), &pDesc)))
			return E_FAIL;
	}
	else if (_strLyaerTag == "Layer_Tree")
	{
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_FIELD, TEXT("Layer_Tree"), TEXT("Prototype_GameObject_Tree"), &pDesc)))
			return E_FAIL;
	}

	else if (_strLyaerTag == "Layer_TailLockStatue")
	{
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_FIELD, TEXT("Layer_TailLockStatue"), TEXT("Prototype_GameObject_TailLockStatue"), &pDesc)))
			return E_FAIL;
	}

	else if (_strLyaerTag == "Layer_TailCaveShutter")
	{
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_FIELD, TEXT("Layer_TailCaveShutter"), TEXT("Prototype_GameObject_TailCaveShutter"), &pDesc)))
			return E_FAIL;
	}
	return S_OK;
}

HRESULT CLevel_Field::Read_NonAnimObj(_int _type, _uint _index, _float3 _fPos, _float3 _fScaled, _float3 _fRot, string _strLyaerTag)
{
	CGameObject::GAMEOBJECT_DESC pDesc = { };
	pDesc.eType = static_cast<CGameObject::OBJ_TYPE>(_type);
	pDesc.listIndex = _index;

	pDesc.vPosition = _fPos;
	pDesc.vScale = _fScaled;
	pDesc.vRotation = _fRot;

	if (_strLyaerTag == "Layer_Lawn")
	{
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_FIELD, TEXT("Layer_Lawn"), TEXT("Prototype_GameObject_Lawn"), &pDesc)))
			return E_FAIL;
	}
	else if (_strLyaerTag == "Layer_Grass")
	{
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_FIELD, TEXT("Layer_Grass"), TEXT("Prototype_GameObject_Grass"), &pDesc)))
			return E_FAIL;
	}
	return S_OK;
}

CLevel_Field* CLevel_Field::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Field* pInstance = new CLevel_Field(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_Field"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Field::Free()
{
	__super::Free();

}
