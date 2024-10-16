#pragma once

#include "Component_Manager.h"
#include "Renderer.h"
#include "PipeLine.h"

/* 1. 엔진과 클라이언트의 소통을 위한 클래스읻. */
/* 2. 엔진에서 클라이언트에 보여주고 싶은 함수들을 모아놓는다. */
/* 3. 함수들 -> 클래스의 멤버함수. 객체가 필요하다! 그래서 기능응ㄹ 제공해주기위한 클래스 객체를 보관한다. */
/* 4. 객체들을 보관하기위해 Initialize_Engine() 함수 안에서 기능을 제공해주기위한 클래스들을 객체화한다. */

BEGIN(Engine)

class ENGINE_DLL CGameInstance final : public CBase
{
	DECLARE_SINGLETON(CGameInstance)
private:
	CGameInstance();
	virtual ~CGameInstance() = default;

public:
	HRESULT Initialize_Engine(HINSTANCE hInst, _uint iNumLevels, const ENGINE_DESC& EngineDesc, _Inout_ ID3D11Device** ppDevice, _Inout_ ID3D11DeviceContext** ppContext);
	void Update_Engine(_float fTimeDelta);
	HRESULT Draw_Engine();
	void FinalUpdate_Engine();
	HRESULT Clear(_uint iLevelIndex);

public:
	POINT Get_MosePos();

	_float Get_Random_Normal() { return (_float)rand() / (_float)RAND_MAX; }
	_float Get_Random(_float fMin, _float fMax) { return fMin + Get_Random_Normal() * (fMax - fMin); }
	
#pragma region GRAPHIC_DEVICE
public:
	void Render_Begin();
	void Render_End();
#pragma endregion

#pragma region INPUT_DEVICE
	_byte	Get_DIKeyState(_ubyte byKeyID);
	_byte	Get_DIMouseState(MOUSEKEYSTATE eMouse);
	_long	Get_DIMouseMove(MOUSEMOVESTATE eMouseState);
#pragma endregion

#pragma region LEVEL_MANAGER
	HRESULT Change_Level(_uint iLevelIndex, class CLevel* pNextLevel);
	class CLevel* Get_CurLevel();
	_uint   Get_CurLevelIndex();
	_uint   Get_PreLevelIndex();
#pragma endregion

#pragma region OBJECT_MANAGER
	HRESULT Add_Prototype(const _wstring& strPrototypeTag, class CGameObject* pPrototype);
	HRESULT Add_CloneObject_ToLayer(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strPrototypeTag, void* pArg = nullptr);
	class CGameObject* Clone_GameObject(const _wstring& strPrototypeTag, void* pArg = nullptr);

	class CComponent*  Find_Component(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strComponentTag, _uint iIndex = 0);
	class CLayer*	   Find_Layer(_uint iLevelIndex, const _wstring& strLayerTag);
	class CGameObject* Find_Object(_uint iLevelIndex, const _wstring& strLayerTag, _uint iIndex);
	class CGameObject* Find_Prototype(const _wstring& strPrototypeTag);
	class CGameObject* Find_Player(_uint iLevelIndex);
	class CGameObject* Find_Camera(_uint iLevelIndex = 0);
#pragma endregion

#pragma region COMPONENT_MANAGER
	HRESULT Add_Prototype(_uint iLevelIndex, const _wstring& strPrototypeTag, class CComponent* pPrototype);
	class CComponent* Clone_Component(_uint iLevelIndex, const _wstring& strPrototypeTag, void* pArg = nullptr );
#pragma endregion

#pragma region TIMER_MANAGER
	HRESULT Add_Timer(const _wstring& strTimerTag);
	_float Compute_TimeDelta(const _wstring& strTimerTag);	
#pragma endregion

#pragma region RENDERER
	HRESULT Add_RenderObject(CRenderer::RENDERGROUP eRenderGroupID, class CGameObject* pRenderObject);
	list<class CGameObject*>& Get_RenderList(CRenderer::RENDERGROUP eGroup);
#ifdef _DEBUG
	HRESULT Add_DebugObject(class CComponent* pDebugObject);
#endif
#pragma endregion

#pragma region PIPELINE
	void Set_Transform(CPipeLine::D3DTRANSFORMSTATE eState, _fmatrix TransformMatrix);
	void  Set_ShadowLight(CPipeLine::D3DTRANSFORMSTATE eState, _fvector vPlayerPos);
	_float4x4 Get_Transform_Float4x4(CPipeLine::D3DTRANSFORMSTATE eState) const;
	_matrix Get_Transform_Matrix(CPipeLine::D3DTRANSFORMSTATE eState) const;
	_float4x4 Get_Transform_Inverse_Float4x4(CPipeLine::D3DTRANSFORMSTATE eState) const;
	_matrix Get_Transform_Inverse_Matrix(CPipeLine::D3DTRANSFORMSTATE eState) const;

