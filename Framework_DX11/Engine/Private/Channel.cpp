#include "Channel.h"
#include "Model.h"
#include "Bone.h"

CChannel::CChannel()
{
}

HRESULT CChannel::Initialize(HANDLE* pFile, const CModel* pModel)
{
	_ulong dwByte = 0;

	ReadFile(*pFile, &m_szName, MAX_PATH, &dwByte, nullptr);

	m_iBoneIndex = pModel->Get_BoneIndex(m_szName);

	/* 가장 큰 키 프레임을 찾음 */
	/* Assimp에서는 키 프레임이 이전 프레임과 큰 차이가 없으면 새로운 키 프레임을 생서하지 않음 (중간이 비어 있는 경우는 없음) */
	/* 따라서 가장 큰 키 프레임 기준으로 진행해야 함. */
	ReadFile(*pFile, &m_iNumKeyFrames, sizeof(_uint), &dwByte, nullptr);

	_float3			vScale = {};
	_float4			vRotation = {};
	_float3			vTranslation = {};

	for (size_t i = 0; i < m_iNumKeyFrames; i++)
	{
		KEYFRAME			KeyFrame{};

		ReadFile(*pFile, &KeyFrame, sizeof(KEYFRAME), &dwByte, nullptr);
		m_KeyFrames.push_back(KeyFrame);
	}

	m_CurrentKeyFrame = m_KeyFrames[0];

	return S_OK;
}

void CChannel::Update_TransformationMatrix(const vector<class CBone*>& Bones, _uint* pCurrentKeyFrameIndex, _double CurrentTrackPosition)
{
	if (0.0 == CurrentTrackPosition)
	{
		*pCurrentKeyFrameIndex = 0;
	}

	KEYFRAME	LastKeyFrame = m_KeyFrames.back();

	_vector		vScale, vRotation, vTranslation;

	/* 보간이 필요없는 경우 */
	if (CurrentTrackPosition >= LastKeyFrame.TrackPosition)
	{
		vScale = XMLoadFloat3(&LastKeyFrame.vScale);
		vRotation = XMLoadFloat4(&LastKeyFrame.vRotation);
		vTranslation = XMVectorSetW(XMLoadFloat3(&LastKeyFrame.vTranslation), 1.f);
	}

	/* 보간이 필요한 대부분 경우 */
	else
	{
		if (CurrentTrackPosition >= m_KeyFrames[*pCurrentKeyFrameIndex + 1].TrackPosition)
		{
			++(*pCurrentKeyFrameIndex);
			m_iNextKeyFrameIndex = (*pCurrentKeyFrameIndex + 1) % m_KeyFrames.size();
		}

		_vector		vSourScale = XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrameIndex].vScale);
		_vector		vDestScale = XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrameIndex + 1].vScale);

		_vector		vSourRotation = XMLoadFloat4(&m_KeyFrames[*pCurrentKeyFrameIndex].vRotation);
		_vector		vDestRotation = XMLoadFloat4(&m_KeyFrames[*pCurrentKeyFrameIndex + 1].vRotation);

		_vector		vSourTranslation = XMVectorSetW(XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrameIndex].vTranslation), 1.f);
		_vector		vDestTranslation = XMVectorSetW(XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrameIndex + 1].vTranslation), 1.f);

		/* 현재 트랙 위치와 다음 트랙 위치간의 비율을 구함 */
		/* (현재 위치 - 왼쪽 트랙 위치) / (오른쪽 트랙 위치 - 왼쪽 트랙 위치) == 전체 길이 */
		_double		Ratio = (CurrentTrackPosition - m_KeyFrames[*pCurrentKeyFrameIndex].TrackPosition) / (m_KeyFrames[*pCurrentKeyFrameIndex + 1].TrackPosition - m_KeyFrames[*pCurrentKeyFrameIndex].TrackPosition);

		vScale = XMVectorLerp(vSourScale, vDestScale, (_float)Ratio);
		vRotation = XMQuaternionSlerp(vSourRotation, vDestRotation, (_float)Ratio);
		vTranslation = XMVectorLerp(vSourTranslation, vDestTranslation, (_float)Ratio);
	}

	/* 이 뼈만의 상태행렬 = 회전기준은 당연히 부모기준.(자식뼈만의 상태를 만들때 원점기준으로 정의해야한다.) */

	_matrix			TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vTranslation);

	Bones[m_iBoneIndex]->Set_TransformationMatrix(TransformationMatrix);

	XMStoreFloat4(&m_CurrentKeyFrame.vRotation, vRotation);
	XMStoreFloat3(&m_CurrentKeyFrame.vScale, vScale);
	XMStoreFloat3(&m_CurrentKeyFrame.vTranslation, vTranslation);
	m_CurrentKeyFrame.TrackPosition = CurrentTrackPosition;
}

KEYFRAME CChannel::Get_KeyFrame(_uint iFrame)
{
	if (m_KeyFrames.size() > iFrame)
	{
		XMStoreFloat4(&m_CurrentKeyFrame.vRotation, XMLoadFloat4(&m_KeyFrames[iFrame].vRotation));
		XMStoreFloat3(&m_CurrentKeyFrame.vScale, XMLoadFloat3(&m_KeyFrames[iFrame].vScale));
		XMStoreFloat3(&m_CurrentKeyFrame.vTranslation, XMLoadFloat3(&m_KeyFrames[iFrame].vTranslation));
		m_CurrentKeyFrame.TrackPosition = m_KeyFrames[iFrame].TrackPosition;
	}
	else
	{
		XMStoreFloat4(&m_CurrentKeyFrame.vRotation, XMLoadFloat4(&m_KeyFrames[m_KeyFrames.size() - 1].vRotation));
		XMStoreFloat3(&m_CurrentKeyFrame.vScale, XMLoadFloat3(&m_KeyFrames[m_KeyFrames.size() - 1].vScale));
		XMStoreFloat3(&m_CurrentKeyFrame.vTranslation, XMLoadFloat3(&m_KeyFrames[m_KeyFrames.size() - 1].vTranslation));
		m_CurrentKeyFrame.TrackPosition = m_KeyFrames[m_KeyFrames.size() - 1].TrackPosition;
	}

	return m_CurrentKeyFrame;
}

CChannel* CChannel::Create(HANDLE* pFile, const CModel* pModel)
{
	CChannel* pInstance = new CChannel();

	if (FAILED(pInstance->Initialize(pFile, pModel)))
	{
		MSG_BOX(TEXT("Failed to Created : CChannel"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CChannel::Free()
{
}
