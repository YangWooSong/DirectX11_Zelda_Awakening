#include "..\Public\GameInstance.h"
#include "Graphic_Device.h"
#include "Input_Device.h"
#include "Level_Manager.h"
#include "Object_Manager.h"
#include "Timer_Manager.h"
#include "Picking_Manager.h"
#include "Font_Manager.h"

#include "Key_Manager.h"
#include "Sound_Manager.h"
#include "UI_Manager.h"

#include "PhysX_Manager.h"
#include "Event_Manager.h"
#include "Collider_Manager.h"
IMPLEMENT_SINGLETON(CGameInstance)

CGameInstance::CGameInstance()
{

}

HRESULT CGameInstance::Initialize_Engine(HINSTANCE hInst, _uint iNumLevels, const ENGINE_DESC& EngineDesc, _Inout_ ID3D11Device** ppDevice, _Inout_ ID3D11DeviceContext** ppContext)
{
	srand(unsigned(time(nullptr)));

	/* �׷��� ī�带 �ʱ�ȭ�ϳ�. */
	m_pGraphic_Device = CGraphic_Device::Create(EngineDesc.hWnd, EngineDesc.isWindowsed, EngineDesc.iWinSizeX, EngineDesc.iWinSizeY, ppDevice, ppContext);
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pTimer_Manager = CTimer_Manager::Create();
	if (nullptr == m_pTimer_Manager)
		return E_FAIL;

	m_pRenderer = CRenderer::Create(*ppDevice, *ppContext);
	if (nullptr == m_pRenderer)
		return E_FAIL;

	m_pPipeLine = CPipeLine::Create();
	if (nullptr == m_pPipeLine)
		return E_FAIL;

	/* ���� ī�带 �ʱ�ȭ�ϳ�. */

	/* �Է���ġ�� �ʱ�ȭ�ϳ�. */
	m_pInput_Device = CInput_Device::Create(hInst, EngineDesc.hWnd);
	if (nullptr == m_pInput_Device)
		return E_FAIL;

	m_pPicking_Manager = CPicking_Manager::Create(*ppDevice, *ppContext, EngineDesc.hWnd, EngineDesc.iWinSizeX, EngineDesc.iWinSizeY);
	if (nullptr == m_pPicking_Manager)
		return E_FAIL;

	/* �������� �Ŵ����� �ʱ�ȭ�ϳ�. */

	m_pLevel_Manager = CLevel_Manager::Create();
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	m_pObject_Manager = CObject_Manager::Create(iNumLevels);
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	m_pComponent_Manager = CComponent_Manager::Create(iNumLevels);
	if (nullptr == m_pComponent_Manager)
		return E_FAIL;

	m_pKey_Manager = CKey_Manager::Create(EngineDesc.hWnd);
	if (nullptr == m_pKey_Manager)
		return E_FAIL;

	m_pSound_Manager = CSound_Manager::Create();
	if (nullptr == m_pSound_Manager)
		return E_FAIL;

	m_pCollider_Manager = CCollider_Manager::Create();
	if (nullptr == m_pCollider_Manager)
		return E_FAIL;

	m_pUI_Manager = CUI_Manager::Create();
	if (nullptr == m_pUI_Manager)
		return E_FAIL;

	m_pPhysX_Manager = CPhysX_Manager::Create();
	if (nullptr == m_pPhysX_Manager)
		return E_FAIL;

	m_pEvent_Manager = CEvent_Manager::Create(m_pLevel_Manager, m_pObject_Manager, m_pUI_Manager); // << �̺�Ʈ�Ŵ����� �׻� �������� ���弼��
	if (nullptr == m_pEvent_Manager)
		return E_FAIL;


	m_pFont_Manager = CFont_Manager::Create(*ppDevice, *ppContext);
	if (nullptr == m_pFont_Manager)
		return E_FAIL;

	return S_OK;
}



