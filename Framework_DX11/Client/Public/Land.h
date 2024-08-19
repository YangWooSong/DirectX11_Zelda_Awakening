#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
END

class CLand :
	public CGameObject
{
private:
	CLand(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLand(const CLand& Prototype);
	virtual ~CLand() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	class CShader* m_pShaderCom = { nullptr };
	class CModel* m_pModelCom = { nullptr };

private:
	HRESULT Ready_Components();

	HRESULT Add_Component_Dungeon();
	HRESULT Add_Component_Field();
	HRESULT Add_Component_SingleMap();

private:
	_wstring m_DungeonList[29] = {
		TEXT("01F"),TEXT("01G"), TEXT("02F"), TEXT("02G"), TEXT("03G"), TEXT("04B"), TEXT("04C"), TEXT("04D"), TEXT("04G"), TEXT("05A"),
		TEXT("05C"), TEXT("05D"), TEXT("05E"), TEXT("05F"), TEXT("05G"), TEXT("06A"), TEXT("06B"), TEXT("06C"), TEXT("06D"), TEXT("06E"),
		TEXT("06F"), TEXT("06G"), TEXT("07A"), TEXT("07C"), TEXT("07D"), TEXT("07E"), TEXT("08B"), TEXT("08C"),  TEXT("08D") };

	_wstring m_FieldList[84] = {
		TEXT("05A"), TEXT("05B"), TEXT("05C"),TEXT("05D"), TEXT("05E"), TEXT("05F"), TEXT("05G"), TEXT("06A"), TEXT("06B"), TEXT("06C"),
		TEXT("06D"), TEXT("06E"), TEXT("06F"),TEXT("06G"), TEXT("07A"), TEXT("07B"), TEXT("07C"), TEXT("07D"), TEXT("07E"), TEXT("07F"),
		TEXT("07G"), TEXT("08A"), TEXT("08B"),TEXT("08C"), TEXT("08D"), TEXT("08E"), TEXT("08F"), TEXT("08G"), TEXT("09A"), TEXT("09B"),
		TEXT("09C"), TEXT("09D"), TEXT("09E"),TEXT("09F"), TEXT("09G"), TEXT("10A"), TEXT("10B"), TEXT("10C"), TEXT("10D"), TEXT("10E"),
		TEXT("10F"), TEXT("10G"), TEXT("11A"),TEXT("11B"), TEXT("11C"), TEXT("11D"), TEXT("11E"), TEXT("11F"), TEXT("11G"), TEXT("12A"),
		TEXT("12B"), TEXT("12C"), TEXT("12D"),TEXT("12E"), TEXT("12F"), TEXT("12G"), TEXT("13A"), TEXT("13B"), TEXT("13C"), TEXT("13D"),
		TEXT("13E"), TEXT("13F"), TEXT("13G"),TEXT("14A"), TEXT("14B"), TEXT("14C"), TEXT("14D"), TEXT("14E"), TEXT("14F"), TEXT("14G"),
		TEXT("15A"), TEXT("15B"), TEXT("15C"),TEXT("15D"), TEXT("15E"), TEXT("15F"), TEXT("15G"), TEXT("16A"), TEXT("16B"), TEXT("16C"),
		TEXT("16D"), TEXT("16E"), TEXT("16F"),TEXT("16G")
	};

public:
	static CLand* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};


