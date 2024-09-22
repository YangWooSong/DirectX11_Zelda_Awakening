#include "stdafx.h"
#include "BladeTrap.h"
#include "GameInstance.h"
#include "PartObject.h"
#include "Detector.h"

CBladeTrap::CBladeTrap(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CContainerObject(pDevice, pContext)
{
}

CBladeTrap::CBladeTrap(const CBladeTrap& Prototype)
    :CContainerObject(Prototype)
{
}
HRESULT CBladeTrap::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CBladeTrap::Initialize(void* pArg)
{
    GAMEOBJECT_DESC* pDesc = static_cast<GAMEOBJECT_DESC*>(pArg);

    /* 직교퉁여을 위한 데이터들을 모두 셋. */
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL; 
    
    if (FAILED(Ready_PartObjects()))
        return E_FAIL;

    //Read한 정보 세팅
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&pDesc->vPosition));
    m_pTransformCom->Set_Scaled(pDesc->vScale.x, pDesc->vScale.y, pDesc->vScale.z);
    m_pTransformCom->RotationThreeAxis(pDesc->vRotation);
    m_vRot = pDesc->vRotation;
    m_iRoomNum = pDesc->iRoomNum;

    m_isActive = false;
    m_vOriPos = XMLoadFloat3(&pDesc->vPosition);

    m_iCurrentAnimIndex = m_pModelCom->Get_AnimationIndex("attack");
    m_pModelCom->SetUp_Animation(m_iCurrentAnimIndex, true);
    m_pModelCom->Set_AnimationSpeed(m_iCurrentAnimIndex, 40);

    Set_LayerTag(TEXT("Layer_Monster"));
    return S_OK;
}

void CBladeTrap::Priority_Update(_float fTimeDelta)
{
    if (m_isActive)
    {
        for (auto& pPartObject : m_Parts)
            pPartObject->Priority_Update(fTimeDelta);
    }
}

void CBladeTrap::Update(_float fTimeDelta)
{
    if (m_isActive)
    {
        for (auto& pPartObject : m_Parts)
            pPartObject->Update(fTimeDelta);

        if(m_iDir != DIR_END)
        {
            if(m_fStopCount != 2)
                 m_pModelCom->Play_Animation(fTimeDelta);

            for (auto& pPartObject : m_Parts)
                static_cast<CDetector*>(pPartObject)->Set_Active_Collider(false);
            m_bHoriDetect = false;
            m_bVerDetect = false;
        }
        else
        {
            for (auto& pPartObject : m_Parts)
                static_cast<CDetector*>(pPartObject)->Set_Active_Collider(true);
        }

        if (m_fStopCount == 3)
        {
            m_fSpeed *= 0.5f;
            m_pModelCom->Set_AnimationSpeed(m_iCurrentAnimIndex, 30.f);
            Move_Reverse(fTimeDelta);
            m_fStopCount++;
        }
        else if (m_fStopCount == 5)
        {
            m_fSpeed *= 2.f;
            m_pModelCom->Set_AnimationSpeed(m_iCurrentAnimIndex, 40.f);
            m_iDir = DIR_END;
            m_fStopCount = 0;
        }
       
        if(m_fStopCount != 2)
            Move(fTimeDelta);
        else
        {
            m_fWaitTimer += fTimeDelta;
            if (m_fWaitTimer > 0.5f)
            {
                m_fStopCount++;
                m_fWaitTimer = 0.f;
            }
        }

        m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_Ptr());
    }
}

void CBladeTrap::Late_Update(_float fTimeDelta)
{
    if (m_isActive)
    {
        __super::Late_Update(fTimeDelta);

        for (auto& pPartObject : m_Parts)
            pPartObject->Late_Update(fTimeDelta);

        m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
        m_pGameInstance->Add_ColliderList(m_pColliderCom);

#ifdef _DEBUG
        m_pGameInstance->Add_DebugObject(m_pColliderCom);
#endif
    }
}

HRESULT CBladeTrap::Render()
{
    if (m_isActive)
    {
        if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
            return E_FAIL;
        if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
            return E_FAIL;

        _uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

        for (size_t i = 0; i < iNumMeshes; i++)
        {
            m_pModelCom->Bind_MeshBoneMatrices(m_pShaderCom, "g_BoneMatrices", (_uint)i);

            if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", TEXTURE_TYPE::DIFFUSE, (_uint)i)))
                return E_FAIL;


            if (FAILED(m_pShaderCom->Begin(0)))
                return E_FAIL;

            if (FAILED(m_pModelCom->Render((_uint)i)))
                return E_FAIL;
        }
    }

    return S_OK;
}

void CBladeTrap::OnCollisionEnter(CGameObject* pOther)
{
    if (m_pColliderCom->Get_IsColl())
    {
        if (pOther->Get_LayerTag() == TEXT("Layer_Monster") || pOther->Get_LayerTag() == TEXT("Layer_Player"))
        {
            //뭐든 닿으면 멈추게
            m_bMove = false;
            m_fStopCount = 3;
        }
    }
}

void CBladeTrap::OnCollisionStay(CGameObject* pOther)
{
}

void CBladeTrap::OnCollisionExit(CGameObject* pOther)
{
}

