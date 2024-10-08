#pragma once
#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CCollider;
class CFsm;
class CModel;
class CShader;
class CSound;
END

BEGIN(Client)

class CBomb final : public CPartObject
{
public:
	typedef struct : public CPartObject::PARTOBJ_DESC
	{
		_uint iLevelIndex = { LEVEL_END };
		const _float4x4* pSocketBoneMatrix = { nullptr };
	}BOMB_DESC;
private:
	CBomb(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBomb(const CBomb& Prototype);
	virtual ~CBomb() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CCollider* m_pColliderCom = { nullptr };
	CSound* m_pSoundCom = { nullptr };

private:
	HRESULT Ready_Components();
	HRESULT Ready_Effects();

private:
	const _float4x4* m_pSocketMatrix = { nullptr };
	_bool m_bPut = { false };
	_float m_fPutTimer = { 0.f };
	_float fExplodeTimer = { 0.f };
	_float m_fRed = { 1.f };

	_int m_iCurrentAnimIndex = {0};
	_uint m_iLevel_Index = { LEVEL_END };
	CGameObject* m_pEffects[2] = {};

private:
	void Red_Blink();

public:
	static CBomb* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END