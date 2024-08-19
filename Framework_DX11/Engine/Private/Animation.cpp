#include "Animation.h"
#include "Channel.h"

CAnimation::CAnimation()
{
}

HRESULT CAnimation::Initialize(HANDLE* pFile, vector<_uint>& KeyFrameIndices, const CModel* pModel)
{
	_ulong dwByte = 0;

	ReadFile(*pFile, &m_szName, MAX_PATH, &dwByte, nullptr);

	ReadFile(*pFile, &m_Duration, sizeof(_double), &dwByte, nullptr);
	ReadFile(*pFile, &m_SpeedPerSec, sizeof(_double), &dwByte, nullptr);

	/* �� �ִϸ��̼��� ����ϴ� ���� ������ �����Ѵ�. */
	ReadFile(*pFile, &m_iNumChannels, sizeof(_uint), &dwByte, nullptr);

	KeyFrameIndices.resize(m_iNumChannels);

	for (size_t i = 0; i < m_iNumChannels; ++i)
	{
		CChannel* pChannel = CChannel::Create(pFile, pModel);
		if (nullptr == pChannel)
			return E_FAIL;

		m_Channels.emplace_back(pChannel);
	}

	return S_OK;
}

void CAnimation::Update_TransformationMatrices(const vector<class CBone*>& Bones, _double* pCurrentTrackPosition, vector<_uint>& CurrentKeyFrameIndices, _bool isLoop, _float fTimeDelta)
{
	/* ���� �����ġ�� ����ϳ�. */
	*pCurrentTrackPosition += m_SpeedPerSec * fTimeDelta;

	if (*pCurrentTrackPosition >= m_Duration)
	{
		if (false == isLoop)
		{
			m_isEnd = true;
			return;
		}

		else
			*pCurrentTrackPosition = 0.f;

	}

	/* ���� �����ġ�� �°� ���� �ִϸ��̼��� ��Ʈ���ؾ� �� ���� ���µ��� �������ش�. */
	_uint	iChannelIndex = { 0 };
	for (auto& pChannel : m_Channels)
	{
		pChannel->Update_TransformationMatrix(Bones, &CurrentKeyFrameIndices[iChannelIndex++], *pCurrentTrackPosition);
	}

}

void CAnimation::Reset()
{
	m_isEnd = false;
}

CAnimation* CAnimation::Create(HANDLE* pFile, vector<_uint>& KeyFrameIndices, const CModel* pModel)
{
	CAnimation* pInstance = new CAnimation();

	if (FAILED(pInstance->Initialize(pFile, KeyFrameIndices, pModel)))
	{
		MSG_BOX(TEXT("Failed to Created : CAnimation"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAnimation::Free()
{
	__super::Free();

	for (auto& pChannel : m_Channels)
		Safe_Release(pChannel);
	m_Channels.clear();
}