HRESULT CBladeTrap::Ready_Components()
{
    /* FOR.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* FOR.Com_Model */
    if (FAILED(__super::Add_Component(LEVEL_DUNGEON, TEXT("Prototype_Component_Model_BladeTrap"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    /* For.Com_Collider */
    CBounding_AABB::BOUNDING_AABB_DESC			ColliderDesc{};
    ColliderDesc.vExtents = _float3(0.65f, 0.4f, 0.65f);
    ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vExtents.y, 0.f);

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
        TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
        return E_FAIL;
    m_pColliderCom->Set_Owner(this);

    /* For.Com_Navigation */
    CNavigation::NAVIGATION_DESC			NaviDesc{};

    NaviDesc.iCurrentIndex = m_iCellNum;
    NaviDesc.iOwnerType = CNavigation::NONPLAYER;

    if (FAILED(__super::Add_Component(LEVEL_DUNGEON, TEXT("Prototype_Component_Navigation"),
        TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &NaviDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CBladeTrap::Ready_PartObjects()
{
    /* 실제 추가하고 싶은 파트오브젝트의 갯수만큼 밸류를 셋팅해놓자. */
    m_Parts.resize(PART_END);

    CDetector::DETECTOR_DESC DetectDesc{};
    DetectDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
    DetectDesc.pDetect = &m_bHoriDetect;
    DetectDesc.vOffset = _float3(0.f, 0.f, 0.f);
    DetectDesc.vSize = _float3(5.f, 0.5f, 0.5f);
    if (FAILED(__super::Add_PartObject(PART_DETECTOR_HORIZON, TEXT("Prototype_GameObject_Detector"), &DetectDesc)))
        return E_FAIL;

    DetectDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
    DetectDesc.pDetect = &m_bVerDetect;
    DetectDesc.vOffset = _float3(0.f, 0.f, 0.f);
    DetectDesc.vSize = _float3(0.5f, 0.5f, 5.f);
    if (FAILED(__super::Add_PartObject(PART_DETECTOR_VERTICAL, TEXT("Prototype_GameObject_Detector"), &DetectDesc)))
        return E_FAIL;


    return S_OK;
}

void CBladeTrap::Move(_float fTimeDelta)
{
    //player 닿으면 방향 확인
    if (m_bVerDetect || m_bHoriDetect)  
        m_bMove = true;

    if (m_iDir == DIR_END)
        Set_Dir();

    switch (m_iDir)
    {
    case RIGHT:
        m_pTransformCom->Go_World_Right(fTimeDelta, m_fSpeed, m_pNavigationCom, &m_bMove);
        break;
    case LEFT:
        m_pTransformCom->Go_World_Left(fTimeDelta, m_fSpeed, m_pNavigationCom, &m_bMove);
        break;
    case UP:
        m_pTransformCom->Go_World_Backward(fTimeDelta, m_fSpeed, m_pNavigationCom, &m_bMove);
        break;
    case DOWN:
        m_pTransformCom->Go_World_Straight(fTimeDelta, m_fSpeed, m_pNavigationCom, &m_bMove);
        break;
    }

    if (fabs(XMVectorGetX(XMVector3Length(m_pTransformCom->Get_State(CTransform::STATE_POSITION) - m_vOriPos))) < 0.1f && m_fStopCount > 1)
    {
        m_fStopCount++;
    }

    if (m_bMove == false)
        m_fStopCount++;
}

void CBladeTrap::Move_Reverse(_float fTimeDelta)
{

        switch (m_iDir)
        {
        case RIGHT:
            m_iDir = LEFT;
            break;
        case LEFT:
            m_iDir = RIGHT;
            break;
        case UP:
            m_iDir = DOWN;
            break;
        case DOWN:
            m_iDir = UP;
            break;
        }
}

void CBladeTrap::Set_Dir()
{
    if (m_bHoriDetect)
    {
        _vector vPlayerPos = m_pGameInstance->Find_Player(LEVEL_DUNGEON)->Get_Transform()->Get_State(CTransform::STATE_POSITION);
        _float fPlayerX = XMVectorGetX(vPlayerPos);
        _float fmyX = XMVectorGetX(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

        if (fmyX < fPlayerX)
            m_iDir = RIGHT;
        else
            m_iDir = LEFT;
    }
    else if (m_bVerDetect)
    {
        _vector vPlayerPos = m_pGameInstance->Find_Player(LEVEL_DUNGEON)->Get_Transform()->Get_State(CTransform::STATE_POSITION);
        _float fPlayerY = XMVectorGetY(vPlayerPos);
        _float  fmyY = XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

        if (fmyY < fPlayerY)
            m_iDir = UP;
        else
            m_iDir = DOWN;
    }
}

CBladeTrap* CBladeTrap::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CBladeTrap* pInstance = new CBladeTrap(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CBladeTrap"));
        Safe_Release(pInstance);
    }

    return pInstance;
}
CGameObject* CBladeTrap::Clone(void* pArg)
{
    CBladeTrap* pInstance = new CBladeTrap(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CBladeTrap"));
        Safe_Release(pInstance);
    }

    return pInstance;
}
void CBladeTrap::Free()
{
    __super::Free();

    for (auto& pPartObject : m_Parts)
        Safe_Release(pPartObject);

    Safe_Release(m_pNavigationCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModelCom);
    Safe_Release(m_pColliderCom);
}
