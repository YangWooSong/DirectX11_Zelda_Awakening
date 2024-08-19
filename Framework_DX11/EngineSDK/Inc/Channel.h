#pragma once

#include "Base.h"

BEGIN(Engine)

/* �ִϸ��̼� ������ ���� ����ϴ� �� �� �ϳ� */
/* �ð��� ���� ���� ������ ������ �ִ�. �ʱ� ������ ��� �ð��� �ٸ� ���°����� �̸� �����صд�. */
/* ���� ��Ÿ�Ӱ��� �� ä�ο��� ���� �ִϸ��̼��� ��� ��ġ�� �������ָ� �����ġ�� �´� �������(m_TransformationMatrix)�� �����. */

/* KeyFrame : Ư�� �����ġ���� �� ���� ���ؾ��� ����(���)�� ����(������)�Ͽ� ǥ���� ����ü.  */

class CChannel : public CBase
{
private:
	CChannel();
	virtual ~CChannel() = default;

public:
	_uint			Get_NetxKeyFrameIndex() { return m_iNextKeyFrameIndex; }

	_uint			Get_BoneIndex() { return m_iBoneIndex; }

	KEYFRAME		Get_CurrentKeyFrame() { return m_CurrentKeyFrame; }
	KEYFRAME		Get_StartKeyFrame() { return m_KeyFrames[0]; }
	void			Set_CurrnetTrackPosition(_double _dTrackPos) { m_CurrentKeyFrame.TrackPosition = _dTrackPos; }
	KEYFRAME		Get_KeyFrame(_uint iFrame);

public:
	HRESULT Initialize(HANDLE* pFile, const class CModel* pModel);
	void Update_TransformationMatrix(const vector<class CBone*>& Bones, _uint* pCurrentKeyFrameIndex, _double CurrentTrackPosition);


private:
	_char				m_szName[MAX_PATH] = "";

	_uint				m_iBoneIndex = { 0 };
	_uint				m_iNextKeyFrameIndex = { 0 };
	_uint				m_iNumKeyFrames = {};
	vector<KEYFRAME>	m_KeyFrames;

	KEYFRAME			m_CurrentKeyFrame;

public:
	static CChannel* Create(HANDLE* pFile, const class CModel* pModel);
	virtual void Free() override;

};

END