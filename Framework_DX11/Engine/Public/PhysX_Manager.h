#pragma once
#include "Base.h"

// ����� ��忡�� ���� ������� crtdebug ������ PhysX�� new �����ǿ��� ������ ��
// ������  ��������� �����ϱ� ���� undef�� new �����Ǹ� ��� ������
#ifdef _DEBUG
#undef new
#endif


#include "PxPhysics.h"
#include "PxPhysicsAPI.h"

// PhysX ��������� ������ �� ��ũ�� �����Ǹ� �ٽ� Ȱ��ȭ
#ifdef _DEBUG
#define new DBG_NEW
#endif

using namespace physx;

BEGIN(Engine)

class CTransform;
class CGameObject;

class ENGINE_DLL CPhysX_Manager :
    public CBase
{
    CPhysX_Manager();
    virtual ~CPhysX_Manager() = default;

public:
    HRESULT Initialize();
    void    PhysX_Update(_float fTimeDelta);

public:
    HRESULT AddScene_ColMesh(class CGameObject* pObject, _wstring strModelName);
    HRESULT SetUp_Player(class CGameObject* pPlayer);

    HRESULT Destroy_PhysXActor(CGameObject* pObject);   //��۾� �̰ž�

    void DeletePlayer() { m_pPlayer = nullptr; }
    void DeleteActors();
private:
    // PhysX Foundation ��ü - PhysX �ý����� �⺻ �ν��Ͻ�. �޸� �Ҵ��ڿ� ���� �ݹ��� �ʱ�ȭ�ϴµ� ���
    PxFoundation* m_PxFoundation = { nullptr };

    // PVD(PhysX Visual Debugger) ��ü - PhysX�� ����� ������ �ð������� �� �� �ְ� ���ִ� ����
    PxPvd* m_Pvd = { nullptr };
    PxPvdTransport* m_pTransport = { nullptr };

    // PhysX SDK ��ü - ���� ������ �ٽ�. ���� �ùķ��̼��� ���� ��� ���� �� ���� ���
    PxPhysics* m_PhysX = { nullptr };

    // ���� �� ��ü - ���� �ùķ��̼��� �̷������ ����. �浹, �߷�, �� ���� ��ȣ�ۿ��� ó��
    PxScene* m_PxScene = { nullptr };
   
    // PhysX �Ҵ��� �� ���� �ݹ� - �޸� �Ҵ�� ���� ó���� ���� �ݹ� Ŭ����
    PxDefaultAllocator m_PxAllocator;
    PxDefaultErrorCallback m_PXErrorCallback;

    // PVD ���� ���� - ����� ������ ������ IP �ּ�
    std::string m_pvdIPAddress = "127.0.0.1";

    // ����ϱ� ���� ��Ʈ��ȣ - PVD�� ����ϴ� ��Ʈ ��ȣ - �⺻ 5425
    _int m_iPvdPortNumber = 5425;

    // 10�� �ȿ� ���� �� �Ǹ� ������ - PVD ���� Ÿ�Ӿƿ� ����
    _uint m_iPvdTimeOutSeconds = 10;

    // CPU ����ó(��Ŀ ������) ��ü - ���� �ùķ��̼��� ���ķ� ó���ϱ� ���� CPU ����ó
    PxDefaultCpuDispatcher* m_PxDispatcher = { nullptr };

    vector<PxTriangleMeshGeometry*> m_ColMesheGeometries;

    // �ӽ÷� ���Ǵ� ���� Actor ���
    vector<PxRigidStatic*> m_StaticActors;

    class CGameObject* m_pPlayer = { nullptr };
    PxRigidDynamic* m_pPlayerRigid = { nullptr };

    // PxRigidDynamic - ��ü
    // PxTriangleMeshGeometry - �ﰢ�� �޽� ������ 
    // PxCapsuleGeometry - ĸ�� ������ 

private:
    // �־��� ���� ������Ʈ�� PhysX ��ȯ ������ ��� ���� �Լ�
    PxTransform Get_PhysXTransform(CGameObject* pObject);


public:
    static CPhysX_Manager* Create();
    virtual void Free() override;
};

END

/* PhysX ���̺귯�� ���ϵ�
PhysX_64.lib
PhysXCommon_64.lib
PhysXCooking_64.lib
PhysXExtensions_static_64.lib
PhysXFoundation_64.lib
PhysXPvdSDK_static_64.lib */