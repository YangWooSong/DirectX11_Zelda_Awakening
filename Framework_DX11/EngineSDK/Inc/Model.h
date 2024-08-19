#pragma once

#include "Component.h"
#include "Bone.h"

BEGIN(Engine)

class ENGINE_DLL CModel final : public CComponent
{
public:
	enum TYPE { TYPE_NONANIM, TYPE_ANIM, TYPE_END };

private:
	CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CModel(const CModel& Prototype);
	virtual ~CModel() = default;

public:
	vector<class CMesh*>&	Get_Meshes() { return m_Meshes; }

	_uint					Get_NumMeshes() const { return (_uint)m_Meshes.size(); }

	TYPE					Get_ModelType() const { return m_eType; }

	_uint					Get_BoneIndex(const _char* pBoneName) const;
	_matrix					Get_BoneCombindTransformationMatrix(_uint iBoneIndex) const { return m_Bones[iBoneIndex]->Get_CombinedTransformationMatrix(); }

	_uint					Get_AnimationIndex(const _char* pAnimationmName) const;
	_char*					Get_CurrentAnimationName();
	_bool					Get_IsEnd_CurrentAnimation();
	_double					Get_CurrentTrackPosition() { return m_CurrentTrackPosition; }
public:
	virtual HRESULT Initialize_Prototype(TYPE eType, const _char* pModelFilePath, _fmatrix PreTransformMatrix);
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Render(_uint iMeshIndex);

public:
	void Play_Animation(_float fTimeDelta);
	void Set_AnimationSpeed(_uint _AnimationIndex,_double _dAnimSpeed); //Anim재생 속도 변경
	void Reset_Animation(_uint iAnimationIndex);
public:
	HRESULT Bind_Material(class CShader* pShader, _char* pConstantName, TEXTURE_TYPE eMaterialType, _uint iMeshIndex);
	HRESULT Bind_MeshBoneMatrices(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex);

private:
	TYPE							m_eType = { TYPE_END };

private: /* 메시의 정보를 저장한다. */
	_uint							m_iNumMeshes = { 0 };
	vector<class CMesh*>			m_Meshes;
	_float4x4						m_PreTransformMatrix = {};

private:
	_uint							m_iNumMaterials = { 0 };
	vector<MESH_MATERIAL>			m_Materials;

private:
	vector<class CBone*>			m_Bones;

private:
	_bool							m_isLoop = { false };

	_uint							m_iCurrentAnimIndex = { 0 };
	_uint							m_iNumAnimations = { 0 };
	vector<class CAnimation*>		m_Animations;

	/* 현재 애니메이션의 재생 위치. */
	_double							m_CurrentTrackPosition = {};
	_double							m_ChangeTrackPosition = {};

	/* 각 애니메이션당 각 채널들의 현재 키프레임인덱스 */
	vector<vector<_uint>>			m_KeyFrameIndices;

	_bool							m_isChangeAni = { false };
	CHANGEANIMATION_DESC			m_tChangeDesc = {};

public:
	void		SetUp_Animation(_uint iAnimationIndex, _bool isLoop = false);
	void		Reset_CurrentAnimation();

	HRESULT     SetUp_NextAnimation(_uint iNextAnimationIndex, _float fChangeDuration = 0.2f, _bool _bLoop = false, _uint iStartFrame = 0);

public:
	HRESULT	Ready_Meshes(HANDLE* pFile);
	HRESULT Ready_Materials(HANDLE* pFile, const _char* pModelFilePath);
	HRESULT Ready_Bones(HANDLE* pFile, _int iParentBoneIndex);
	HRESULT Ready_Animations(HANDLE* pFile);

public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eType, const _char* pModelFilePath, _fmatrix PreTransformMatrix = XMMatrixIdentity());
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END