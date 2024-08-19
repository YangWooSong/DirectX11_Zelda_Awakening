#pragma once

#include "Base.h"

BEGIN(Engine)

class CAnimation final : public CBase
{
private:
	CAnimation();
	virtual ~CAnimation() = default;

public:
	_bool						Get_IsEnd() { return m_isEnd; }
	_char*						Get_Name() { return m_szName; }

	vector<class CChannel*>&	Get_Channels() { return m_Channels; }

public:
	HRESULT Initialize(HANDLE* pFile, vector<_uint>& KeyFrameIndices, const class CModel* pModel);
	void Update_TransformationMatrices(const vector<class CBone*>& Bones, _double* pCurrentTrackPosition, vector<_uint>& CurrentKeyFrameIndices, _bool isLoop, _float fTimeDelta);

public:
	void Reset();
	void Set_Speed(_double _dAnimationSpeed) { m_SpeedPerSec = _dAnimationSpeed; }	//Anim��� �ӵ� ����
	_double Get_Duration() { return m_Duration; }
private:
	_char						m_szName[MAX_PATH] = {};

	/* �� �ִϸ��̼��� ����ϴ� ���� ���� */
	_uint						m_iNumChannels = { 0 };

	/* ���� ������ ���� */
	/* Channel : �� (�ð��� ���� �� ���� ���� ����� �����Ѵ�) */
	vector<class CChannel*>		m_Channels;

	/* �� �ִϸ��̼��� �����ϱ����� �ɸ��� ��ü �Ÿ�. */
	_double						m_Duration = {};

	/* �ִϸ��̼��� �ʴ� ����ӵ� */
	_double						m_SpeedPerSec = {};

	_bool						m_isEnd = { false };

public:
	static CAnimation* Create(HANDLE* pFile, vector<_uint>& KeyFrameIndices, const class CModel* pModel);
	virtual void Free() override;
};

END