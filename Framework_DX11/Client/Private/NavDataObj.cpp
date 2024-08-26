#include "stdafx.h"
#include "NavDataObj.h"
#include "GameInstance.h"

CNavDataObj::CNavDataObj(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}


CNavDataObj::CNavDataObj(const CNavDataObj& Prototype)
	: CGameObject{ Prototype }
{
}


HRESULT CNavDataObj::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CNavDataObj::Initialize(void* pArg)
{

	NAVOBJ_DESC* Desc{};
	Desc = static_cast<NAVOBJ_DESC*>(pArg);
	m_iLevelNum = Desc->iLevelNum;

	/* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(&pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	return S_OK;
}

void CNavDataObj::Priority_Update(_float fTimeDelta)
{
}

void CNavDataObj::Update(_float fTimeDelta)
{
}

void CNavDataObj::Late_Update(_float fTimeDelta)
{
	m_pNavigationCom->Update(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()));
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CNavDataObj::Render()
{
#ifdef _DEBUG
	if(m_pNavigationCom != nullptr)
		m_pNavigationCom->Render();
#endif

	return S_OK;
}

HRESULT CNavDataObj::Ready_Components()
{
	if(m_iLevelNum < LEVEL_END)
	{
		/* For.Com_Navigation */
		if (FAILED(__super::Add_Component(m_iLevelNum, TEXT("Prototype_Component_Navigation"),
			TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom))))
			return E_FAIL;
	}

	return S_OK;
}

CNavDataObj* CNavDataObj::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CNavDataObj* pInstance = new CNavDataObj(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CNavDataObj"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CNavDataObj::Clone(void* pArg)
{

	CNavDataObj* pInstance = new CNavDataObj(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CNavDataObj"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNavDataObj::Free()
{
	__super::Free();

	Safe_Release(m_pNavigationCom);
}
