#include "stdafx.h"
#include "RollingSpike.h"
#include "GameInstance.h"
#include "Link.h"

CRollingSpike::CRollingSpike(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CGameObject(pDevice, pContext)
{
}

CRollingSpike::CRollingSpike(const CRollingSpike& Prototype)
    :CGameObject(Prototype)
{
}
HRESULT CRollingSpike::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CRollingSpike::Initialize(void* pArg)
{
    GAMEOBJECT_DESC* pDesc = static_cast<GAMEOBJECT_DESC*>(pArg);
    pDesc->fRotationPerSec = 10.f;
    m_fRotateSpeed = 10.f;
    m_fMoveSpeed = 5.f;

    /* 직교퉁여을 위한 데이터들을 모두 셋. */
    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    //Read한 정보 세팅
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&pDesc->vPosition));
    m_pTransformCom->Set_Scaled(pDesc->vScale.x, pDesc->vScale.y, pDesc->vScale.z);
    m_pTransformCom->RotationThreeAxis(pDesc->vRotation);
    
    m_vRot = pDesc->vRotation;
    m_iRoomNum = pDesc->iRoomNum;
    m_eObjType = CGameObject::ANIM_MONSTER;
    m_isActive = false;

    m_iCurrentAnimIndex = m_pModelCom->Get_AnimationIndex("dead");
    m_pModelCom->SetUp_Animation(m_iCurrentAnimIndex, false);
    m_pModelCom->Add_Texture_to_Material(TEXT("../Bin/Resources/Zelda/Effect/dissolve_02.dds"), TEXTURE_TYPE::DISSOLVE, 0);

    return S_OK;
}

void CRollingSpike::Priority_Update(_float fTimeDelta)
{
}

void CRollingSpike::Update(_float fTimeDelta)
{
    if (m_isActive)
    {
       m_pModelCom->Play_Animation(fTimeDelta);
       m_pSoundCom->Update(fTimeDelta);
       m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_Ptr());

       if (m_bMoveLeft)
           Move_Left(fTimeDelta);

       if (m_bMoveRight)
           Move_Right(fTimeDelta);

       if (m_bActiveDissolve)
       {
           if (m_fAlpha == 1.f)
               m_isActive = false;

           m_fAlpha = min(1.f, m_fAlpha + fTimeDelta * 0.5f);
       }
    }
}

void CRollingSpike::Late_Update(_float fTimeDelta)
{
    if (m_isActive)
    {
        __super::Late_Update(fTimeDelta);
        m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
      //  m_pGameInstance->Add_RenderObject(CRenderer::RG_SHADOWOBJ, this);
        m_pGameInstance->Add_ColliderList(m_pColliderCom);

#ifdef _DEBUG
        m_pGameInstance->Add_DebugObject(m_pColliderCom);
#endif
    }
}

HRESULT CRollingSpike::Render()
{
    if (m_isActive)
    {
        if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
            return E_FAIL;
        if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_RawValue("g_bActiveDissolve", &m_bActiveDissolve, sizeof(_bool))))
            return E_FAIL;
        
        if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float))))
            return E_FAIL;

        _uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

        for (size_t i = 0; i < iNumMeshes; i++)
        {
            m_pModelCom->Bind_MeshBoneMatrices(m_pShaderCom, "g_BoneMatrices", (_uint)i);

            if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", TEXTURE_TYPE::DIFFUSE, (_uint)i)))
                return E_FAIL;
            if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DissolveTexture", TEXTURE_TYPE::DISSOLVE, (_uint)i)))
                return E_FAIL;
            if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", TEXTURE_TYPE::NORMALS, (_uint)i)))
                return E_FAIL;
            _bool bflase = { true };
            if (FAILED(m_pShaderCom->Bind_RawValue("g_bNormalize", &bflase, sizeof(_bool))))
                return E_FAIL;
            if (FAILED(m_pShaderCom->Begin(8)))
                return E_FAIL;

            if (FAILED(m_pModelCom->Render((_uint)i)))
                return E_FAIL;
        }

        _bool bFalse = { false };
        if (FAILED(m_pShaderCom->Bind_RawValue("g_bActiveDissolve", &bFalse, sizeof(_bool))))
            return E_FAIL;

        bool bTrue = { true };
        if (FAILED(m_pShaderCom->Bind_RawValue("g_bNormalize", &bTrue, sizeof(_bool))))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CRollingSpike::Render_LightDepth()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldMatrix_Ptr())))
        return E_FAIL;

    _float4x4		ViewMatrix;
    XMStoreFloat4x4(&ViewMatrix, XMMatrixLookAtLH(m_pGameInstance->Get_ShadowLightPos_Vector(), m_pGameInstance->Get_LightLook_Vector(), XMVectorSet(0.f, 1.f, 0.f, 0.f)));

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

    _uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", TEXTURE_TYPE::DIFFUSE, (_uint)i)))
            return E_FAIL;

        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", TEXTURE_TYPE::NORMALS, (_uint)i)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(6)))
            return E_FAIL;

        if (FAILED(m_pModelCom->Render((_uint)i)))
            return E_FAIL;
    }

    return S_OK;
}

