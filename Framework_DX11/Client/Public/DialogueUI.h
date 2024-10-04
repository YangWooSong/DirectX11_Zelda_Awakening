#pragma once
#include "Client_Defines.h"
#include "UIObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)
class CDialogueUI :
	public CUIObject
{
public:
	enum NPC_TYPE {TOOTOOLSHOPKEEPER, TYPE_END};
	typedef struct : public CUIObject::UI_DESC
	{
		_uint iOwnerType;
	} DIALOGUE_DESC;
private:
	CDialogueUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDialogueUI(const CDialogueUI& Prototype);
	virtual ~CDialogueUI() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Set_LineNum(_int iIndex) { iLineIndex = iIndex; }
private:
	HRESULT Ready_Components();
	HRESULT Render_Text();
public:
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

private:
	_uint iOwnerType = { TYPE_END };	
	_int iLineIndex = { 0 };
	//vector<const _tchar*> m_Text = {TEXT("������ ������ ����."),TEXT("��ź�� 5���Ǿ�.") ,TEXT("��Ʈ�� 10���Ǿ�.") };

private:
	HRESULT Ready_ChildUI();
public:
	static CDialogueUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END;
