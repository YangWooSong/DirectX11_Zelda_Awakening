#include "stdafx.h"
#include "SmallKey.h"
#include "GameInstance.h"

CSmallKey::CSmallKey(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CGameObject(pDevice, pContext)
{
}

CSmallKey::CSmallKey(const CSmallKey& Prototype)
    :CGameObject(Prototype)
{
}
HRESULT CSmallKey::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CSmallKey::Initialize(void* pArg)
{
    GAMEOBJECT_DESC* pDesc = static_cast<GAMEOBJECT_DESC*>(pArg);

    /* 직교퉁여을 위한 데이터들을 모두 셋. */
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    //Read한 정보 세팅
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&pDesc->vPosition));
    m_pTransformCom->Set_Scaled(pDesc->vScale.x, pDesc->vScale.y, pDesc->vScale.z);
    m_pTransformCom->RotationThreeAxis(pDesc->vRotation);
    m_vRot = pDesc->vRotation;
    m_iRoomNum = pDesc->iRoomNum;

    m_isActive = false;
    return S_OK;
}

void CSmallKey::Priority_Update(_float fTimeDelta)
{
}

void CSmallKey::Update(_float fTimeDelta)
{
    if (m_isActive)
    {
        if (m_bShow == false)
            Show(fTimeDelta);
        else
        {
            if (m_pSoundCom->Get_IsPlaying() == false && m_bGimmickSoundPlay == false)
            {
                m_bGimmickSoundPlay = true;
                m_pGameInstance->Pause_BGM();
                m_pSoundCom->Play_Sound(TEXT("5_GimmickSolve.wav"), 1.f);
            }
            else if (m_bGimmickSoundPlay && m_pSoundCom->Get_IsPause() == false && m_bBGMPlay == false)
            {
                m_bBGMPlay = true;
                m_pGameInstance->Pause_BGM();
            }
        }
        m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_Ptr());
    }
}

void CSmallKey::Late_Update(_float fTimeDelta)
{
    if (m_isActive)
    {
        m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
        m_pGameInstance->Add_ColliderList(m_pColliderCom);

#ifdef _DEBUG
        m_pGameInstance->Add_DebugObject(m_pColliderCom);
#endif
    }
}

HRESULT CSmallKey::Render()
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

void CSmallKey::OnCollisionEnter(CGameObject* pOther)
{
    if (m_pColliderCom->Get_IsColl())
    {
        if (pOther->Get_LayerTag() == TEXT("Layer_Player"))
        {
            m_isDead = true;
            m_isActive = false;
        }
    }
}

void CSmallKey::OnCollisionStay(CGameObject* pOther)
{
}

void CSmallKey::OnCollisionExit(CGameObject* pOther)
{
}

HRESULT CSmallKey::Ready_Components()
{
    /* FOR.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxModel"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* FOR.Com_Model */
    if (FAILED(__super::Add_Component(LEVEL_DUNGEON, TEXT("Prototype_Component_Model_SmallKey"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    /* For.Com_Collider */
    CBounding_AABB::BOUNDING_AABB_DESC			ColliderDesc{};
    ColliderDesc.vExtents = _float3(0.4f, 0.6f, 0.4f);
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

void CSmallKey::Show(_float fTimeDelta)
{
    if (m_bPlayFalling == false)
    {
        m_bPlayFalling = true;
        m_pSoundCom->Play_Sound(TEXT("4_Obj_Key_Falling.wav"), 1.f);
        m_pSoundCom->Set_PlaySpeed(1.2f);
    }

    _float fLerpSpeed = 0.f;

    if (XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION)) <  3.f)
        fLerpSpeed = min(1.0f, 4.f * fTimeDelta);
    else
        fLerpSpeed = min(1.0f, 2.f * fTimeDelta);
    

    _vector vNewPos = XMVectorLerp(m_pTransformCom->Get_State(CTransform::STATE_POSITION), m_TargetPos, fLerpSpeed);
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, vNewPos);

    if (XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION)) < 0.55f)
    {
        m_pSoundCom->Set_PlaySpeed(1.f);
        m_pSoundCom->Play_Sound(TEXT("4_Obj_Key_Dropped.wav"), 1.f);
        m_bShow = true;
    }

  
}

CSmallKey* CSmallKey::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CSmallKey* pInstance = new CSmallKey(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CSmallKey"));
        Safe_Release(pInstance);
    }

    return pInstance;
}
CGameObject* CSmallKey::Clone(void* pArg)
{
    CSmallKey* pInstance = new CSmallKey(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CSmallKey"));
        Safe_Release(pInstance);
    }

    return pInstance;
}
void CSmallKey::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModelCom);
    Safe_Release(m_pColliderCom);
    Safe_Release(m_pSoundCom);
}
