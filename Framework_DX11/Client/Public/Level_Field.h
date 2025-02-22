#pragma once
#include "Client_Defines.h"


#include "Level.h"

BEGIN(Client)

class CLevel_Field final : public CLevel
{
private:
	CLevel_Field(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Field() = default;

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
	
	HRESULT Read();
	HRESULT Read_LandObjects(_int _type, _uint _index, _float3 _fPos, _float3 _fScaled, _float3 _fRot);
	HRESULT Read_AnimMonster(_int _type, _uint _index, _float3 _fPos, _float3 _fScaled, _float3 _fRot, string _strLyaerTag, _int _iCellNum);
	HRESULT Read_NonAnimObj(_int _type, _uint _index, _float3 _fPos, _float3 _fScaled, _float3 _fRot, string _strLyaerTag);
	HRESULT Read_AnimObj(_int _type, _uint _index, _float3 _fPos, _float3 _fScaled, _float3 _fRot, string _strLyaerTag, _int _iCellNum);

private:
	class CPlayer* m_pPlayer = { nullptr };
	class CTeleport* m_pTeleportObj_0 = { nullptr };
	class CTeleport* m_pTeleportObj_1 = { nullptr };
	class CTeleport* m_pTeleportObj_2 = { nullptr };
	class CMainUI* m_pMainUI = { nullptr };

	_bool m_bFadeOut = { false };
private:
	void Setting_Gimmick();
public:
	static CLevel_Field* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END