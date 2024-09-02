#include "stdafx.h"
#include "..\Public\Level_Logo.h"

#include "Level_Loading.h"

#include "GameInstance.h"
#include "BackGround.h"

CLevel_Logo::CLevel_Logo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel { pDevice, pContext }
{
}

HRESULT CLevel_Logo::Initialize()
{
	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	m_pGameInstance->Play_SoundRepeat(TEXT("0_Title_NoIntro.wav"), SOUND_BGM, 0.8f);
	return S_OK;
}

void CLevel_Logo::Update(_float fTimeDelta)
{
	if (GetKeyState(VK_RETURN) & 0x8000)
	{
		m_pGameInstance->Stop_All();
		if (FAILED(m_pGameInstance->Change_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_MARINHOUSE))))
			return;
	}
}

HRESULT CLevel_Logo::Render()
{
	SetWindowText(g_hWnd, TEXT("로고레벨입니다."));

	return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_BackGround(const _wstring& strLayerTag)
{
	CBackGround::BACKGROUND_DESC Desc { };
	

	Desc.eType = CBackGround::KEY;
	Desc.fSizeX = 400;
	Desc.fSizeY = 90;
	Desc.fX = g_iWinSizeX >> 1;
	Desc.fY = 570;

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_LOGO, strLayerTag,
		TEXT("Prototype_GameObject_BackGround"), &Desc)))
		return E_FAIL;

	Desc.eType = CBackGround::KEY_BACKGROUND;
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_LOGO, strLayerTag,
		TEXT("Prototype_GameObject_BackGround"), &Desc)))
		return E_FAIL;

	Desc.eType = CBackGround::BACKGROUND;
	Desc.fX = g_iWinSizeX >> 1;
	Desc.fY = g_iWinSizeY >> 1;
	Desc.fSizeX = g_iWinSizeX;
	Desc.fSizeY = g_iWinSizeY;

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_LOGO, strLayerTag,
		TEXT("Prototype_GameObject_BackGround"), &Desc)))
		return E_FAIL;

	return S_OK;
}

CLevel_Logo * CLevel_Logo::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Logo*		pInstance = new CLevel_Logo(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_Logo"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Logo::Free()
{
	__super::Free();

}