void CGameInstance::Update_Engine(_float fTimeDelta)
{
	/* ���� Ű����� ���콺�� ���¸� �޾ƿò���. */
	m_pInput_Device->Update();

	m_pKey_Manager->Update();	// Ű �Ŵ����� ������Ʈ�� �׻� �� ó���� ������

	m_pObject_Manager->Priority_Update(fTimeDelta);	

	m_pPipeLine->Update();

	m_pPicking_Manager->Update();

	m_pObject_Manager->Update(fTimeDelta);
	
	m_pObject_Manager->Late_Update(fTimeDelta);

	m_pCollider_Manager->Update();

	m_pPhysX_Manager->PhysX_Update(fTimeDelta);

	//UIManager�� �׻� Render �Ǳ� ���� ȣ��
	m_pUI_Manager->Update();
	
	m_pLevel_Manager->Update(fTimeDelta);		
}

HRESULT CGameInstance::Draw_Engine()
{
	m_pRenderer->Draw();

	return m_pLevel_Manager->Render();
}

void CGameInstance::FinalUpdate_Engine()
{
	m_pEvent_Manager->Update();
}

HRESULT CGameInstance::Clear(_uint iLevelIndex)
{
	if (iLevelIndex == 0 )
		return S_OK;

	/* Ư�� ������ ���� �����ϴ� �ڿ����� �ش� ������ ������ ��������. */

	m_pObject_Manager->Clear(iLevelIndex);

	/* ������Ʈ �����鵵 �������� �����߾���. */
	m_pComponent_Manager->Clear(iLevelIndex);

	return S_OK;
}

POINT CGameInstance::Get_MosePos()
{
	POINT      ptMouse = {};
	GetCursorPos(&ptMouse);
	ScreenToClient(m_pGraphic_Device->Get_Handle(), &ptMouse);

	return ptMouse;
}

void CGameInstance::Render_Begin()
{
	/*m_pGraphic_Device->Render_Begin();*/
	m_pGraphic_Device->Clear_BackBuffer_View(_float4(0.f, 0.f, 0.f, 1.f));
	m_pGraphic_Device->Clear_DepthStencil_View();
	

}

void CGameInstance::Render_End()
{
	/*m_pGraphic_Device->Render_End(hWnd);*/

	m_pGraphic_Device->Present();
	
	
}

#pragma region INPUT_DEVICE

_byte CGameInstance::Get_DIKeyState(_ubyte byKeyID)
{
	return m_pInput_Device->Get_DIKeyState(byKeyID);
}

_byte CGameInstance::Get_DIMouseState(MOUSEKEYSTATE eMouse)
{
	return m_pInput_Device->Get_DIMouseState(eMouse);
}

_long CGameInstance::Get_DIMouseMove(MOUSEMOVESTATE eMouseState)
{
	return m_pInput_Device->Get_DIMouseMove(eMouseState);
}

#pragma endregion

#pragma region LEVEL_MANAGER

HRESULT CGameInstance::Change_Level(_uint iLevelIndex, CLevel * pNextLevel)
{
	return m_pLevel_Manager->Change_Level(iLevelIndex, pNextLevel);	
}

CLevel* CGameInstance::Get_CurLevel()
{
	return m_pLevel_Manager->Get_CurLevel();
}

_uint CGameInstance::Get_CurLevelIndex()
{
	return m_pLevel_Manager->Get_CurLevelIndex();
}

#pragma endregion

#pragma region OBJECT_MANAGER

HRESULT CGameInstance::Add_Prototype(const _wstring & strPrototypeTag, CGameObject * pPrototype)
{
	return m_pObject_Manager->Add_Prototype(strPrototypeTag, pPrototype);
}

HRESULT CGameInstance::Add_CloneObject_ToLayer(_uint iLevelIndex, const _wstring & strLayerTag, const _wstring & strPrototypeTag, void * pArg)
{
	return m_pObject_Manager->Add_CloneObject_ToLayer(iLevelIndex, strLayerTag, strPrototypeTag, pArg);
}

