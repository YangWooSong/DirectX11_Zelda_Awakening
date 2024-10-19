#include "stdafx.h"
#include "GrandmaUlrira.h"
#include "GameInstance.h"

#include "State_GrandmaUlrira_Idle.h"
#include "State_GrandmaUlrira_Talk.h"

#include "DialogueUI.h"

CGrandmaUlrira::CGrandmaUlrira(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CNPC{ pDevice, pContext }
{
}


CGrandmaUlrira::CGrandmaUlrira(const CGrandmaUlrira& Prototype)
    : CNPC{ Prototype }
{
}

HRESULT CGrandmaUlrira::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CGrandmaUlrira::Initialize(void* pArg)
{
    /* ���������� ���� �����͵��� ��� ���ϳ�. */
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_State()))
        return E_FAIL;

    if (FAILED(Ready_PartObjects()))
        return E_FAIL;

    if (FAILED(Ready_UIObjects()))
        return E_FAIL;

    m_pFsmCom->Set_State(IDLE);
    m_pTransformCom->RotationThreeAxis(_float3(0.f,90.f, 0.f));

    return S_OK;
}

void CGrandmaUlrira::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
}

void CGrandmaUlrira::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);

    m_pFsmCom->Update(fTimeDelta);
    m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_Ptr());

}

void CGrandmaUlrira::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);

    m_pGameInstance->Add_ColliderList(m_pColliderCom);

#ifdef _DEBUG
    m_pGameInstance->Add_DebugObject(m_pColliderCom);
#endif
}

HRESULT CGrandmaUlrira::Render()
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

    __super::Render();

    return S_OK;
}

void CGrandmaUlrira::OnCollisionEnter(CGameObject* pOther)
{
}

void CGrandmaUlrira::OnCollisionStay(CGameObject* pOther)
{
    if (m_pColliderCom->Get_IsColl())
    {
        if (KEY_TAP(KEY::E))
        {
            if (m_pFsmCom->Get_PrevState() != TALK)
            {
                Change_State(TALK);
                m_bTalk = true;
            }
        }
    }
}

void CGrandmaUlrira::OnCollisionExit(CGameObject* pOther)
{
    if (pOther->Get_LayerTag() == TEXT("Layer_Player"))
    {
        if (m_pFsmCom->Get_PrevState() != IDLE)
            m_pFsmCom->Change_State(IDLE);
    }
}

HRESULT CGrandmaUlrira::Ready_Components()
{
    __super::Ready_Components();

    /* FOR.Com_Model */
    if (FAILED(__super::Add_Component(LEVEL_FIELD, TEXT("Prototype_Component_Model_NPC_GrandmaUlrira"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    /* For.Com_Collider */
    CBounding_AABB::BOUNDING_AABB_DESC			ColliderDesc{};
    ColliderDesc.vExtents = _float3(0.8f, 1.f, 0.8f);
    ColliderDesc.vCenter = _float3(1.f, ColliderDesc.vExtents.y * 0.5f, 0.f);

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
        TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
        return E_FAIL;
    m_pColliderCom->Set_Owner(this);

    return S_OK;
}

HRESULT CGrandmaUlrira::Ready_PartObjects()
{
    return S_OK;
}

HRESULT CGrandmaUlrira::Ready_UIObjects()
{
    CGameObject* pGameObj = m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_DialogueUI"));
    if (pGameObj != nullptr)
    {
        CDialogueUI::DIALOGUE_DESC UIDesc{};
        UIDesc.fSizeX = g_iWinSizeX / 2.f;
        UIDesc.fSizeY = g_iWinSizeY / 5.f;
        UIDesc.fX = g_iWinSizeX * 0.5f;
        UIDesc.fY = g_iWinSizeY * 0.75f;
        UIDesc.iOwnerType = CDialogueUI::GRANDMA;

        CUIObject* m_pDialogueUI = dynamic_cast<CUIObject*>(pGameObj->Clone(&UIDesc));
        m_pDialogueUI->Set_ParentObj(this);
        m_NPCUI.push_back(m_pDialogueUI);
    }

    return S_OK;
}

HRESULT CGrandmaUlrira::Ready_State()
{
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Fsm"),
        TEXT("Com_Fsm"), reinterpret_cast<CComponent**>(&m_pFsmCom), this)))
        return E_FAIL;

    m_pFsmCom->Add_State(CState_GrandmaUlrira_Idle::Create(m_pFsmCom, this, IDLE));
    m_pFsmCom->Add_State(CState_GrandmaUlrira_Talk::Create(m_pFsmCom, this, TALK));

    return S_OK;
}

CGrandmaUlrira* CGrandmaUlrira::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CGrandmaUlrira* pInstance = new CGrandmaUlrira(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CGrandmaUlrira"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CGrandmaUlrira::Clone(void* pArg)
{
    CGrandmaUlrira* pInstance = new CGrandmaUlrira(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CGrandmaUlrira"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CGrandmaUlrira::Free()
{
    __super::Free();

    if (nullptr != m_pFsmCom)
        m_pFsmCom->Release_States();

    Safe_Release(m_pFsmCom);
}
