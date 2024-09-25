#include "PhysX_Manager.h"

#include "Transform.h"

#include "GameObject.h"
#include "Model.h"
#include "Mesh.h"

CPhysX_Manager::CPhysX_Manager()
{
}

HRESULT CPhysX_Manager::Initialize()
{
    // PhysX Foundation ����
    // PhysX�� ���õ� ��� ��ü���� ����� �Ǵ� �̱��� ��ü
    m_PxFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_PxAllocator, m_PXErrorCallback);

    // PVD(PhysX Visual Debugger) ����
    m_Pvd = PxCreatePvd(*m_PxFoundation);
    // PVD ���� ���� ����
    m_pTransport = PxDefaultPvdSocketTransportCreate(m_pvdIPAddress.c_str(), m_iPvdPortNumber, m_iPvdTimeOutSeconds);
    m_Pvd->connect(*m_pTransport, PxPvdInstrumentationFlag::eALL);

    // PhysX SDK ����
    // ������ ��ü���� ������ �� �ִ� �̱��� ���丮 Ŭ����
    // ���̳� ���͸����� �����ϰų� ���� ��ü�� ��ü ����(Shape), ��ü �ν��Ͻ� �� ���� ��κ��� PhysX ��ü���� ������ �� ���
    m_PhysX = PxCreatePhysics(PX_PHYSICS_VERSION, *m_PxFoundation, PxTolerancesScale(), true, m_Pvd);
    PxInitExtensions(*m_PhysX, m_Pvd);

    // �� ���� ����
    PxSceneDesc sceneDesc(m_PhysX->getTolerancesScale());
    sceneDesc.gravity = PxVec3(0.0f, 0.0f, 0.0f);

    // CPU ����ó(��Ŀ ������) ����
    m_PxDispatcher = PxDefaultCpuDispatcherCreate(2);
    sceneDesc.cpuDispatcher = m_PxDispatcher;
    sceneDesc.filterShader = PxDefaultSimulationFilterShader;

    // �� ���� PhysX ���͵��� ��ġ�� �� �ִ� ��
    m_PxScene = m_PhysX->createScene(sceneDesc);
    
    // PVD Ŭ���̾�Ʈ ����
    // Physx Visual Debugger ���α׷��� �� ���� ������ ������ �� � ������ ������ ���� �÷��׸� �������ִ� ��ü
    PxPvdSceneClient* PvdClient = m_PxScene->getScenePvdClient();
    if (PvdClient)
    {
        // PVD Ŭ���̾�Ʈ �÷��� ����
        PvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true); //��ü ����
        PvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true); // �浹 ����
        PvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true); // Scene ����
    }

    // Material�� �������� ǥ�� ������ ���¸� ��Ÿ���� ��ü
    // ǥ���� �����������, ��������, �ݹ߰�� �� ǥ�� ������ �پ��� ������ ������ �� �ִ�
    //gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.6f);

    //PxRigidStatic* groundPlane = PxCreatePlane(*gPhysics, PxPlane(0, 1, 0, 0), *gMaterial);
    //gScene->addActor(*groundPlane);

    return S_OK;
}

