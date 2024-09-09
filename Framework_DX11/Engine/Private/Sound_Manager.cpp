#include "Sound_Manager.h"
#include "Timer_Manager.h"

#include "GameObject.h"

CSound_Manager::CSound_Manager()
	:CBase{}
{
}

HRESULT CSound_Manager::Initialize()
{
	// ���带 ����ϴ� ��ǥ��ü�� �����ϴ� �Լ�
	FMOD::System_Create(&m_pSystem);

	m_pSystem->init(300, FMOD_INIT_NORMAL, nullptr);

	if (m_pSystem == nullptr)
		return E_FAIL;

	return S_OK;
}

void CSound_Manager::Update()
{
	if (nullptr != m_pListenerObject)
	{
		_matrix WorldMatrix = m_pListenerObject->Get_Transform()->Get_WorldMatrix();

		_vector vUp = XMVector3Normalize(WorldMatrix.r[1]);
		_vector vLook = XMVector3Normalize(WorldMatrix.r[2]);
		_vector vPos = WorldMatrix.r[3];

		FMOD_VECTOR ListenerPos = { XMVectorGetX(vPos), XMVectorGetY(vPos), XMVectorGetZ(vPos) };
		FMOD_VECTOR ListenerUp = { XMVectorGetX(vUp), XMVectorGetY(vUp), XMVectorGetZ(vUp) };
		FMOD_VECTOR ListenerLook = { XMVectorGetX(vLook), XMVectorGetY(vLook), XMVectorGetZ(vLook) };

		m_pSystem->set3DListenerAttributes(0, &ListenerPos, 0, &ListenerUp, &ListenerLook);
	}

	m_pSystem->update();
}

void CSound_Manager::Play_BGM(const TCHAR* pSoundKey, _float fVolume)
{
	auto iter = Find_Sound(pSoundKey);

	if (iter == m_Sounds.end())
		return;

	//FMOD_System_PlaySound(m_pSystem, FMOD_CHANNEL_FREE, iter->second, FALSE, &m_pChannelArr[SOUND_BGM]);
	m_pSystem->playSound(iter->second, 0, false, &m_pBGMChannel);

	//FMOD_Channel_SetMode(m_pChannelArr[SOUND_BGM], FMOD_LOOP_NORMAL);
	m_pBGMChannel->setMode(FMOD_LOOP_NORMAL);

	//FMOD_Channel_SetVolume(m_pChannelArr[SOUND_BGM], fVolume);
	m_pBGMChannel->setVolume(fVolume);
}

void CSound_Manager::Stop_BGM()
{
	m_pBGMChannel->stop();
}

void CSound_Manager::Pause_BGM()
{
	_bool isPause;
	m_pBGMChannel->getPaused(&isPause);
	m_pBGMChannel->setPaused(!isPause);
}

void CSound_Manager::SetVolume_BGM(_float fVolume)
{
	m_pBGMChannel->setVolume(fVolume);
}

void CSound_Manager::LoadSoundFile(const char* pFolderName)
{
	// _finddata_t : <io.h>���� �����ϸ� ���� ������ �����ϴ� ����ü
	_finddata_t fd;

	char path[MAX_PATH] = "../../Client/Bin/Sound/";
	strcat_s(path, MAX_PATH, pFolderName);
	strcat_s(path, MAX_PATH, "/*");

	// _findfirst : <io.h>���� �����ϸ� ����ڰ� ������ ��� ������ ���� ù ��° ������ ã�� �Լ�
	intptr_t handle = _findfirst(path, &fd);

	if (handle == -1)
		return;

	int iResult = 0;

	char szCurPath[MAX_PATH] = "../../Client/Bin/Sound/";
	char szFullPath[MAX_PATH] = "";

	while (iResult != -1)
	{
		strcpy_s(szFullPath, szCurPath);
		strcat_s(szFullPath, pFolderName);
		strcat_s(szFullPath, MAX_PATH, "/");
		strcat_s(szFullPath, fd.name);

		FMOD::Sound* pSound = nullptr;

		//FMOD_RESULT eRes = FMOD_System_CreateSound(m_pSystem, szFullPath, FMOD_HARDWARE, 0, &pSound);
		FMOD_RESULT eRes = m_pSystem->createSound(szFullPath, FMOD_LOOP_OFF | FMOD_3D, 0, &pSound);

		if (eRes == FMOD_OK)
		{
			_int iLength = _int(strlen(fd.name) + 1);

			TCHAR* pSoundKey = new TCHAR[iLength];
			ZeroMemory(pSoundKey, sizeof(TCHAR) * iLength);

			// �ƽ�Ű �ڵ� ���ڿ��� �����ڵ� ���ڿ��� ��ȯ�����ִ� �Լ�
			MultiByteToWideChar(CP_ACP, 0, fd.name, iLength, pSoundKey, iLength);

			pSound->set3DMinMaxDistance(8.f, 30.f);
			m_Sounds.emplace(pSoundKey, pSound);
		}
		//_findnext : <io.h>���� �����ϸ� ���� ��ġ�� ������ ã�� �Լ�, ���̻� ���ٸ� -1�� ����
		iResult = _findnext(handle, &fd);
	}

	_findclose(handle);
}

void CSound_Manager::Clear()
{
	for (auto& Pair : m_Sounds)
	{
		delete[] Pair.first;

		//FMOD_Sound_Release(Mypair.second);
		Pair.second->release();
	}
	m_Sounds.clear();
}

map<TCHAR*, FMOD::Sound*>::iterator CSound_Manager::Find_Sound(const TCHAR* pSoundKey)
{
	map<TCHAR*, FMOD::Sound*>::iterator iter;

	// iter = find_if(m_mapSound.begin(), m_mapSound.end(), CTag_Finder(pSoundKey));
	iter = find_if(m_Sounds.begin(), m_Sounds.end(), [&](auto& iter)->bool
		{
			return !lstrcmp(pSoundKey, iter.first);
		});

	return iter;
}

CSound_Manager* CSound_Manager::Create()
{
	CSound_Manager* pInstance = new CSound_Manager();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CSound_Manager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSound_Manager::Free()
{
	for (auto& Pair : m_Sounds)
	{
		delete[] Pair.first;

		//FMOD_Sound_Release(Mypair.second);
		Pair.second->release();
	}
	m_Sounds.clear();

	m_pSystem->close();
	m_pSystem->release();
}
