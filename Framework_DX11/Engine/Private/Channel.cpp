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

	/* ���� ū Ű �������� ã�� */
	/* Assimp������ Ű �������� ���� �����Ӱ� ū ���̰� ������ ���ο� Ű �������� �������� ���� (�߰��� ��� �ִ� ���� ����) */
	/* ���� ���� ū Ű ������ �������� �����ؾ� ��. */
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

	/* ������ �ʿ���� ��� */
	if (CurrentTrackPosition >= LastKeyFrame.TrackPosition)
	{
		vScale = XMLoadFloat3(&LastKeyFrame.vScale);
		vRotation = XMLoadFloat4(&LastKeyFrame.vRotation);
		vTranslation = XMVectorSetW(XMLoadFloat3(&LastKeyFrame.vTranslation), 1.f);
	}

	/* ������ �ʿ��� ��κ� ��� */
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

		/* ���� Ʈ�� ��ġ�� ���� Ʈ�� ��ġ���� ������ ���� */
		/* (���� ��ġ - ���� Ʈ�� ��ġ) / (������ Ʈ�� ��ġ - ���� Ʈ�� ��ġ) == ��ü ���� */
		_double		Ratio = (CurrentTrackPosition - m_KeyFrames[*pCurrentKeyFrameIndex].TrackPosition) / (m_KeyFrames[*pCurrentKeyFrameIndex + 1].TrackPosition - m_KeyFrames[*pCurrentKeyFrameIndex].TrackPosition);

		vScale = XMVectorLerp(vSourScale, vDestScale, (_float)Ratio);
		vRotation = XMQuaternionSlerp(vSourRotation, vDestRotation, (_float)Ratio);
		vTranslation = XMVectorLerp(vSourTranslation, vDestTranslation, (_float)Ratio);
	}

	/* �� ������ ������� = ȸ�������� �翬�� �θ����.(�ڽĻ����� ���¸� ���鶧 ������������ �����ؾ��Ѵ�.) */

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