CComponent * CGameInstance::Find_Component(_uint iLevelIndex, const _wstring & strLayerTag, const _wstring & strComponentTag, _uint iIndex)
{
	return m_pObject_Manager->Find_Component(iLevelIndex, strLayerTag, strComponentTag, iIndex);	
}

CLayer* CGameInstance::Find_Layer(_uint iLevelIndex, const _wstring& strLayerTag)
{
	return m_pObject_Manager->Find_Layer(iLevelIndex, strLayerTag);
}

CGameObject* CGameInstance::Find_Object(_uint iLevelIndex, const _wstring& strLayerTag, _uint iIndex)
{
	return m_pObject_Manager->Find_Object(iLevelIndex, strLayerTag, iIndex);
}

CGameObject* CGameInstance::Find_Prototype(const _wstring& strPrototypeTag)
{
	return m_pObject_Manager->Find_Prototype(strPrototypeTag);
}

CGameObject* CGameInstance::Find_Player(_uint iLevelIndex)
{
	return m_pObject_Manager->Find_Player(iLevelIndex);
}

CGameObject* CGameInstance::Find_Camera(_uint iLevelIndex)
{
	return m_pObject_Manager->Find_Camera(iLevelIndex);
}

CGameObject* CGameInstance::Clone_GameObject(const _wstring& strPrototypeTag, void* pArg)
{
	return m_pObject_Manager->Clone_GameObject(strPrototypeTag, pArg);
}

#pragma endregion

#pragma region COMPONENT_MANAGER

HRESULT CGameInstance::Add_Prototype(_uint iLevelIndex, const _wstring & strPrototypeTag, CComponent * pPrototype)
{
	return m_pComponent_Manager->Add_Prototype(iLevelIndex, strPrototypeTag, pPrototype);	
}

CComponent * CGameInstance::Clone_Component(_uint iLevelIndex, const _wstring & strPrototypeTag, void * pArg)
{
	return m_pComponent_Manager->Clone_Component(iLevelIndex, strPrototypeTag, pArg);
}

#pragma endregion

#pragma region TIMER_MANAGER

HRESULT CGameInstance::Add_Timer(const _wstring & strTimerTag)
{
	return m_pTimer_Manager->Add_Timer(strTimerTag);
}

_float CGameInstance::Compute_TimeDelta(const _wstring & strTimerTag)
{
	return m_pTimer_Manager->Compute_TimeDelta(strTimerTag);
}

#pragma endregion

#pragma region RENDERER

HRESULT CGameInstance::Add_RenderObject(CRenderer::RENDERGROUP eRenderGroupID, CGameObject* pRenderObject)
{
	return m_pRenderer->Add_RenderObject(eRenderGroupID, pRenderObject);
}

list<class CGameObject*>& CGameInstance::Get_RenderList(CRenderer::RENDERGROUP eGroup)
{
	return m_pRenderer->Get_RenderList(eGroup);
}

#pragma endregion

#pragma region PIPELINE

void CGameInstance::Set_Transform(CPipeLine::D3DTRANSFORMSTATE eState, _fmatrix TransformMatrix)
{
	return m_pPipeLine->Set_Transform(eState, TransformMatrix);
}

_float4x4 CGameInstance::Get_Transform_Float4x4(CPipeLine::D3DTRANSFORMSTATE eState) const
{
	return m_pPipeLine->Get_Transform_Float4x4(eState);
}

_matrix CGameInstance::Get_Transform_Matrix(CPipeLine::D3DTRANSFORMSTATE eState) const
{
	return m_pPipeLine->Get_Transform_Matrix(eState);
}

_float4x4 CGameInstance::Get_Transform_Inverse_Float4x4(CPipeLine::D3DTRANSFORMSTATE eState) const
{
	return m_pPipeLine->Get_Transform_Inverse_Float4x4(eState);
}

