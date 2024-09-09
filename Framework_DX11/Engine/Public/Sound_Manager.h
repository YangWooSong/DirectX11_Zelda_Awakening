#pragma once
#include "Base.h"

#include "fmod.h"
#include "fmod.hpp"

#include <io.h>

#define SOUND_MAX 1.0f
#define SOUND_MIN 0.0f
#define SOUND_DEFAULT 0.5f
#define SOUND_WEIGHT 0.1f

BEGIN(Engine)

class ENGINE_DLL CSound_Manager final : public CBase
{
private:
	CSound_Manager();
	virtual ~CSound_Manager() = default;

public:
	map<TCHAR*, FMOD::Sound*>& Get_Sounds() { return m_Sounds; }

	FMOD::System* Get_System() { return m_pSystem; }

	void Set_Listener(class CGameObject* pListener) { m_pListenerObject = pListener; }

public:
	HRESULT Initialize();
	void	Update();

public:
	void Play_BGM(const TCHAR* pSoundKey, _float fVolume);
	void Stop_BGM();
	void Pause_BGM();
	void SetVolume_BGM(_float fVolume);

	void LoadSoundFile(const char* pFolderName);
	void Clear();

private:
	map<TCHAR*, FMOD::Sound*>::iterator Find_Sound(const TCHAR* pSoundKey);


private:
	// ���� ���ҽ� ������ ���� ��ü 
	map<TCHAR*, FMOD::Sound*> m_Sounds;

	// FMOD_CHANNEL : ����ϰ� �ִ� ���带 ������ ��ü 
	FMOD::Channel* m_pBGMChannel;

	// ���� ,ä�� ��ü �� ��ġ�� �����ϴ� ��ü 
	//FMOD::System* m_pSystem;
	FMOD::System* m_pSystem = { nullptr };

	class CGameObject* m_pListenerObject = { nullptr };
public:
	static CSound_Manager* Create();
	virtual void Free() override;
};

END