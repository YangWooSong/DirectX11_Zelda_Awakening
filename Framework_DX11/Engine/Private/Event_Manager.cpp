#include "../Public/Event_Manager.h"
#include "../Public/GameObject.h"

#include "Level_Manager.h"
#include "Object_Manager.h"
#include "UI_Manager.h"

#include "Level.h"
#include "UIObject.h"

CEvent_Manager::CEvent_Manager(CLevel_Manager* pLevelMgr, CObject_Manager* pObjectMgr, CUI_Manager* pUIMgr)
	: m_pLevel_Manager(pLevelMgr)
	, m_pObject_Manager(pObjectMgr)
	, m_pUI_Manager(pUIMgr)
{
	Safe_AddRef(m_pLevel_Manager);
	Safe_AddRef(m_pObject_Manager);
	Safe_AddRef(m_pUI_Manager);
}

HRESULT CEvent_Manager::Initialize()
{
	return S_OK;
}

// �Ͼ �̺�Ʈ���� ���� ��Ƶα� ����
// �������� ���� �� ���� ������ �����ӿ� �����
void CEvent_Manager::Update()
{
	// ���� �̺�Ʈ�� �� �������� �и�
	if (m_Deadlist.size() > 0)
	{
		for (auto& pDeadObject : m_Deadlist)
			Safe_Release(pDeadObject);
		m_Deadlist.clear();
	}

	// ���� �Ͼ �̺�Ʈ���� ó����
	for (auto& Event = m_Eventlist.begin(); Event != m_Eventlist.end();)
	{

		Excute(*Event);

		if (nullptr != Event->CloneDesc)
		{
			Safe_Delete(Event->CloneDesc->pArg);
			Safe_Delete(Event->CloneDesc);
		}

		Event = m_Eventlist.erase(Event);
	}
	m_Eventlist.clear();



	/* �̷� ��ȯ�ΰ���

	1. ������Ʈ ���߿� ��𼱰� Add_Event�� �̺�Ʈ�� �߰���
	2. �����Ǵ� ������Ʈ�� ����Ʈ�� ���� �˻��ϰ� ������ ������ -> �� ������ �и�����
	3. �� �ڿ� ����, ���� ��ȯ ���� �̺�Ʈ��  Excute�� ������
	4. ������ ���� �̺�Ʈ�� Excute���� �����ϴ°� �ƴ϶� ���� ����Ʈ�� �־��ִ� �۾����� ������
	5. ���� ������ Update���� ���� ����Ʈ�� �ִ� ���� ���������� ������

	*/
}


// ����Ʈ�� ��Ƶ� �̺�Ʈ���� ������ �����ϴ� �Լ�
void CEvent_Manager::Excute(const EVENT tEvent)
{
	switch ((UINT)tEvent.eEven)
	{
	case (UINT)EVENT_TYPE::CREATE_OBJECT:
	{
		// lParam : �Ⱦ�
		// wParam : �Ⱦ�
		// pArg   : Clone_Desc ���� -> �츮�� clone�� ����
		const CLONE_DESC* CloneDesc = tEvent.CloneDesc;

		m_pObject_Manager->Add_CloneObject_ToLayer(CloneDesc->iLevelIndex, CloneDesc->strLayerTag, CloneDesc->strPrototypeTag, CloneDesc->pArg);
	}
	break;

	case (UINT)EVENT_TYPE::DELETE_OBJECT:
	{
		// lParam : Object adress		-> ������ ������Ʈ�� �ּ�
		// wParam: Group Type			-> ������Ʈ�� � �׷�����(�ٵ� �� ��)

		CGameObject* pDeadObj = (CGameObject*)tEvent.lParam;

		// Ȥ�� �������� ���°� ����
		if (!pDeadObj->Get_Dead())
		{		
			// Object�� Dead���·� ����
			// ���⼭ �� ������ �и��ϱ� Dead�� ���� ó���� ��������
			pDeadObj->Set_Dead(true);

			// �������� ������Ʈ���� ��Ƶд�
			m_Deadlist.push_back(pDeadObj);
		}

		// �������� foucs ����
		if (nullptr != dynamic_cast<CUIObject*>(pDeadObj))	// UI�� �Ҹ���
		{
			m_pUI_Manager->Set_FocusUI(nullptr);
		}

	}
	break;


	case (UINT)EVENT_TYPE::CHANGE_LEVEL:
	{
		// lParam:   Level Index
		// wParam :  Next Scene Type
		m_pLevel_Manager->Change_Level((_uint)tEvent.lParam, (CLevel*)tEvent.wParam);

		//m_pUI_Manager->SetFocusUI(nullptr);
	}
	break;

	case (UINT)EVENT_TYPE::CHANGE_STATE:
	{
		// lParam : AI
		// wParam : Next Type

	}
	break;
	}
}



CEvent_Manager* CEvent_Manager::Create(CLevel_Manager* pLevelMgr, CObject_Manager* pObjectMgr, CUI_Manager* pUIMgr)
{
	CEvent_Manager* pInstance = new CEvent_Manager(pLevelMgr, pObjectMgr, pUIMgr);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CEvent_Manager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_Manager::Free()
{
	__super::Free();

	Safe_Release(m_pLevel_Manager);
	Safe_Release(m_pObject_Manager);
	Safe_Release(m_pUI_Manager);


	for (auto& pDeadObject : m_Deadlist)
		Safe_Release(pDeadObject);
	m_Deadlist.clear();

	m_Eventlist.clear();
}



