#pragma once

#include "Component_Manager.h"
#include "Renderer.h"
#include "PipeLine.h"

/* 1. ������ Ŭ���̾�Ʈ�� ������ ���� Ŭ������. */
/* 2. �������� Ŭ���̾�Ʈ�� �����ְ� ���� �Լ����� ��Ƴ��´�. */
/* 3. �Լ��� -> Ŭ������ ����Լ�. ��ü�� �ʿ��ϴ�! �׷��� ������� �������ֱ����� Ŭ���� ��ü�� �����Ѵ�. */
/* 4. ��ü���� �����ϱ����� Initialize_Engine() �Լ� �ȿ��� ����� �������ֱ����� Ŭ�������� ��üȭ�Ѵ�. */

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
#pragma endregion

#pragma region OBJECT_MANAGER
	HRESULT Add_Prototype(const _wstring& strPrototypeTag, class CGameObject* pPrototype);
	HRESULT Add_CloneObject_ToLayer(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strPrototypeTag, void* pArg = nullptr);

	class CComponent* Find_Component(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strComponentTag, _uint iIndex = 0);
	class CLayer* Find_Layer(_uint iLevelIndex, const _wstring& strLayerTag);
	class CGameObject* Find_Object(_uint iLevelIndex, const _wstring& strLayerTag, _uint iIndex);
	class CGameObject* Find_Prototype(const _wstring& strPrototypeTag);

	class CGameObject* Find_Player(_uint iLevelIndex);
	class CGameObject* Find_Camera(_uint iLevelIndex = 0);

	class CGameObject* Clone_GameObject(const _wstring& strPrototypeTag, void* pArg = nullptr);
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
#pragma endregion

#pragma region PIPELINE
	void Set_Transform(CPipeLine::D3DTRANSFORMSTATE eState, _fmatrix TransformMatrix);
	_float4x4 Get_Transform_Float4x4(CPipeLine::D3DTRANSFORMSTATE eState) const;
	_matrix Get_Transform_Matrix(CPipeLine::D3DTRANSFORMSTATE eState) const;
	_float4x4 Get_Transform_Inverse_Float4x4(CPipeLine::D3DTRANSFORMSTATE eState) const;
	_matrix Get_Transform_Inverse_Matrix(CPipeLine::D3DTRANSFORMSTATE eState) const;

	_float4 Get_CamPosition_Float4() const;
	_vector Get_CamPosition_Vector() const;
#pragma endregion

#pragma region PICKING
	void Transform_MouseRay_ToLocalSpace(const _fmatrix& WorldMatrix);
	_bool isPicked_InLocalSpace(const _fvector& vPointA, const _fvector& vPointB, const _fvector& vPointC, _float3* pOut);
#pragma endregion

#pragma region KEY_MANAGER
	KEY_STATE   Get_KeyState(KEY _eKey);
#pragma endregion

#pragma region SOUND_MANAGER
	void	Play_Sound(const TCHAR* pSoundKey, _uint eID, _float fVolume);
	void	Play_SoundRepeat(const TCHAR* pSoundKey, _uint eID, _float fVolume);
	void	Play_BGM(const TCHAR* pSoundKey, _float fVolume);
	void	Stop_Sound(_uint eID);
	void	Stop_All();
	void	Set_ChannelVolume(_uint eID, _float fVolume);
	void	Set_PlayeSpeed(_uint eID, _float fSpeedRatio);
	void	Pause(_uint eID);
	_bool	IsPlaying(_uint eID);
#pragma endregion

#pragma region EVENT_MANAGER
	HRESULT Event_CreateObject(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strPrototypeTag, _uint iSize = 0, void* pArg = nullptr);
	HRESULT Event_DestoryObject(class CGameObject* pGameObject);
#pragma endregion

#pragma region PhysX_MANAGER
	HRESULT AddScene_ColMesh(class CGameObject* pObject, _wstring strModelName);
	HRESULT SetUp_Player(class CGameObject* pPlayer);
	void	DeletePlayer();
	void	DeleteActors();
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

	/*�߰��� ��ɵ�*/
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