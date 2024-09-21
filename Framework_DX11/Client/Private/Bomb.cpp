#include "stdafx.h"
#include "Bomb.h"
#include "GameInstance.h"

CBomb::CBomb(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CPartObject(pDevice, pContext)
{
}


CBomb::CBomb(const CBomb& Prototype)
	:CPartObject(Prototype)
{
}

HRESULT CBomb::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBomb::Initialize(void* pArg)
{
    BOMB_DESC* pDesc = static_cast<BOMB_DESC*>(pArg);
    m_pSocketMatrix = pDesc->pSocketBoneMatrix;

    /* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_eObjType = CGameObject::ANIM_OBJ;

    Set_LayerTag(TEXT("Layer_Bomb"));

    m_isActive = false;

    m_iCurrentAnimIndex = m_pModelCom->Get_AnimationIndex("pop");
    m_pModelCom->SetUp_Animation(m_iCurrentAnimIndex, true);
    return S_OK;
}

void CBomb::Priority_Update(_float fTimeDelta)
{
}

void CBomb::Update(_float fTimeDelta)
{

    //작동한다
    if (m_isActive)
    {
        if (m_bPut == false)
        {
            m_fPutTimer += fTimeDelta;

            //진자 눈에 보이기 시작
            if(m_fPutTimer > 0.05f)
            {
                m_bPut = true;
                m_pColliderCom->Set_IsActive(true);

                _matrix		SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

                for (size_t i = 0; i < 3; i++)
                {
                    SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);
                }
                XMStoreFloat4x4(&m_WorldMatrix, SocketMatrix * XMLoadFloat4x4(m_pParentMatrix));

                m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(&m_WorldMatrix));

                _vector fOffset = { 0.f, 0.5f, 0.f };

                m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4x4(&m_WorldMatrix).r[3]);
                m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_State(CTransform::STATE_POSITION) + fOffset);
            }
        }
        else
            fExplodeTimer += fTimeDelta;

        m_pColliderCom->Update(&m_WorldMatrix);
        m_pSoundCom->Update(fTimeDelta);
        m_pModelCom->Play_Animation(fTimeDelta);

        if (fExplodeTimer > 2.f)
        {
            Red_Blink();
        }
        if (fExplodeTimer > 3.3f)
        {
            m_pColliderCom->Set_IsActive(false);
            m_isActive = false;
            m_pSoundCom->Play_Sound(TEXT("4_Obj_Bomb.wav"), 1.f);
        }

    }
    else
    {
        if(m_bPut)
        {
            m_bPut = false;
            m_fPutTimer = 0.f;
            fExplodeTimer = 0.f;
            m_fRed = 1.f;
        }
    }
}

void CBomb::Late_Update(_float fTimeDelta)
{
    if (m_bPut)
    {
        __super::Late_Update(fTimeDelta);

        m_pGameInstance->Add_ColliderList(m_pColliderCom);
        m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);

#ifdef _DEBUG
        m_pGameInstance->Add_DebugObject(m_pColliderCom);
#endif
    }
}

HRESULT CBomb::Render()
{
    if (m_bPut)
    {
        if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
            return E_FAIL;
        if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_RawValue("g_fRed", &m_fRed, sizeof(_float))))
            return E_FAIL;

        _uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

        for (size_t i = 0; i < iNumMeshes; i++)
        {
            m_pModelCom->Bind_MeshBoneMatrices(m_pShaderCom, "g_BoneMatrices", (_uint)i);

            if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", TEXTURE_TYPE::DIFFUSE, (_uint)i)))
                return E_FAIL;


            if (FAILED(m_pShaderCom->Begin(3)))
                return E_FAIL;

            if (FAILED(m_pModelCom->Render((_uint)i)))
                return E_FAIL;
        }
    }

    return S_OK;
}

HRESULT CBomb::Ready_Components()
{
    /* FOR.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* FOR.Com_Model */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Bomb"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    /* For.Com_Collider */
    CBounding_AABB::BOUNDING_AABB_DESC			ColliderDesc{};
    ColliderDesc.vExtents = _float3(0.5f, 0.5f, 0.5f);
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

void CBomb::Red_Blink()
{
    m_fRed += 0.5f;
    if (m_fRed > 10.f)
        m_fRed = 1.f;
}

CBomb* CBomb::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CBomb* pInstance = new CBomb(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CBomb"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CBomb::Clone(void* pArg)
{
    CBomb* pInstance = new CBomb(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CBomb"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBomb::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModelCom);
    Safe_Release(m_pColliderCom);
    Safe_Release(m_pSoundCom);
}
