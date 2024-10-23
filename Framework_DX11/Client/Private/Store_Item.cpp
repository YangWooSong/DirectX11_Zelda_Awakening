#include "stdafx.h"
#include "Store_Item.h"
#include "GameInstance.h"
#include "Link.h"
#include "ItemExplainUI.h"
CStore_Item::CStore_Item(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CGameObject(pDevice, pContext)
{
}

CStore_Item::CStore_Item(const CStore_Item& Prototype)
    :CGameObject(Prototype)
{
}

HRESULT CStore_Item::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CStore_Item::Initialize(void* pArg)
{
    STORE_ITEM_DESC* pDesc = static_cast<STORE_ITEM_DESC*>(pArg);
    m_iItemType = pDesc->iItemType;

    /* 직교퉁여을 위한 데이터들을 모두 셋. */
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    //Read한 정보 세팅
    if (m_iItemType == SOLD_OUT)
        pDesc->vPosition.y = 0.8f;
    else
        pDesc->vPosition.y = 1.1f;

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&pDesc->vPosition));
    m_pTransformCom->Set_Scaled(pDesc->vScale.x, pDesc->vScale.y, pDesc->vScale.z);
    m_pTransformCom->RotationThreeAxis(_float3(0.f, 0.f, 0.f));
    m_vRot = pDesc->vRotation;
    m_vOriginPos = pDesc->vPosition;

    m_isActive = true;

    m_pPlayer = static_cast<CLink*>(m_pGameInstance->Find_Player(LEVEL_STORE));
    m_pSocketMatrix = m_pPlayer->Get_Model()->Get_BoneCombindTransformationMatrix_Ptr("itemA_L");

  

    CGameObject* pGameObj = m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_ItemExplainUI"));
    if (pGameObj != nullptr)
    {
        CItemExplainUI::ITEM_EXPLAIN_DESC UIDesc;

        if (m_iItemType == BOMB)
            UIDesc.iItemType = CItemExplainUI::BOMB;
        else if (m_iItemType == HEART)
            UIDesc.iItemType = CItemExplainUI::HEART;
        _float3 vPos;
        XMStoreFloat3(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

        UIDesc.vPosition = vPos;
        UIDesc.vPosition.y += 1.f;
        m_pEffect = dynamic_cast<CUIObject*>(pGameObj->Clone(&UIDesc));
        m_pEffect->Set_ParentObj(this);
    }

    return S_OK;
}

void CStore_Item::Priority_Update(_float fTimeDelta)
{
}

void CStore_Item::Update(_float fTimeDelta)
{
    if (m_bPicked == false)
    {
        m_fTimer = 0.f;

        if(m_pColliderCom != nullptr)
            m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_Ptr());

        if (m_pColliderCom != nullptr && m_pColliderCom->IsActive() == false)
            m_pColliderCom->Set_IsActive(true);
    }
    else
    {
        if (m_pColliderCom != nullptr && m_pColliderCom->IsActive())
            m_pColliderCom->Set_IsActive(false);

        if (m_fTimer > 0.5f)
        {
            _matrix		SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);
            m_pTransformCom->Set_WorldMatrix(SocketMatrix * m_pPlayer->Get_Transform()->Get_WorldMatrix());
        }
        m_fTimer += fTimeDelta;
    }

    if (m_bActiveEffect)
        m_pEffect->SetActive(true);
    else
        m_pEffect->SetActive(false);
    m_pEffect->Update(fTimeDelta);
}

void CStore_Item::Late_Update(_float fTimeDelta)
{
    if (m_isActive)
    {
        __super::Late_Update(fTimeDelta);
        m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
        m_pGameInstance->Add_RenderObject(CRenderer::RG_SHADOWOBJ, this);

        if(m_pColliderCom != nullptr && m_bPicked == false)
            m_pGameInstance->Add_ColliderList(m_pColliderCom);

        m_pEffect->Late_Update(fTimeDelta);

#ifdef _DEBUG
        if(m_pColliderCom != nullptr)
            m_pGameInstance->Add_DebugObject(m_pColliderCom);
#endif
    }
}

HRESULT CStore_Item::Render()
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

            if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", TEXTURE_TYPE::NORMALS, (_uint)i)))
                return E_FAIL;

            if (FAILED(m_pShaderCom->Begin(5)))
                return E_FAIL;

            if (FAILED(m_pModelCom->Render((_uint)i)))
                return E_FAIL;
        }
    }

    return S_OK;
}

HRESULT CStore_Item::Render_LightDepth()
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

        if (FAILED(m_pShaderCom->Begin(13)))
            return E_FAIL;

        if (FAILED(m_pModelCom->Render((_uint)i)))
            return E_FAIL;
    }

    return S_OK;
}

void CStore_Item::OnCollisionEnter(CGameObject* pOther)
{

}

void CStore_Item::OnCollisionStay(CGameObject* pOther)
{
    if (m_pColliderCom != nullptr &&  m_pColliderCom->Get_IsColl())
    {
        if (pOther->Get_LayerTag() == TEXT("Layer_Player") && m_bPicked == false)
        {
            if(KEY_TAP(KEY::E))
                m_bPicked = true;
            if(m_bPicked == false)
                m_bActiveEffect = true;
            else
                m_bActiveEffect = false;
        }
    }
}

void CStore_Item::OnCollisionExit(CGameObject* pOther)
{
    m_bActiveEffect = false;
}

void CStore_Item::Go_Back_OriginPos()
{
    m_bPicked = false;
    m_isActive = true;
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&m_vOriginPos));
    m_pTransformCom->RotationThreeAxis(_float3(0.f, 0.f,0.f));
}

HRESULT CStore_Item::Ready_Components()
{
    /* FOR.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxModel"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* FOR.Com_Model */
    switch (m_iItemType)
    {
    case BOMB:
        if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Bomb"),
            TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
            return E_FAIL;
        break;
    case SOLD_OUT:
        if (FAILED(__super::Add_Component(LEVEL_STORE, TEXT("Prototype_Component_Model_SoldOutPlate"),
            TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
            return E_FAIL;
        break;
    case HEART:
        if (FAILED(__super::Add_Component(LEVEL_STORE, TEXT("Prototype_Component_Model_HeartContainer"),
            TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
            return E_FAIL;
        break;
    default:
        break;
    }
 

    if(m_iItemType != SOLD_OUT)
    {
        /* For.Com_Collider */
        CBounding_AABB::BOUNDING_AABB_DESC			ColliderDesc{};
        ColliderDesc.vExtents = _float3(0.5f, 1.f, 1.f);
        ColliderDesc.vCenter = _float3(0.f, 0.f, 0.f);

        if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
            TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
            return E_FAIL;
        m_pColliderCom->Set_Owner(this);
    }

    return S_OK;
}

CStore_Item* CStore_Item::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CStore_Item* pInstance = new CStore_Item(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CStore_Item"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CStore_Item::Clone(void* pArg)
{
    CStore_Item* pInstance = new CStore_Item(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CStore_Item"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CStore_Item::Free()
{
    __super::Free();

    Safe_Release(m_pEffect);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModelCom);
    Safe_Release(m_pColliderCom);
}
