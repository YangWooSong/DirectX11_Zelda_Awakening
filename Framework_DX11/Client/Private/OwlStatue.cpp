#include "stdafx.h"
#include "OwlStatue.h"
#include "GameInstance.h"
#include "Link.h"
#include "MainUI.h"
#include "DialogueUI.h"

COwlStatue::COwlStatue(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CGameObject(pDevice, pContext)
{
}

COwlStatue::COwlStatue(const COwlStatue& Prototype)
    :CGameObject(Prototype)
{
}
HRESULT COwlStatue::Initialize_Prototype()
{
    return S_OK;
}

HRESULT COwlStatue::Initialize(void* pArg)
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

    m_pDialogueUI = static_cast<CDialogueUI*>(static_cast<CMainUI*>(m_pGameInstance->Find_Object(LEVEL_DUNGEON, TEXT("Layer_MainUI"), 0))->Get_ChildUI(CMainUI::DIALOGUE));
    return S_OK;
}

void COwlStatue::Priority_Update(_float fTimeDelta)
{
}

void COwlStatue::Update(_float fTimeDelta)
{
    if (m_isActive)
    {
        m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_Ptr());

        if (m_bInteract)
        {
            if(m_bActiveDialogue == false)
            {
                m_bActiveDialogue = true;
                m_pDialogueUI->Set_OwnerType(CDialogueUI::STATUE);
                if (m_iRoomNum == 9 || m_iRoomNum == 15)
                    m_pDialogueUI->Set_LineNum(CDialogueUI::ROOM9);
                else if (m_iRoomNum == 14)
                    m_pDialogueUI->Set_LineNum(CDialogueUI::ROOM14);
                m_pDialogueUI->SetActive(true);
            }
        }
        else
        {
            if(m_bActiveDialogue)
            {
                m_bActiveDialogue = false;
                m_pDialogueUI->SetActive(false);
            }
        }

    }
}

void COwlStatue::Late_Update(_float fTimeDelta)
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

HRESULT COwlStatue::Render()
{
    if (m_isActive)
    {
        if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
            return E_FAIL;
        if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_RawValue("g_bInteract", &m_bInteract, sizeof(_bool))))
            return E_FAIL;

        _uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

        for (size_t i = 0; i < iNumMeshes; i++)
        {

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

void COwlStatue::OnCollisionEnter(CGameObject* pOther)
{
}

void COwlStatue::OnCollisionStay(CGameObject* pOther)
{
    if (m_pColliderCom->Get_IsColl())
    {
        if (pOther->Get_LayerTag() == TEXT("Layer_Player") && (static_cast<CLink*>(pOther)->Get_isGetBeak()))
        {
            if (KEY_AWAY(E))
                m_bInteract = true;
        }
    }
}

void COwlStatue::OnCollisionExit(CGameObject* pOther)
{
    if (pOther->Get_LayerTag() == TEXT("Layer_Player"))
    {
        m_bInteract = false;
    }
}

HRESULT COwlStatue::Ready_Components()
{
    /* FOR.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxModel"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* FOR.Com_Model */
    if (FAILED(__super::Add_Component(LEVEL_DUNGEON, TEXT("Prototype_Component_Model_DungeonOwlStatue"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    /* For.Com_Collider */
    CBounding_AABB::BOUNDING_AABB_DESC			ColliderDesc{};
    ColliderDesc.vExtents = _float3(1.f, 1.f, 1.f);
    ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vExtents.y, 0.f);

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
        TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
        return E_FAIL;
    m_pColliderCom->Set_Owner(this);

    return S_OK;
}

COwlStatue* COwlStatue::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    COwlStatue* pInstance = new COwlStatue(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : COwlStatue"));
        Safe_Release(pInstance);
    }

    return pInstance;
}
CGameObject* COwlStatue::Clone(void* pArg)
{
    COwlStatue* pInstance = new COwlStatue(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : COwlStatue"));
        Safe_Release(pInstance);
    }

    return pInstance;
}
void COwlStatue::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModelCom);
    Safe_Release(m_pColliderCom);
}
