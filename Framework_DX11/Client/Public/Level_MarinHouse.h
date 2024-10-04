#pragma once

#include "Client_Defines.h"


#include "Level.h"

BEGIN(Client)

class CLevel_MarinHouse final : public CLevel
{
private:
	CLevel_MarinHouse(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_MarinHouse() = default;

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
	HRESULT Read_Nav();
	HRESULT Read_LandObjects(_int _type, _uint _index, _float3 _fPos, _float3 _fScaled, _float3 _fRot);
	HRESULT Read_NonAnimObj(_int _type, _uint _index, _float3 _fPos, _float3 _fScaled, _float3 _fRot, string _strLyaerTag);
	HRESULT Read_AnimObj(_int _type, _uint _index, _float3 _fPos, _float3 _fScaled, _float3 _fRot, string _strLyaerTag);

private:
	class CNavigation* m_pNavigationCom = { nullptr };
	class CTeleport* m_pTeleportObj = { nullptr };

public:
	static CLevel_MarinHouse* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END
