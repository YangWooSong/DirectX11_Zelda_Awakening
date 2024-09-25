#include "stdafx.h"
#include "OnewayDoorReverse.h"
#include "GameInstance.h"
#include "Link.h"

COnewayDoorReverse::COnewayDoorReverse(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CGameObject(pDevice, pContext)
{
}

COnewayDoorReverse::COnewayDoorReverse(const COnewayDoorReverse& Prototype)
    :CGameObject(Prototype)
{
}
HRESULT COnewayDoorReverse::Initialize_Prototype()
{
    return S_OK;
}

HRESULT COnewayDoorReverse::Initialize(void* pArg)
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

    m_iOpenAnimIndex = m_pModelCom->Get_AnimationIndex("open1");
    m_iCloseAnimIndex = m_pModelCom->Get_AnimationIndex("close1");

    if(m_iRoomNum == 10)
    {
        m_iCurrentAnimIndex = m_pModelCom->Get_AnimationIndex("open_wait1");
        m_bOpend = false;
    }
    else
    {
        m_iCurrentAnimIndex = m_pModelCom->Get_AnimationIndex("close_wait1");
        m_bOpend = true;
    }

    m_pModelCom->SetUp_Animation(m_iCurrentAnimIndex, false);
    m_pModelCom->Set_AnimationSpeed(m_iCurrentAnimIndex, 40);

    Set_LayerTag(TEXT("Layer_Monster"));
    m_pGameInstance->AddScene_ColMesh(this, TEXT("OnewayDoorReverse"));
    return S_OK;
}

void COnewayDoorReverse::Priority_Update(_float fTimeDelta)
{
}

void COnewayDoorReverse::Update(_float fTimeDelta)
{
    if (m_isActive)
    {
        if (m_bOpend == true && m_fTimer == 0.f)
        {
            m_pSoundCom->Play_Sound(TEXT("4_Obj_OnewayDoor_Open.wav"), 0.8f);
            m_iCurrentAnimIndex = m_iOpenAnimIndex;
            m_pModelCom->SetUp_NextAnimation(m_iCurrentAnimIndex, false);
        }
        else if (m_bOpend == false && m_fTimer == 0.f)
        {
            m_pSoundCom->Play_Sound(TEXT("4_Obj_OnewayDoor_Close.wav"), 0.8f);
            m_iCurrentAnimIndex = m_iCloseAnimIndex;
            m_pModelCom->SetUp_NextAnimation(m_iCurrentAnimIndex, false);
        }
        m_fTimer += fTimeDelta;
        m_pModelCom->Play_Animation(fTimeDelta);
    }

    if (m_bOpend)
    {
        m_pGameInstance->Destroy_PhysXActor(this);
    }
}

void COnewayDoorReverse::Late_Update(_float fTimeDelta)
{
    if (m_isActive)
    {
        __super::Late_Update(fTimeDelta);
        m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
    }
}

HRESULT COnewayDoorReverse::Render()
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

void COnewayDoorReverse::OnCollisionEnter(CGameObject* pOther)
{
}

void COnewayDoorReverse::OnCollisionStay(CGameObject* pOther)
{

}

void COnewayDoorReverse::OnCollisionExit(CGameObject* pOther)
{
}

HRESULT COnewayDoorReverse::Ready_Components()
{
    /* FOR.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* FOR.Com_Model */
    if (FAILED(__super::Add_Component(LEVEL_DUNGEON, TEXT("Prototype_Component_Model_OnewayDoorReverse"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    /* FOR.Com_Sound */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sound"),
        TEXT("Com_Sound"), reinterpret_cast<CComponent**>(&m_pSoundCom))))
        return E_FAIL;
    m_pSoundCom->Set_Owner(this);

    return S_OK;
}

COnewayDoorReverse* COnewayDoorReverse::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    COnewayDoorReverse* pInstance = new COnewayDoorReverse(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : COnewayDoorReverse"));
        Safe_Release(pInstance);
    }

    return pInstance;
}
CGameObject* COnewayDoorReverse::Clone(void* pArg)
{
    COnewayDoorReverse* pInstance = new COnewayDoorReverse(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : COnewayDoorReverse"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void COnewayDoorReverse::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pSoundCom);
    Safe_Release(m_pModelCom);
}