void CRollingSpike::OnCollisionEnter(CGameObject* pOther)
{
}

void CRollingSpike::OnCollisionStay(CGameObject* pOther)
{
}

void CRollingSpike::OnCollisionExit(CGameObject* pOther)
{
}

void CRollingSpike::Move_Right(_float fTimeDelta)
{

    if (m_pSoundCom->Get_IsPlaying() == false)
        m_pSoundCom->Play_Sound(TEXT("4_Obj_RollingSpike.wav"), 1.f);

    _vector vTurn = { 0.f, 0.f, -1.f };
    m_fMoveTimer += fTimeDelta;

    //오른쪽 굴러가기
    if (m_fMoveTimer < 1.8f)
    {
        m_pTransformCom->Turn(vTurn, fTimeDelta);
        m_pTransformCom->Go_World_Right(fTimeDelta, 5.f);
    }
    else if (m_fMoveTimer < 2.8f)
    {   
        //벽 부딪히는 척 반대로 굴러가기 + 느려짐
        m_fRotateSpeed = max(0.3f, m_fRotateSpeed -= 3.f * fTimeDelta);
        m_pTransformCom->Set_RotationSpeed(m_fRotateSpeed);
        m_pTransformCom->Turn(-vTurn, fTimeDelta);
        m_pTransformCom->Go_World_Left(fTimeDelta, max(3.f, m_fMoveSpeed -= 6.f * fTimeDelta));
    }
    else
    {
        //초기화
        m_fRotateSpeed = 10.f;
        m_pTransformCom->Set_RotationSpeed(m_fRotateSpeed);
        m_fMoveTimer = 0.f;
        m_bMoveRight = false;
        m_fMoveSpeed = 5.f;
        m_pSoundCom->Stop();
    }
}

void CRollingSpike::Move_Left(_float fTimeDelta)
{
    if (m_pSoundCom->Get_IsPlaying() == false)
        m_pSoundCom->Play_Sound(TEXT("4_Obj_RollingSpike.wav"), 1.f);

    _vector vTurn = { 0.f, 0.f, 1.f };
    m_fMoveTimer += fTimeDelta;

    //왼쪽 굴러가기
    if (m_fMoveTimer < 1.8f)
    {
        m_pTransformCom->Turn(vTurn, fTimeDelta);
        m_pTransformCom->Go_World_Left(fTimeDelta, 5.f);
    }
    else if(m_fMoveTimer < 2.8f)
    {
        //벽 부딪힌 척 반대로 굴러가기 + 느려지기
        m_fRotateSpeed = max(0.3f, m_fRotateSpeed -= 3.f * fTimeDelta);
        m_pTransformCom->Set_RotationSpeed(m_fRotateSpeed);
        m_pTransformCom->Turn(-vTurn, fTimeDelta);
       m_pTransformCom->Go_World_Right(fTimeDelta, max(3.f, m_fMoveSpeed -= 6.f * fTimeDelta));
    }
    else
    {
        //초기화
        m_fRotateSpeed = 10.f;
        m_pTransformCom->Set_RotationSpeed(m_fRotateSpeed);
        m_fMoveTimer = 0.f;
        m_bMoveLeft = false;
        m_fMoveSpeed = 5.f;
        m_pSoundCom->Stop();
    }
}

HRESULT CRollingSpike::Ready_Components()
{
    /* FOR.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* FOR.Com_Model */
    if (FAILED(__super::Add_Component(LEVEL_DUNGEON, TEXT("Prototype_Component_Model_RollingSpike"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    /* For.Com_Collider */
    CBounding_AABB::BOUNDING_AABB_DESC			ColliderDesc{};
    ColliderDesc.vExtents = _float3(0.3f, 0.4f, 5.f);
    ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vExtents.y, 0.f);

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
        TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
        return E_FAIL;
    m_pColliderCom->Set_Owner(this);


    /* FOR.Com_Sound */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sound"),
        TEXT("Com_Sound"), reinterpret_cast<CComponent**>(&m_pSoundCom))))
        return E_FAIL;
    m_pSoundCom->Set_Owner(this);

    return S_OK;
}

CRollingSpike* CRollingSpike::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CRollingSpike* pInstance = new CRollingSpike(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CRollingSpike"));
        Safe_Release(pInstance);
    }

    return pInstance;
}
CGameObject* CRollingSpike::Clone(void* pArg)
{
    CRollingSpike* pInstance = new CRollingSpike(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CRollingSpike"));
        Safe_Release(pInstance);
    }

    return pInstance;
}
void CRollingSpike::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pSoundCom);
    Safe_Release(m_pModelCom);
    Safe_Release(m_pColliderCom);
}
