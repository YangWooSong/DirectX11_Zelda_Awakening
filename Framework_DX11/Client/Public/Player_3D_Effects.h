#pragma once
#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CShader;
class CFsm;
class CModel;
END

BEGIN(Client)

class CPlayer_3D_Effects final : public CPartObject
{
public:
	enum EFFECT_TYPE{SWISH,CHARGE_SLASH, TYPE_END};
	typedef struct : public CPartObject::PARTOBJ_DESC
	{
		CFsm* pPlayerFsm;
		const _float4x4* pSocketBoneMatrix = { nullptr };
	}PLAYER_3D_DESC;

private:
	CPlayer_3D_Effects(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer_3D_Effects(const CPlayer_3D_Effects& Prototype);
	virtual ~CPlayer_3D_Effects() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Set_Type(_uint iType) { m_iEffectType = iType; }
private:
	CFsm* m_pPlayerFsm = { nullptr };
	CModel* m_pModelCom[2] = {nullptr};
	CShader* m_pShaderCom = { nullptr };
private:
	const _float4x4* m_pSocketMatrix = { nullptr };
	_uint m_iEffectType = { 0 };
	_bool m_bRender = { false };
	_bool m_bMove = { false };
	_bool m_bEnd = { false };
	_float m_fAlpha = {};
	_float m_bStoptimer = {0.f};
	_float m_bActivetimer = {0.f};
	_float m_fBright = {};
	_float m_fTextureScale = { 0.1f };
	_float3 m_fColor = {1.f,1.f,1.f};
private:
	HRESULT Ready_Components();

public:
	static CPlayer_3D_Effects* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END