void CPhysX_Manager::PhysX_Update(_float fTimeDelta)
{
    if (nullptr == m_pPlayer)
        return;

    PxVec3  vDir;
    PxReal  fLength;

    CTransform* PlayerTransformCom = m_pPlayer->Get_Transform();
    Safe_AddRef(PlayerTransformCom);

    _matrix PlayerWorldMatirx = PlayerTransformCom->Get_WorldMatrix();

    _vector vUp = PlayerWorldMatirx.r[1];
    vUp = XMVector3Normalize(vUp);

    _vector vPos = PlayerWorldMatirx.r[3];

    _vector vStartPos = vPos + vUp * -0.1f;
    _vector vEndPos = vPos + vUp * 1.5f;

    PxVec3 PxStartPos = { XMVectorGetX(vStartPos), XMVectorGetY(vStartPos), XMVectorGetZ(vStartPos) };
    PxVec3 PxEndPos = { XMVectorGetX(vEndPos), XMVectorGetY(vEndPos), XMVectorGetZ(vEndPos) };

    PxTransform PxPlayerTransform = PxTransformFromSegment(PxStartPos, PxEndPos);

    _bool isCollision = false;
    _bool isGround = false;

    for (auto& Mesh : m_ColMesheGeometries)
    {
        if (Mesh == nullptr)
            return;

        // �浹 ����, �浹 ����, �˻��� ����(EX: ĸ��), 
        if (PxComputeTriangleMeshPenetration(vDir, fLength, PxCapsuleGeometry(0.5f, 0.3f), PxPlayerTransform, *Mesh, PxTransform(PxIDENTITY::PxIdentity), 1))
        {
            // �浹�� ���̰� 0.1f �̻��̸�
            if (fLength > 0.1f)
            {
                isCollision = true;

                // �浹�� ����
                _vector vResultDir = XMVectorSet(vDir.x, vDir.y, vDir.z, 0.f);
                vResultDir = XMVector3Normalize(vResultDir);

                // Up ���Ϳ� �浹�� ���� ���͸� ����
                _vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
                vUp = XMVector3Dot(vUp, vResultDir);

                _float vDownFloat;
                XMStoreFloat(&vDownFloat, vUp);

                // �浹 ������ 45�� �Ʒ��� ���� �پ��ִ°ɷ� �Ǵ�
                _float fAngle = XMConvertToDegrees(acosf(vDownFloat));
                if (fAngle <= 45.0f)
                {
                    isGround = true;
                }

                fLength -= 0.1f;

                vResultDir *= fLength;

                _vector vPlayerPos = PlayerTransformCom->Get_State(CTransform::STATE_POSITION);

                vPlayerPos += vResultDir;

                PlayerTransformCom->Set_State(CTransform::STATE_POSITION, vPlayerPos);
            }
        }
    }

    m_pPlayerRigid->setGlobalPose(PxPlayerTransform);

    // ���� �� ������Ʈ
    m_PxScene->simulate(fTimeDelta);
    m_PxScene->fetchResults(true);

    Safe_Release(PlayerTransformCom);
}

