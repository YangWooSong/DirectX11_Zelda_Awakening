#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
class CCollider;
class CSound;
END


BEGIN(Client)
class CGrass :
	public CGameObject
{
public:
	enum GRASS_TYPE{GRASS, LAWN, GRASS_TYPE_END};
	typedef struct : public CGameObject::GAMEOBJECT_DESC
	{
		_uint iGrassType = 0;
	} GRASS_DESC;
private:
	CGrass(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGrass(const CGrass& Prototype);
	virtual ~CGrass() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void OnCollisionEnter(CGameObject* pOther)  override;
	virtual void OnCollisionStay(CGameObject* pOther) override;
	virtual void OnCollisionExit(CGameObject* pOther)  override;

public:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CCollider* m_pColliderCom = { nullptr };
	CSound* m_pSoundCom = { nullptr };

private:
	_uint m_iGrassType = { GRASS_TYPE_END };

	_bool	m_bCut = { false };
	_bool   m_bAddColliderList = { false };
	_bool m_bPlaySound = { false };
	_bool m_bCreate_Lupee = { false };
	CGameObject* m_pParticle = { nullptr };
	class CPlayer* m_pPlayer = { nullptr };
private:
	HRESULT Ready_Components();
	HRESULT Ready_Particle();
	void Culculate_Distance_Player();
	HRESULT Drop_Lupee();

public:
	static CGrass* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END