_matrix CGameInstance::Get_Transform_Inverse_Matrix(CPipeLine::D3DTRANSFORMSTATE eState) const
{
	return m_pPipeLine->Get_Transform_Inverse_Matrix(eState);
}

_float4 CGameInstance::Get_CamPosition_Float4() const
{
	return m_pPipeLine->Get_CamPosition_Float4();
}

_vector CGameInstance::Get_CamPosition_Vector() const
{
	return m_pPipeLine->Get_CamPosition_Vector();
}

#pragma endregion

#pragma region Picking_Manager

void CGameInstance::Transform_MouseRay_ToLocalSpace(const _fmatrix& WorldMatrix)
{
	m_pPicking_Manager->Transform_ToLocalSpace(WorldMatrix);
}

_bool CGameInstance::isPicked_InLocalSpace(const _fvector& vPointA, const _fvector& vPointB, const _fvector& vPointC, _float3* pOut)
{
	return m_pPicking_Manager->isPicked_InLocalSpace(vPointA, vPointB, vPointC, pOut);
}

#pragma endregion

#pragma region Key_Manager

KEY_STATE CGameInstance::Get_KeyState(KEY _eKey)
{
	return m_pKey_Manager->Get_KeyState(_eKey);
}

#pragma endregion

#pragma region SOUND_MANAGER

void CGameInstance::Play_Sound(const TCHAR* pSoundKey, _uint eID, _float fVolume)
{
	m_pSound_Manager->PlaySound(pSoundKey, eID, fVolume);
}

void CGameInstance::Play_SoundRepeat(const TCHAR* pSoundKey, _uint eID, _float fVolume)
{
	m_pSound_Manager->Play_SoundRepeat(pSoundKey, eID, fVolume);
}

void CGameInstance::Play_BGM(const TCHAR* pSoundKey, _float fVolume)
{
	m_pSound_Manager->Play_BGM(pSoundKey, fVolume);
}

void CGameInstance::Stop_Sound(_uint eID)
{
	m_pSound_Manager->Stop_Sound(eID);
}

void CGameInstance::Stop_All()
{
	m_pSound_Manager->Stop_All();
}

void CGameInstance::Set_ChannelVolume(_uint eID, _float fVolume)
{
	m_pSound_Manager->Set_ChannelVolume(eID, fVolume);
}

void CGameInstance::Set_PlayeSpeed(_uint eID, _float fSpeedRatio)
{
	m_pSound_Manager->Set_PlayeSpeed(eID, fSpeedRatio);
}

void CGameInstance::Pause(_uint eID)
{
	m_pSound_Manager->Pause(eID);
}

_bool CGameInstance::IsPlaying(_uint eID)
{
	return m_pSound_Manager->IsPlaying(eID);
}
#pragma endregion

#pragma region EVENT_MANAGER

HRESULT CGameInstance::Event_CreateObject(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strPrototypeTag, _uint iSize, void* pArg)
{
	CEvent_Manager::EVENT tEvent = {};
	tEvent.eEven = CEvent_Manager::EVENT_TYPE::CREATE_OBJECT;	//���� �̺�Ʈ 

	// Ŭ���� ������ ä���ֱ�
	CEvent_Manager::CLONE_DESC* pCloneDesc = new CEvent_Manager::CLONE_DESC;
	pCloneDesc->iLevelIndex = iLevelIndex;	// ��� ����?
	pCloneDesc->strLayerTag = strLayerTag;	// ��� ���̾�?
	pCloneDesc->strPrototypeTag = strPrototypeTag; // ������Ÿ�� �±� ����?

	if (nullptr != pArg)
	{
		// �� void*�� ��ü�� Clone�ϸ鼭 Initizlie�� �Ѱ��� ������ �ʿ��ϸ� �������ִ� �뵵
		// �ٵ� ����ü ���� ���� �Ѱ������ ������ ��� �ٸ� -> �ٵ� �������� �и��� ������ �Լ� ���� �����ϸ� �����
		// �ٵ� ��� ������ �����ϸ鼭 �Ѱ�����? -> ���� �׳� �����ͷ� ���� �ǰڴ� -> �ٵ� void*�ε� ��� �Ѱ�����?
		// ���� �׷� ����� �Ű������� �޾ƿͼ� 1byte������ char�� �ʱ�ȭ�ϰ� �޸� ��ü�� �����ع�����!!
		
		// �޾ƿ� ���̸� �̿��ؼ� pArg ���� 
		pCloneDesc->pArg = new char[iSize];

		// �޸� ä�� ����
		memcpy(pCloneDesc->pArg, pArg, iSize);
	}

	tEvent.CloneDesc = pCloneDesc;

	m_pEvent_Manager->AddEvent(tEvent);

	return S_OK;
}

