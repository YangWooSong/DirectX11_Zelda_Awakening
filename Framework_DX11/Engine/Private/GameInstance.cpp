#include "..\Public\GameInstance.h"
#include "Graphic_Device.h"
#include "Input_Device.h"
#include "Level_Manager.h"
#include "Object_Manager.h"
#include "Timer_Manager.h"
#include "Picking_Manager.h"
#include "Font_Manager.h"
#include "Light_Manager.h"
#include "Target_Manager.h"

#include "Key_Manager.h"
#include "Sound_Manager.h"
#include "UI_Manager.h"

#include "PhysX_Manager.h"
#include "Event_Manager.h"
#include "Collider_Manager.h"
#include "Frustum.h"

IMPLEMENT_SINGLETON(CGameInstance)

CGameInstance::CGameInstance()
{

}

HRESULT CGameInstance::Initialize_Engine(HINSTANCE hInst, _uint iNumLevels, const ENGINE_DESC& EngineDesc, _Inout_ ID3D11Device** ppDevice, _Inout_ ID3D11DeviceContext** ppContext)
{
	srand(unsigned(time(nullptr)));

	/* 그래픽 카드를 초기화하낟. */
	m_pGraphic_Device = CGraphic_Device::Create(EngineDesc.hWnd, EngineDesc.isWindowsed, EngineDesc.iWinSizeX, EngineDesc.iWinSizeY, ppDevice, ppContext);
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pFrustum = CFrustum::Create();
	if (nullptr == m_pFrustum)
		return E_FAIL;

	m_pTimer_Manager = CTimer_Manager::Create();
	if (nullptr == m_pTimer_Manager)
		return E_FAIL;

	m_pTarget_Manager = CTarget_Manager::Create(*ppDevice, *ppContext);
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	m_pRenderer = CRenderer::Create(*ppDevice, *ppContext);
	if (nullptr == m_pRenderer)
		return E_FAIL;

	m_pPipeLine = CPipeLine::Create();
	if (nullptr == m_pPipeLine)
		return E_FAIL;

	m_pLight_Manager = CLight_Manager::Create();
	if (nullptr == m_pLight_Manager)
		return E_FAIL;

	/* 사운드 카드를 초기화하낟. */

	/* 입력장치를 초기화하낟. */
	m_pInput_Device = CInput_Device::Create(hInst, EngineDesc.hWnd);
	if (nullptr == m_pInput_Device)
		return E_FAIL;

	m_pPicking_Manager = CPicking_Manager::Create(*ppDevice, *ppContext, EngineDesc.hWnd, EngineDesc.iWinSizeX, EngineDesc.iWinSizeY);
	if (nullptr == m_pPicking_Manager)
		return E_FAIL;

	/* 여러가지 매니져를 초기화하낟. */

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

	m_pEvent_Manager = CEvent_Manager::Create(m_pLevel_Manager, m_pObject_Manager, m_pUI_Manager); // << 이벤트매니저는 항상 마지막에 만드세요
	if (nullptr == m_pEvent_Manager)
		return E_FAIL;


	m_pFont_Manager = CFont_Manager::Create(*ppDevice, *ppContext);
	if (nullptr == m_pFont_Manager)
		return E_FAIL;

	return S_OK;
}



