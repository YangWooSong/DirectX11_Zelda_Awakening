#pragma once
#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CRola_Hand_Effect final : public CPartObject
{
public:
	typedef struct : public CPartObject::PARTOBJ_DESC
	{
		const _float4x4* pSocketBoneMatrix = { nullptr };
		CGameObject* m_pParent;
	}ROLA_HAND_DESC;

private:
	CRola_Hand_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRola_Hand_Effect(const CRola_Hand_Effect& Prototype);
	virtual ~CRola_Hand_Effect() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
private:
	const _float4x4* m_pSocketMatrix = { nullptr };
	_bool m_bCreateEffect = { false };
	CGameObject* m_pEffect = { nullptr };
	CGameObject* m_pParent = { nullptr };

private:
	HRESULT Ready_Components();

public:
	static CRola_Hand_Effect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END