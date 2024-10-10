#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
END


BEGIN(Client)
class C3D_Effects :
	public CGameObject
{
public:
	enum EFFECT_TYPE { MONSTER_HIT, PLAYER_ITEM_GET, PLAYER_SWISH, TYPE_END };
	typedef struct : public CGameObject::GAMEOBJECT_DESC
	{
		const _float4x4* pParentWorldMatrix = { nullptr };
		_uint iEffectType = { TYPE_END };
		_uint iLevelIndex = { LEVEL_END };
	} MODEL_EFFECT_DESC;
protected:
	C3D_Effects(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	C3D_Effects(const C3D_Effects& Prototype);
	virtual ~C3D_Effects() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };

protected:
	_uint m_iEffectType = { TYPE_END };
	_float m_fSize = { 0.5f };
	_float m_fAlpha = { 1.f };
	_float m_fBright = { 1.f };
	_uint m_iLevelIndex = { LEVEL_END };
	const _float4x4* m_pParentMatrix = { nullptr };
	_float4x4				m_WorldMatrix = {};
private:
	HRESULT Ready_Components();
	void Monster_HIt_SizeUp(_float fTimeDelta);
public:
	static C3D_Effects* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END