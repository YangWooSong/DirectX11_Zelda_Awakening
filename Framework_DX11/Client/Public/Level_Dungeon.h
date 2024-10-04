#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)

class CLevel_Dungeon final : public CLevel
{
public:
	enum BGM_INDEX{ BGM_BACKGROUND, BGM_BOSS, BGM_INSTRUMENT, BGM_END };
private:
	CLevel_Dungeon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Dungeon() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	HRESULT Ready_Lights();
	HRESULT Ready_Layer_Camera();
	HRESULT Ready_Layer_BackGround();
	HRESULT Ready_Layer_Effect();

	HRESULT Ready_LandObjects();
	//HRESULT Ready_Layer_Player(CLandObject::LANDOBJECT_DESC& LandObjectDesc);
	//HRESULT Ready_Layer_Monster(CLandObject::LANDOBJECT_DESC& LandObjectDesc);

	HRESULT Read();
	HRESULT Read_LandObjects(_int _type, _uint _index, _float3 _fPos, _float3 _fScaled, _float3 _fRot);
	HRESULT Read_AnimMonster(_int _type, _uint _index, _float3 _fPos, _float3 _fScaled, _float3 _fRot, string _strLyaerTag, _int _iCellNum, _uint _iRoomNum);
	HRESULT Read_NonAnimObj(_int _type, _uint _index, _float3 _fPos, _float3 _fScaled, _float3 _fRot, string _strLyaerTag, _uint _iRoomNum);
	HRESULT Read_AnimObj(_int _type, _uint _index, _float3 _fPos, _float3 _fScaled, _float3 _fRot, string _strLyaerTag, _int _iCellNum, _uint _iRoomNum);

private:
	class CNavigation* m_pNavigationCom = { nullptr };
	class CPlayer* m_pPlayer = { nullptr };
	class CPlayerCamera* m_pPlayerCam = { nullptr };

	_bool	m_isPlayerGetCampus = { false };
	_bool   m_bFirstInRoom10 = { true };
	_bool   m_bFirstInRoom12 = { true };
	_bool   m_bCamZoomOut = { false };
	_bool   m_bCamZoomOut_Room10 = { false };
	_bool   m_bChageBGM = { false };
	_bool m_bRestartBgm = { false };

	_uint m_iBgmIndex = { 0 };
	_uint m_iCurRoomNum = { 1 };

	_float  m_fTimer = { 0.f };
	_float  m_fRolaDeadTimer = { 0.f };
	_float m_fFps = { 0.f };
	_float m_fFpsTimer = { 0.f };
	_float m_fFrameCount = { 0.f };
	vector<vector<_uint>> m_RoomNumList = {
		{28},               // Room 1
		{27},               // Room 2
		{26},               // Room 3
		{24},               // Room 4
		{25},               // Room 5
		{16},               // Room 6
		{10, 11, 12, 17, 18, 19},  // Room 7
		{20},               // Room 8
		{13},               // Room 9
		{21},               // Room 10
		{14},               // Room 11
		{8},                // Room 12
		{4},                // Room 13
		{6, 7},             // Room 14
		{5},                // Room 15
		{9, 15, 22}        // Room 16
	};

	_float4  m_CameraRoomPos[16] =
	{
		_float4(0.f, 20.f, -11.f, 1.f), _float4(-15.f, 20.f, -11.f, 1.f),_float4(-30.f, 20.f, -11.f, 1.f),_float4(0.f, 20.f, 1.f, 1.f),
		_float4(15.f, 20.f, 1.f, 1.f),  _float4(-30.f, 20.f, 13.f, 1.f),_float4(-15.f, 15.f, 13.f, 25.f),_float4(30.f, 20.f, 13.f, 1.f),
		_float4(30.f, 20.f, 25.f, 1.f),_float4(45.f, 20.f, 13.f, 1.f),_float4(45.f, 20.f, 25.f, 1.f),_float4(45.f, 20.f, 37.f, 1.f),
		_float4(45.f, 20.f, 49.f, 1.f),_float4(-15.f, 0.f, 37.f, 1.f),_float4(-30.f, 20.f, 37.f, 1.f),_float4(-45.f, -45.f, -11.f, 1.f),
	};

private:
	void Change_Room();
	void Setting_Gimmick(_float fTimeDelta);

public:
	static CLevel_Dungeon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END
