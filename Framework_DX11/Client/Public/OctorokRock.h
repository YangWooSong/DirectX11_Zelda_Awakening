#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
class CNavigation;
END

BEGIN(Client)
class COctorokRock :
	public CGameObject
{
public:
	typedef struct : public CGameObject::GAMEOBJECT_DESC
	{
		_uint iDir;
	} OCTOROKROCK_DESC;

public:
	enum OCTOROKROCK_DIR { FRONT, BACK, LEFT, RIGHT, OCTOROKROCK_DIR_END};
private:
	COctorokRock(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	COctorokRock(const COctorokRock& Prototype);
	virtual ~COctorokRock() = default;

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
	class CNavigation* m_pNavigationCom = { nullptr };
private:
	HRESULT Ready_Components();
	void Set_CellNum(_int iNum) { m_iCellNum = iNum; }

private:
	_float m_fSpeed = {  };
	_uint	m_iRockDir = { OCTOROKROCK_DIR_END };

	_float3 m_fOffset = {};
public:
	static COctorokRock* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};
END
