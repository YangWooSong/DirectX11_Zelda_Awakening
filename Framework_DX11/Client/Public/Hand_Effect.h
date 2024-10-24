#pragma once
#include "Client_Defines.h"
#include "2DEffects.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)
class CHand_Effect :
	public C2DEffects
{
public:
	typedef struct : public C2DEffects::EFFECT_DESC
	{
		const _float4x4* pSocketBoneMatrix = { nullptr };
		const _float4x4* pParentWorldMatrix = { nullptr };
	} HAND_EFFECT_DESC;
private:
	CHand_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CHand_Effect(const CHand_Effect& Prototype);
	virtual ~CHand_Effect() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_float3 m_vOffset = {};
	_float3 m_vNewPos = {};
	const _float4x4* m_pSocketBoneMatrix = { nullptr };
	_float4x4				m_WorldMatrix = {};
	const _float4x4* m_pParentMatrix = { nullptr };
private:
	HRESULT Ready_Components();

public:
	static CHand_Effect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END;
