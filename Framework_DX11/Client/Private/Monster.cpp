#include "stdafx.h"
#include "Monster.h"
#include "GameInstance.h"
#include "PartObject.h"

CMonster::CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CContainerObject{ pDevice, pContext }
{
}


CMonster::CMonster(const CMonster& Prototype)
    : CContainerObject{ Prototype }
{
}

HRESULT CMonster::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CMonster::Initialize(void* pArg)
{
	MONSTER_DESC* pDesc = static_cast<MONSTER_DESC*>(pArg);

	/* 직교투영을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(&pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_iLevelIndex = pDesc->LevelIndex;
	m_iRoomNum = pDesc->iRoomNum;
	m_eObjType = pDesc->eType;

	//m_pTransformCom->Set_Scaled(pDesc->vScale.x, pDesc->vScale.y, pDesc->vScale.z);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&pDesc->vPosition));
	m_iCellNum = pDesc->iCellNum;

	

	return S_OK;
}

void CMonster::Priority_Update(_float fTimeDelta)
{
	for (auto& pPartObject : m_Parts)
		pPartObject->Priority_Update(fTimeDelta);

}

void CMonster::Update(_float fTimeDelta)
{

	for (auto& pPartObject : m_Parts)
		pPartObject->Update(fTimeDelta);

}

void CMonster::Late_Update(_float fTimeDelta)
{
	/* 직교투영을 위한 월드행렬까지 셋팅하게 된다. */
	for (auto& pPartObject : m_Parts)
		pPartObject->Late_Update(fTimeDelta);
	
}

HRESULT CMonster::Render()
{
	
    return S_OK;
}

HRESULT CMonster::Render_LightDepth()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldMatrix_Ptr())))
		return E_FAIL;

	_float4x4		ViewMatrix;
	XMStoreFloat4x4(&ViewMatrix, XMMatrixLookAtLH(m_pGameInstance->Get_ShadowLightPos_Vector(), m_pGameInstance->Get_LightLook_Vector(), XMVectorSet(0.f, 1.f, 0.f, 0.f)));
	
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pModelCom->Bind_MeshBoneMatrices(m_pShaderCom, "g_BoneMatrices", (_uint)i);

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", TEXTURE_TYPE::DIFFUSE, (_uint)i)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(6)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render((_uint)i)))
			return E_FAIL;
	}

	return S_OK;
}

void CMonster::Go_Straight(_float fTimeDelta, _float fSpeed, CNavigation* pNavigation, _bool* bMove)
{
	m_pTransformCom->Go_Straight(fTimeDelta, fSpeed, pNavigation, bMove);
}

void CMonster::Go_Straight_in_Room(_float fTimeDelta, _float fSpeed, CNavigation* pNavigation)
{
	m_pTransformCom->Go_Straight_InRoom(fTimeDelta, m_pNavigationCom->Get_CurrentCell_RoomNum(), fSpeed, pNavigation);
}

void CMonster::Go_Straight_in_Room_Reverse(_float fTimeDelta, _float fSpeed, _bool* bReflect, CNavigation* pNavigation, _int* iStopCount)
{
	m_pTransformCom->Go_Straight_InRoom_Reverse(fTimeDelta, m_pNavigationCom->Get_CurrentCell_RoomNum(), bReflect, fSpeed, pNavigation, iStopCount);
}

void CMonster::Change_State(const _uint iState)
{
	m_pFsmCom->Change_State(iState);
}

void CMonster::SetUp_NextAnimation(_uint iNextAnimationIndex, _float fChangeDuration, _bool _bLoop, _uint iStartFrame)
{
	m_pModelCom->SetUp_NextAnimation(iNextAnimationIndex, fChangeDuration, _bLoop, iStartFrame);
}

void CMonster::Set_AnimationSpeed(_uint _AnimationIndex, _double _dAnimSpeed)
{
	m_pModelCom->Set_AnimationSpeed(_AnimationIndex, _dAnimSpeed);
}

_bool CMonster::Get_IsEnd_CurrentAnimation()
{
	return m_pModelCom->Get_IsEnd_CurrentAnimation();
}

void CMonster::Set_NewRandom_Dir()
{
	_uint new_dir = (_uint)m_pGameInstance->Get_Random(0, 4);
	if (m_iDir != new_dir)
	{
		m_iDir = new_dir;
	}
	else
		Set_NewRandom_Dir();        //만약 기존 방향이랑 새 방향이랑 같으면 다시 랜덤값 돌림
}

_float CMonster::Get_Distance(_vector _Pos1, _vector _Pos2)
{
	_float3 Pos1, Pos2;

	XMStoreFloat3(&Pos1, _Pos1);
	XMStoreFloat3(&Pos2, _Pos2);

	_float3 vDir = { (Pos1.x - Pos2.x),(Pos1.y - Pos2.y),(Pos1.z - Pos2.z) };

	_float fLength = sqrt((vDir.x * vDir.x) + (vDir.y * vDir.y) + (vDir.z * vDir.z));

	return fLength;
}

_vector CMonster::Get_Pos_vector()
{
	return m_pTransformCom->Get_State(CTransform::STATE_POSITION);
}

_int CMonster::Get_CurrentCellNum()
{
	if (m_pNavigationCom == nullptr)
		return -1;

	return m_pNavigationCom->Get_CurrentCellIndex();
}

void CMonster::Effect2D_Active()
{
	if (m_pEffect != nullptr)
		m_pEffect->SetActive(true);
}

HRESULT CMonster::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

CMonster* CMonster::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{

	CMonster* pInstance = new CMonster(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CMonster"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMonster::Clone(void* pArg)
{
	CMonster* pInstance = new CMonster(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CMonster"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMonster::Free()
{
	__super::Free();

	Safe_Release(m_pEffect);
	Safe_Release(m_p3D_Effect);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pColliderCom);
}