void CGameInstance::Update_Engine(_float fTimeDelta)
{
	/* 현재 키보드와 마우스의 상태를 받아올꺼야. */
	m_pInput_Device->Update();

	m_pKey_Manager->Update();	// 키 매니저의 업데이트는 항상 맨 처음에 해주자

	m_pObject_Manager->Priority_Update(fTimeDelta);	

	m_pPipeLine->Update();

	m_pFrustum->Update();

	m_pPicking_Manager->Update();

	m_pObject_Manager->Update(fTimeDelta);
	
	m_pObject_Manager->Late_Update(fTimeDelta);

	m_pCollider_Manager->Update();

	m_pPhysX_Manager->PhysX_Update(fTimeDelta);

	//UIManager는 항상 Render 되기 전에 호출
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

	/* 특정 레벨을 위해 존재하는 자원들은 해당 레벨이 끝나면 삭제하자. */

	m_pObject_Manager->Clear(iLevelIndex);

	/* 컴포넌트 원형들도 레벨별로 관리했었다. */
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

ID3D11ShaderResourceView* CGameInstance::Get_BackBuffer_SRV() const
{
	return m_pGraphic_Device->Get_BackBuffer_SRV();
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

_uint CGameInstance::Get_PreLevelIndex()
{
	return m_pLevel_Manager->Get_PreLevelIndex();
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

#ifdef _DEBUG
HRESULT CGameInstance::Add_DebugObject(CComponent* pDebugObject)
{
	return m_pRenderer->Add_DebugObject(pDebugObject);
}
#endif
#pragma endregion

#pragma region PIPELINE

void CGameInstance::Set_Transform(CPipeLine::D3DTRANSFORMSTATE eState, _fmatrix TransformMatrix)
{
	return m_pPipeLine->Set_Transform(eState, TransformMatrix);
}

void CGameInstance::Set_ShadowLightLook(CPipeLine :: D3DTRANSFORMSTATE eState, _fvector vPlayerPos)
{
	return m_pPipeLine->Set_ShadowLightLook(eState, vPlayerPos);
}

void CGameInstance::Set_ShadowLightPos(CPipeLine::D3DTRANSFORMSTATE eState, _fvector vLightPos)
{
	return m_pPipeLine->Set_ShadowLightPos(eState, vLightPos);
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

_vector CGameInstance::Get_ShadowLightPos_Vector()
{
	return m_pPipeLine->Get_ShadowLightPos_Vector();
}

_vector CGameInstance::Get_LightLook_Vector()
{
	return m_pPipeLine->Get_LightLook_Vector();
}

#pragma endregion

#pragma region LIGHT_MANAGER
HRESULT CGameInstance::Add_Light(const LIGHT_DESC& LightDesc)
{
	return m_pLight_Manager->Add_Light(LightDesc);
}
const LIGHT_DESC* CGameInstance::Get_LightDesc(_uint iIndex) const
{
	return m_pLight_Manager->Get_LightDesc(iIndex);
}

HRESULT CGameInstance::Render_Lights(CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
	return m_pLight_Manager->Render(pShader, pVIBuffer);
}

HRESULT CGameInstance::Reset_Lights()
{
	return m_pLight_Manager->Reset_Lights();
}

#pragma endregion

#pragma region TARGET_MANAGER
HRESULT CGameInstance::Add_RenderTarget(const _wstring& strTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4& vClearColor)
{
	return m_pTarget_Manager->Add_RenderTarget(strTargetTag, iWidth, iHeight, ePixelFormat, vClearColor);
}

HRESULT CGameInstance::Add_MRT(const _wstring& strMRTTag, const _wstring& strTargetTag)
{
	return m_pTarget_Manager->Add_MRT(strMRTTag, strTargetTag);
}

HRESULT CGameInstance::Begin_MRT(const _wstring& strMRTTag, ID3D11DepthStencilView* pDSV)
{
	return m_pTarget_Manager->Begin_MRT(strMRTTag, pDSV);
}

HRESULT CGameInstance::End_MRT()
{
	return m_pTarget_Manager->End_MRT();
}

HRESULT CGameInstance::Bind_RT_ShaderResource(CShader* pShader, const _wstring& strTargetTag, const _char* pConstantName)
{
	return m_pTarget_Manager->Bind_ShaderResource(pShader, strTargetTag, pConstantName);
}

#ifdef _DEBUG
HRESULT CGameInstance::Ready_RT_Debug(const _wstring& strTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY)
{
	return m_pTarget_Manager->Ready_Debug(strTargetTag, fX, fY, fSizeX, fSizeY);
}
HRESULT CGameInstance::Render_MRT_Debug(const _wstring& strMRTTag, CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
	return m_pTarget_Manager->Render(strMRTTag, pShader, pVIBuffer);
}
void CGameInstance::Active_Debug_Renderer()
{
	m_pTarget_Manager->Acticve_DebugRender();
}
#endif


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

void CGameInstance::Play_BGM(const TCHAR* pSoundKey, _float fVolume)
{
	m_pSound_Manager->Play_BGM(pSoundKey, fVolume);
}

void CGameInstance::Stop_BGM()
{
	m_pSound_Manager->Stop_BGM();
}

void CGameInstance::Pause_BGM()
{
	m_pSound_Manager->Pause_BGM();
}

void CGameInstance::SetVolume_BGM(_float fVolume)
{
	m_pSound_Manager->SetVolume_BGM(fVolume);
}

void CGameInstance::Set_Listener(CGameObject* pListener)
{
	m_pSound_Manager->Set_Listener(pListener);
}

FMOD::System* CGameInstance::Get_SoundSystem()
{
	return m_pSound_Manager->Get_System();
}

map<TCHAR*, FMOD::Sound*>& CGameInstance::Get_Sounds()
{
	return m_pSound_Manager->Get_Sounds();
}

void CGameInstance::LoadSoundFile(const char* pFolderName)
{
	m_pSound_Manager->LoadSoundFile(pFolderName);
}

#pragma endregion

#pragma region EVENT_MANAGER

HRESULT CGameInstance::Event_CreateObject(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strPrototypeTag, _uint iSize, void* pArg)
{
	CEvent_Manager::EVENT tEvent = {};
	tEvent.eEven = CEvent_Manager::EVENT_TYPE::CREATE_OBJECT;	//생성 이벤트 

	// 클론할 정보들 채워넣기
	CEvent_Manager::CLONE_DESC* pCloneDesc = new CEvent_Manager::CLONE_DESC;
	pCloneDesc->iLevelIndex = iLevelIndex;	// 어디 레벨?
	pCloneDesc->strLayerTag = strLayerTag;	// 어디 레이어?
	pCloneDesc->strPrototypeTag = strPrototypeTag; // 프로토타입 태그 뭐야?

	if (nullptr != pArg)
	{
		// 이 void*는 객체를 Clone하면서 Initizlie에 넘겨줄 정보가 필요하면 저장해주는 용도
		// 근데 구조체 정보 많고 넘겨줘야할 정보가 모두 다름 -> 근데 프레임이 밀리기 때문에 함수 내에 생성하면 사라짐
		// 근데 어떻게 정보를 유지하면서 넘겨주지? -> 아하 그냥 포인터로 쓰면 되겠다 -> 근데 void*인데 어떻게 넘겨주지?
		// 아하 그럼 사이즈도 매개변수로 받아와서 1byte단위인 char로 초기화하고 메모리 자체를 복사해버리자!!
		
		// 받아온 사이를 이용해서 pArg 생성 
		pCloneDesc->pArg = new char[iSize];

		// 메모리 채로 복사
		memcpy(pCloneDesc->pArg, pArg, iSize);
	}

	tEvent.CloneDesc = pCloneDesc;

	m_pEvent_Manager->AddEvent(tEvent);

	return S_OK;
}

HRESULT CGameInstance::Event_DestoryObject(CGameObject* pGameObject)
{
	CEvent_Manager::EVENT tEvent = {};
	tEvent.eEven = CEvent_Manager::EVENT_TYPE::DELETE_OBJECT;	// 삭제 이벤트야

	// 일단 있어야만 해서 생성함 신경X
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

HRESULT CGameInstance::Destroy_PhysXActor(CGameObject* pObject)
{
	return m_pPhysX_Manager->Destroy_PhysXActor(pObject);
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
HRESULT CGameInstance::Render_Text(const _wstring& strFontTag, const _tchar* pText, _fvector vPosition, _fvector vColor, _float fRadian, _fvector vPivot, _float fScale, _bool bChangeAlpha)
{
	return m_pFont_Manager->Render(strFontTag, pText, vPosition, vColor, fRadian, vPivot, fScale, bChangeAlpha);
}
HRESULT CGameInstance::Render_Center(const _wstring& strFontTag, const _tchar* pText, _fvector vPosition, _fvector vColor, _float fRadian, _fvector vPivot, _float fScale, _bool bChangeAlpha)
{
	return  m_pFont_Manager->Render_Center(strFontTag, pText, vPosition, vColor, fRadian, vPivot, fScale, bChangeAlpha);
}
#pragma endregion

#pragma region Collider_Manager
HRESULT CGameInstance::Add_ColliderList(CCollider* pCollider)
{
	return m_pCollider_Manager->Add_ColliderList(pCollider);
}
#pragma endregion

#pragma region FRUSTUM

_bool CGameInstance::isIn_Frustum_WorldSpace(_fvector vPosition, _float fRadius)
{
	return m_pFrustum->isIn_WorldSpace(vPosition, fRadius);
}

#pragma endregion



void CGameInstance::Release_Engine()
{
	Safe_Release(m_pFrustum);
	Safe_Release(m_pTarget_Manager);
	Safe_Release(m_pPhysX_Manager);
	Safe_Release(m_pFont_Manager);
	Safe_Release(m_pLight_Manager);
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