HRESULT CPhysX_Manager::AddScene_ColMesh(CGameObject* pObject, _wstring strModelName)
{
    // ���� ��ü�� �� ������Ʈ�� ����
    CModel* pModel = dynamic_cast<CModel*>(pObject->Find_Component(MODEL));

    // ���� �� �޽��� ���� ó��
    for (auto* Mesh : pModel->Get_Meshes())
    {
        VTXMESH* pVB = Mesh->Get_Vertices();  // ���� ������
        VTXANIMMESH* pAnimVB = Mesh->Get_AnimVertices();  // ���� ������
        _uint* pIB = Mesh->Get_Indices();  // �ε��� ������

        _uint iNumVertices = Mesh->Get_NumVertices();  // ���� ����
        _uint iNumIndices = Mesh->Get_NumIndices();  // �ε��� ����
        _uint iNumTriangles = iNumIndices / 3;  // �ﰢ�� ����

        // ���� ��ǥ�� ������ �������� ����
        vector<PxVec3> Vertices;
        Vertices.reserve(iNumVertices);

        // ���� ��ü�� ���� ��ȯ ���
        _matrix WorldMatrix = pObject->Get_Transform()->Get_WorldMatrix();

        // ���� ��ǥ ��ȯ �� ����
        for (_uint i = 0; i < iNumVertices; ++i)
        {
            _vector vVertexPosition = {};
            if (nullptr == pVB)
                vVertexPosition = XMLoadFloat3(&pAnimVB[i].vPosition);
            else
                vVertexPosition = XMLoadFloat3(&pVB[i].vPosition);

            vVertexPosition = XMVector3TransformCoord(vVertexPosition, WorldMatrix);
            Vertices.push_back(PxVec3(XMVectorGetX(vVertexPosition), XMVectorGetY(vVertexPosition), XMVectorGetZ(vVertexPosition)));
        }

        // �ε����� ������ ����
        vector<PxU32> Indices;
        Indices.reserve(iNumIndices);

        // �ε��� ������ ����
        for (_uint i = 0; i < iNumIndices; ++i)
        {
            _uint Index = pIB[i];
            Indices.push_back(Index);
        }

        // �ﰢ�� �޽� ���� ����
        PxTriangleMeshDesc tDesc;
        tDesc.points.count = iNumVertices;
        tDesc.points.stride = sizeof(PxVec3);
        tDesc.points.data = Vertices.data();
        tDesc.triangles.count = iNumTriangles;
        tDesc.triangles.stride = sizeof(PxU32) * 3;
        tDesc.triangles.data = Indices.data();

        // �ﰢ�� �޽� ����
        PxTriangleMesh* pTriangleMesh = PxCreateTriangleMesh(PxCookingParams(PxTolerancesScale(0.0f, 0.0f)), tDesc);

        // �ﰢ�� �޽� ���� ����
        PxTriangleMeshGeometry* pGeometry = new PxTriangleMeshGeometry(pTriangleMesh);

        // �浹 �޽� ��Ͽ� �߰�
        m_ColMesheGeometries.push_back(pGeometry);

        // RigidStatic ��ü ����
        // �������� �ʴ� ���� �𵨵��� �̰ŷ� ��������
        PxRigidStatic* pActor = m_PhysX->createRigidStatic(Get_PhysXTransform(pObject));

        // ���� ���� ����
        PxMaterial* Material = m_PhysX->createMaterial(0.5f, 0.5f, 0.6f);
        // �浹 ���� ���� �� �߰�
        PxShape* pShape = m_PhysX->createShape(*pGeometry, *Material);
        if(pActor!=nullptr)
        {
            pActor->attachShape(*pShape);
            // ���� ���� Actor �߰�
            m_PxScene->addActor(*pActor);

            // ���� Actor ��Ͽ� �߰�
            m_StaticActors.push_back(pActor);
        }

    }
    return S_OK;
}

HRESULT CPhysX_Manager::SetUp_Player(CGameObject* pPlayer)
{
    PxRigidDynamic* pRigid = m_PhysX->createRigidDynamic(Get_PhysXTransform(pPlayer));

    // ���׸��� ȯ�� ����
    // ���� ����, ���� ����, �ݹ� ���
    PxMaterial* Material = m_PhysX->createMaterial(0.5f, 0.5f, 0.6f);

    // �������� 0.5�̰� ���̰� 0.3�� ĸ�� ����
    PxShape* pShape = m_PhysX->createShape(PxCapsuleGeometry(0.5f, 0.3f), *Material);

    pRigid->attachShape(*pShape);

    m_PxScene->addActor(*pRigid);
    m_pPlayerRigid = pRigid;

    m_pPlayer = pPlayer;

    return S_OK;
}

HRESULT CPhysX_Manager::Destroy_PhysXActor(CGameObject* pObject)
{
    if (nullptr == pObject)
        return E_FAIL;

    _vector vObjectPos = pObject->Get_Transform()->Get_State(CTransform::STATE_POSITION);

    _uint iObjectIndex = 0;
    for (auto& pActor : m_StaticActors)
    {
        _vector vActorPos = XMVectorSet(pActor->getGlobalPose().p.x, pActor->getGlobalPose().p.y, pActor->getGlobalPose().p.z, 1.f);

        _float fDistanceSquared = XMVectorGetX(XMVector3LengthSq(XMVectorSubtract(vObjectPos, vActorPos)));

        if (fDistanceSquared < 0.0001f)
            break;

        ++iObjectIndex;
    }

    if (iObjectIndex >= m_StaticActors.size())
        return E_FAIL;

    PxTriangleMeshGeometry* pGeometry = m_ColMesheGeometries[iObjectIndex];
    PhysX_RELEASE(pGeometry->triangleMesh);
    Safe_Delete(pGeometry);

    PxRigidStatic* pActor = m_StaticActors[iObjectIndex];
    PhysX_RELEASE(pActor);

    m_ColMesheGeometries.erase(m_ColMesheGeometries.begin() + iObjectIndex);
    m_StaticActors.erase(m_StaticActors.begin() + iObjectIndex);

    return S_OK;
}

