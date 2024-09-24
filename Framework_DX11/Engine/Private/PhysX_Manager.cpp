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
    // PhysX Foundation 생성
    // PhysX와 관련된 모든 객체들의 기반이 되는 싱글톤 객체
    m_PxFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_PxAllocator, m_PXErrorCallback);

    // PVD(PhysX Visual Debugger) 생성
    m_Pvd = PxCreatePvd(*m_PxFoundation);
    // PVD 소켓 연결 설정
    m_pTransport = PxDefaultPvdSocketTransportCreate(m_pvdIPAddress.c_str(), m_iPvdPortNumber, m_iPvdTimeOutSeconds);
    m_Pvd->connect(*m_pTransport, PxPvdInstrumentationFlag::eALL);

    // PhysX SDK 생성
    // 피직스 객체들을 생성할 수 있는 싱글톤 팩토리 클래스
    // 씬이나 매터리얼을 생성하거나 물리 객체의 형체 정보(Shape), 강체 인스턴스 등 거의 대부분의 PhysX 객체들을 생성할 때 사용
    m_PhysX = PxCreatePhysics(PX_PHYSICS_VERSION, *m_PxFoundation, PxTolerancesScale(), true, m_Pvd);
    PxInitExtensions(*m_PhysX, m_Pvd);

    // 씬 설명 설정
    PxSceneDesc sceneDesc(m_PhysX->getTolerancesScale());
    sceneDesc.gravity = PxVec3(0.0f, 0.0f, 0.0f);

    // CPU 디스패처(워커 스레드) 생성
    m_PxDispatcher = PxDefaultCpuDispatcherCreate(2);
    sceneDesc.cpuDispatcher = m_PxDispatcher;
    sceneDesc.filterShader = PxDefaultSimulationFilterShader;

    // 씬 생성 PhysX 액터들이 배치될 수 있는 씬
    m_PxScene = m_PhysX->createScene(sceneDesc);
    
    // PVD 클라이언트 설정
    // Physx Visual Debugger 프로그램에 한 씬의 정보를 전달할 때 어떤 정보를 전달해 줄지 플래그를 설정해주는 객체
    PxPvdSceneClient* PvdClient = m_PxScene->getScenePvdClient();
    if (PvdClient)
    {
        // PVD 클라이언트 플래그 설정
        PvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true); //강체 관련
        PvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true); // 충돌 관련
        PvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true); // Scene 쿼리
    }

    // Material은 물리적인 표면 재질의 상태를 나타내는 객체
    // 표면의 정지마찰계수, 운동마찰계수, 반발계수 등 표면 재질의 다양한 성질을 지정할 수 있다
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

        // 충돌 방향, 충돌 깊이, 검사할 형태(EX: 캡슐), 
        if (PxComputeTriangleMeshPenetration(vDir, fLength, PxCapsuleGeometry(0.5f, 0.3f), PxPlayerTransform, *Mesh, PxTransform(PxIDENTITY::PxIdentity), 1))
        {
            // 충돌한 깊이가 0.1f 이상이면
            if (fLength > 0.1f)
            {
                isCollision = true;

                // 충돌한 방향
                _vector vResultDir = XMVectorSet(vDir.x, vDir.y, vDir.z, 0.f);
                vResultDir = XMVector3Normalize(vResultDir);

                // Up 벡터와 충돌한 방향 벡터를 내적
                _vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
                vUp = XMVector3Dot(vUp, vResultDir);

                _float vDownFloat;
                XMStoreFloat(&vDownFloat, vUp);

                // 충돌 각도가 45도 아래면 땅에 붙어있는걸로 판단
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

    // 물리 씬 업데이트
    m_PxScene->simulate(fTimeDelta);
    m_PxScene->fetchResults(true);

    Safe_Release(PlayerTransformCom);
}

HRESULT CPhysX_Manager::AddScene_ColMesh(CGameObject* pObject, _wstring strModelName)
{
    // 게임 객체의 모델 컴포넌트를 얻음
    CModel* pModel = dynamic_cast<CModel*>(pObject->Find_Component(MODEL));

    // 모델의 각 메쉬에 대해 처리
    for (auto* Mesh : pModel->Get_Meshes())
    {
        VTXMESH* pVB = Mesh->Get_Vertices();  // 정점 데이터
        VTXANIMMESH* pAnimVB = Mesh->Get_AnimVertices();  // 정점 데이터
        _uint* pIB = Mesh->Get_Indices();  // 인덱스 데이터

        _uint iNumVertices = Mesh->Get_NumVertices();  // 정점 개수
        _uint iNumIndices = Mesh->Get_NumIndices();  // 인덱스 개수
        _uint iNumTriangles = iNumIndices / 3;  // 삼각형 개수

        // 정점 좌표를 저장할 피직스용 벡터
        vector<PxVec3> Vertices;
        Vertices.reserve(iNumVertices);

        // 게임 객체의 월드 변환 행렬
        _matrix WorldMatrix = pObject->Get_Transform()->Get_WorldMatrix();

        // 정점 좌표 변환 및 저장
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

        // 인덱스를 저장할 벡터
        vector<PxU32> Indices;
        Indices.reserve(iNumIndices);

        // 인덱스 데이터 저장
        for (_uint i = 0; i < iNumIndices; ++i)
        {
            _uint Index = pIB[i];
            Indices.push_back(Index);
        }

        // 삼각형 메쉬 설명 생성
        PxTriangleMeshDesc tDesc;
        tDesc.points.count = iNumVertices;
        tDesc.points.stride = sizeof(PxVec3);
        tDesc.points.data = Vertices.data();
        tDesc.triangles.count = iNumTriangles;
        tDesc.triangles.stride = sizeof(PxU32) * 3;
        tDesc.triangles.data = Indices.data();

        // 삼각형 메쉬 생성
        PxTriangleMesh* pTriangleMesh = PxCreateTriangleMesh(PxCookingParams(PxTolerancesScale(0.0f, 0.0f)), tDesc);

        // 삼각형 메쉬 형상 생성
        PxTriangleMeshGeometry* pGeometry = new PxTriangleMeshGeometry(pTriangleMesh);

        // 충돌 메쉬 목록에 추가
        m_ColMesheGeometries.push_back(pGeometry);

        // RigidStatic 객체 생성
        // 움직이지 않는 정적 모델들은 이거로 만들어야함
        PxRigidStatic* pActor = m_PhysX->createRigidStatic(Get_PhysXTransform(pObject));

        // 물리 재질 생성
        PxMaterial* Material = m_PhysX->createMaterial(0.5f, 0.5f, 0.6f);
        // 충돌 형상 생성 및 추가
        PxShape* pShape = m_PhysX->createShape(*pGeometry, *Material);
        if(pActor!=nullptr)
        {
            pActor->attachShape(*pShape);
            // 물리 씬에 Actor 추가
            m_PxScene->addActor(*pActor);

            // 정적 Actor 목록에 추가
            m_StaticActors.push_back(pActor);
        }

    }
    return S_OK;
}

HRESULT CPhysX_Manager::SetUp_Player(CGameObject* pPlayer)
{
    PxRigidDynamic* pRigid = m_PhysX->createRigidDynamic(Get_PhysXTransform(pPlayer));

    // 머테리얼 환경 정의
    // 정적 마찰, 동적 마찰, 반발 계수
    PxMaterial* Material = m_PhysX->createMaterial(0.5f, 0.5f, 0.6f);

    // 반지름이 0.5이고 높이가 0.3인 캡슐 헝태
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
    //이게 맞나?
    //암튼 피직스용 Transfrom으로 변환
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

    // PhysX Extensions 닫기
    PxCloseExtensions();

    PhysX_RELEASE(m_PxDispatcher);
    PhysX_RELEASE(m_PhysX);
    PhysX_RELEASE(m_pTransport);
    PhysX_RELEASE(m_Pvd);

    // 제일 마지막에 Release
    PhysX_RELEASE(m_PxFoundation);


}


//else
//{
//    // 벽에 부딪힌 경우 그 위치에 고정
//    _vector vPlayerPos = PlayerTransformCom->Get_State(CTransform::STATE_POSITION);

//    // 충돌 방향으로 플레이어 위치를 미세하게 조정하여 벽에 고정
//    vPlayerPos += vResultDir * fLength;

//    // 플레이어의 방향을 벽에 맞게 회전
//    _vector vForward = XMVector3Cross(vResultDir, vUp);
//    vForward = XMVector3Normalize(vForward);
//    _vector vNewUp = XMVector3Cross(vForward, vResultDir);
//    vNewUp = XMVector3Normalize(vNewUp);

//    // 새로운 회전 행렬을 계산
//    _matrix RotationMatrix = XMMatrixIdentity();
//    RotationMatrix.r[0] = XMVectorSetW(vForward, 0.f);
//    RotationMatrix.r[1] = XMVectorSetW(vNewUp, 0.f);
//    RotationMatrix.r[2] = XMVectorSetW(vResultDir, 0.f);
//    RotationMatrix.r[3] = XMVectorSet(0.f, 0.f, 0.f, 1.f);

//    PlayerTransformCom->Set_WorldMatrix(RotationMatrix);

//    // 위치를 고정
//    PlayerTransformCom->Set_State(CTransform::STATE_POSITION, vPlayerPos);
//    }