	_float4 Get_CamPosition_Float4() const;
	_vector Get_CamPosition_Vector() const;
	_vector Get_ShadowLightPos_Vector();
	_vector Get_LightLook_Vector();
#pragma endregion

#pragma region LIGHT_MANAGER
	HRESULT Add_Light(const LIGHT_DESC& LightDesc);
	const LIGHT_DESC* Get_LightDesc(_uint iIndex) const;
	HRESULT Render_Lights(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
	HRESULT Reset_Lights();
#pragma endregion

#pragma region TARGET_MANAGER
	HRESULT Add_RenderTarget(const _wstring& strTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4& vClearColor);
	HRESULT Add_MRT(const _wstring& strMRTTag, const _wstring& strTargetTag);
	HRESULT Begin_MRT(const _wstring& strMRTTag, ID3D11DepthStencilView* pDSV = nullptr);
	HRESULT End_MRT();
	HRESULT Bind_RT_ShaderResource(class CShader* pShader, const _wstring& strTargetTag, const _char* pConstantName);
	
#ifdef _DEBUG
	HRESULT Ready_RT_Debug(const _wstring& strTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY);
	HRESULT Render_MRT_Debug(const _wstring& strMRTTag, class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
	void Active_Debug_Renderer();
#endif
#pragma endregion

#pragma region PICKING
	void Transform_MouseRay_ToLocalSpace(const _fmatrix& WorldMatrix);
	_bool isPicked_InLocalSpace(const _fvector& vPointA, const _fvector& vPointB, const _fvector& vPointC, _float3* pOut);
#pragma endregion

#pragma region KEY_MANAGER
	KEY_STATE   Get_KeyState(KEY _eKey);
#pragma endregion

#pragma region SOUND_MANAGER
	void Play_BGM(const TCHAR* pSoundKey, _float fVolume);
	void Stop_BGM();
	void Pause_BGM();
	void SetVolume_BGM(_float fVolume);
	void	Set_Listener(class CGameObject* pListener);
	FMOD::System* Get_SoundSystem();
	map<TCHAR*, FMOD::Sound*>& Get_Sounds();
	void	LoadSoundFile(const char* pFolderName);
#pragma endregion

#pragma region EVENT_MANAGER
	HRESULT Event_CreateObject(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strPrototypeTag, _uint iSize = 0, void* pArg = nullptr);
	HRESULT Event_DestoryObject(class CGameObject* pGameObject);
#pragma endregion

#pragma region PhysX_MANAGER
	HRESULT AddScene_ColMesh(class CGameObject* pObject, _wstring strModelName);
	HRESULT SetUp_Player(class CGameObject* pPlayer);
	HRESULT Destroy_PhysXActor(CGameObject* pObject);   //우송아 이거야
	void	DeletePlayer();
	void	DeleteActors();
#pragma endregion

#pragma region FONT_MANAGER
	HRESULT Add_Font(const _wstring& strFontTag, const _tchar* pFontFilePath);
	HRESULT Render_Text(const _wstring& strFontTag, const _tchar* pText, _fvector vPosition, _fvector vColor = XMVectorSet(1.f, 1.f, 1.f, 1.f), _float fRadian = 0.f, _fvector vPivot = XMVectorSet(0.f, 0.f, 0.f, 1.f), _float fScale = 1.f, _bool bChangeAlpha = false);
	HRESULT Render_Center(const _wstring& strFontTag, const _tchar* pText, _fvector vPosition, _fvector vColor, _float fRadian, _fvector vPivot, _float fScale, _bool bChangeAlpha = false);
#pragma endregion

#pragma region Collider_Manager
	HRESULT Add_ColliderList(class CCollider* pCollider);
#pragma endregion

#pragma region FRUSTUM
	public:
		_bool isIn_Frustum_WorldSpace(_fvector vPosition, _float fRadius = 0.f);
#pragma endregion

private:
	class CGraphic_Device*			m_pGraphic_Device = { nullptr };
	class CInput_Device*			m_pInput_Device = { nullptr };
	class CLevel_Manager*			m_pLevel_Manager = { nullptr };
	class CObject_Manager*			m_pObject_Manager = { nullptr };
	class CComponent_Manager*		m_pComponent_Manager = { nullptr };
	class CTimer_Manager*			m_pTimer_Manager = { nullptr };
	class CRenderer*				m_pRenderer = { nullptr };
	class CPipeLine*				m_pPipeLine = { nullptr };
	class CPicking_Manager*			m_pPicking_Manager = { nullptr };
	class CFont_Manager* m_pFont_Manager = { nullptr };
	class CCollider_Manager* m_pCollider_Manager = { nullptr };
	class CLight_Manager* m_pLight_Manager = { nullptr };
	class CTarget_Manager* m_pTarget_Manager = { nullptr };
	class CFrustum* m_pFrustum = { nullptr };

	/*추가한 기능들*/
	class CKey_Manager*				m_pKey_Manager = { nullptr };
	class CSound_Manager*			m_pSound_Manager = { nullptr };
	class CUI_Manager*				m_pUI_Manager = { nullptr };

	class CPhysX_Manager*			m_pPhysX_Manager = { nullptr };
	class CEvent_Manager*			m_pEvent_Manager = { nullptr };


public:	
	void Release_Engine();
	virtual void Free() override;
};

END