HRESULT CGameInstance::Event_DestoryObject(CGameObject* pGameObject)
{
	CEvent_Manager::EVENT tEvent = {};
	tEvent.eEven = CEvent_Manager::EVENT_TYPE::DELETE_OBJECT;	// ���� �̺�Ʈ��

	// �ϴ� �־�߸� �ؼ� ������ �Ű�X
	CEvent_Manager::CLONE_DESC* pCloneDesc = new CEvent_Manager::CLONE_DESC;
	tEvent.lParam = (DWORD_PTR)pGameObject;

	tEvent.CloneDesc = pCloneDesc;

	m_pEvent_Manager->AddEvent(tEvent);

	return S_OK;
}
#pragma endregion

#pragma region PhysX_MANAGER

HRESULT CGameInstance::AddScene_ColMesh(CGameObject* pObject, _wstring strModelName)
{
	return m_pPhysX_Manager->AddScene_ColMesh(pObject, strModelName);
}

HRESULT CGameInstance::SetUp_Player(CGameObject* pPlayer)
{
	return m_pPhysX_Manager->SetUp_Player(pPlayer);
}

void CGameInstance::DeletePlayer()
{
	m_pPhysX_Manager->DeletePlayer();
}

void CGameInstance::DeleteActors()
{
	m_pPhysX_Manager->DeleteActors();
}

#pragma endregion



#pragma region FONT_MANAGER 

HRESULT CGameInstance::Add_Font(const _wstring& strFontTag, const _tchar* pFontFilePath)
{
	return m_pFont_Manager->Add_Font(strFontTag, pFontFilePath);
}
HRESULT CGameInstance::Render_Text(const _wstring& strFontTag, const _tchar* pText, _fvector vPosition, _fvector vColor, _float fRadian, _fvector vPivot, _float fScale)
{
	return m_pFont_Manager->Render(strFontTag, pText, vPosition, vColor, fRadian, vPivot, fScale);
}
#pragma endregion

#pragma region Collider_Manager
HRESULT CGameInstance::Add_ColliderList(CCollider* pCollider)
{
	return m_pCollider_Manager->Add_ColliderList(pCollider);
}

#pragma endregion



void CGameInstance::Release_Engine()
{
	Safe_Release(m_pPhysX_Manager);
	Safe_Release(m_pFont_Manager);
	Safe_Release(m_pPicking_Manager);
	Safe_Release(m_pPipeLine);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pKey_Manager);
	Safe_Release(m_pSound_Manager);
	Safe_Release(m_pTimer_Manager);
	Safe_Release(m_pComponent_Manager);
	Safe_Release(m_pCollider_Manager);

	Safe_Release(m_pEvent_Manager);
	Safe_Release(m_pUI_Manager);
	Safe_Release(m_pObject_Manager);
	Safe_Release(m_pLevel_Manager);
	Safe_Release(m_pInput_Device);
	Safe_Release(m_pGraphic_Device);

	CGameInstance::Get_Instance()->Destroy_Instance();	
}

void CGameInstance::Free()
{	
	__super::Free();

}