void CPhysX_Manager::DeleteActors()
{
    for (auto& Geometry : m_ColMesheGeometries)
    {
        PhysX_RELEASE(Geometry->triangleMesh);
        Safe_Delete(Geometry);
    }

    for (auto& Actor : m_StaticActors)
    {
        PhysX_RELEASE(Actor);
    }

    m_ColMesheGeometries.clear();
    m_StaticActors.clear();
}

PxTransform CPhysX_Manager::Get_PhysXTransform(CGameObject* pObject)
{
    //�̰� �³�?
    //��ư �������� Transfrom���� ��ȯ
    _float4x4 WorldMat = pObject->Get_Transform()->Get_World4x4();
    return PxTransform(PxMat44(&WorldMat._11));
}

CPhysX_Manager* CPhysX_Manager::Create()
{
    CPhysX_Manager* pInstance = new CPhysX_Manager();

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX(TEXT("Failed to Created : CPhysX_Manager"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPhysX_Manager::Free()
{
    __super::Free();

    for (auto& Geometry : m_ColMesheGeometries)
    {
        if(Geometry  != nullptr)
        {
            PhysX_RELEASE(Geometry->triangleMesh);
        }
        Safe_Delete(Geometry);
    }

    for (auto& Actor : m_StaticActors)
    {
        PhysX_RELEASE(Actor);
    }

    PhysX_RELEASE(m_PxScene);

    // PhysX Extensions �ݱ�
    PxCloseExtensions();

    PhysX_RELEASE(m_PxDispatcher);
    PhysX_RELEASE(m_PhysX);
    PhysX_RELEASE(m_pTransport);
    PhysX_RELEASE(m_Pvd);

    // ���� �������� Release
    PhysX_RELEASE(m_PxFoundation);


}


//else
//{
//    // ���� �ε��� ��� �� ��ġ�� ����
//    _vector vPlayerPos = PlayerTransformCom->Get_State(CTransform::STATE_POSITION);

//    // �浹 �������� �÷��̾� ��ġ�� �̼��ϰ� �����Ͽ� ���� ����
//    vPlayerPos += vResultDir * fLength;

//    // �÷��̾��� ������ ���� �°� ȸ��
//    _vector vForward = XMVector3Cross(vResultDir, vUp);
//    vForward = XMVector3Normalize(vForward);
//    _vector vNewUp = XMVector3Cross(vForward, vResultDir);
//    vNewUp = XMVector3Normalize(vNewUp);

//    // ���ο� ȸ�� ����� ���
//    _matrix RotationMatrix = XMMatrixIdentity();
//    RotationMatrix.r[0] = XMVectorSetW(vForward, 0.f);
//    RotationMatrix.r[1] = XMVectorSetW(vNewUp, 0.f);
//    RotationMatrix.r[2] = XMVectorSetW(vResultDir, 0.f);
//    RotationMatrix.r[3] = XMVectorSet(0.f, 0.f, 0.f, 1.f);

//    PlayerTransformCom->Set_WorldMatrix(RotationMatrix);

//    // ��ġ�� ����
//    PlayerTransformCom->Set_State(CTransform::STATE_POSITION, vPlayerPos);
//    }