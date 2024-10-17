#include "stdafx.h"
#include "Land.h"
#include "GameInstance.h"

CLand::CLand(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CGameObject(pDevice, pContext)
{
}

CLand::CLand(const CLand& Prototype)
    :CGameObject(Prototype)
{
}

HRESULT CLand::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CLand::Initialize(void* pArg)
{
     GAMEOBJECT_DESC* pDesc = static_cast<GAMEOBJECT_DESC*>(pArg);

    /* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    //Read한 정보 세팅
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&pDesc->vPosition));

    if (m_eObjType == NONANIM_LEVEL && m_iListIndex == 2)
    {
        m_pTransformCom->Set_Scaled(1.05f, 1.05f, 1.05f);
    }
    else
      m_pTransformCom->Set_Scaled(pDesc->vScale.x, pDesc->vScale.y, pDesc->vScale.z);
    m_pTransformCom->RotationThreeAxis(pDesc->vRotation);
    m_vRot = pDesc->vRotation;

   // m_pGameInstance->AddScene_ColMesh(this, TEXT("Land"));

    return S_OK;
}

void CLand::Priority_Update(_float fTimeDelta)
{
}

void CLand::Update(_float fTimeDelta)
{
}

void CLand::Late_Update(_float fTimeDelta)
{
    if (true == m_pGameInstance->isIn_Frustum_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 15.f))
    {
         __super::Late_Update(fTimeDelta);
         if(m_isActive)
         {
             m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
             m_pGameInstance->Add_RenderObject(CRenderer::RG_SHADOWOBJ, this);
         }
    }
}

HRESULT CLand::Render()
{
    if(m_isActive == true)
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
            if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", TEXTURE_TYPE::DIFFUSE,(_uint) i)))
                return E_FAIL;

            if(m_eObjType == NONANIM_LEVEL && m_iListIndex == 2)
            {
                if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", TEXTURE_TYPE::NORMALS, (_uint)i)))
                    return E_FAIL;

                if (FAILED(m_pShaderCom->Begin(5)))
                    return E_FAIL;
            }
            else
                if (FAILED(m_pShaderCom->Begin(0)))
                    return E_FAIL;

            if (FAILED(m_pModelCom->Render((_uint)i)))
                return E_FAIL;
        }
    }

    return S_OK;
}

HRESULT CLand::Render_LightDepth()
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

HRESULT CLand::Ready_Components()
{
    /* FOR.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxModel"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    switch (m_eObjType)
    {
    case NONANIM_LEVEL:
        Add_Component_SingleMap();
        break;
    case NONANIM_LEVEL_DUNGEON:
        Add_Component_Dungeon();
        break;
    case NONANIM_LEVEL_FIELD:
        Add_Component_Field();
        break;
    default:
        break;
    }

    return S_OK;
}

HRESULT CLand::Add_Component_Dungeon()
{
    m_isActive = false;

    _wstring sPath = TEXT("Prototype_Component_Level_Lv01TailCave_");
    _wstring finalName = sPath + m_DungeonList[m_iListIndex];

    /* FOR.Com_Model */
    if (FAILED(__super::Add_Component(LEVEL_DUNGEON, finalName,
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CLand::Add_Component_Field()
{
    m_isActive = true; 

    _wstring sPath = TEXT("Prototype_Component_Level_Field_");
    _wstring finalName = sPath + m_FieldList[m_iListIndex];

    /* FOR.Com_Model */
    if (FAILED(__super::Add_Component(LEVEL_FIELD, finalName,
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CLand::Add_Component_SingleMap()
{
    switch (m_iListIndex)
    {
    case 2:
        /* FOR.Com_Model */
        if (FAILED(__super::Add_Component(LEVEL_MARINHOUSE, TEXT("Prototype_Component_Level_MarinHouse"),
            TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
            return E_FAIL;
        break;
    case 3:
        /* FOR.Com_Model */
        if (FAILED(__super::Add_Component(LEVEL_STORE, TEXT("Prototype_Component_Level_Shop"),
            TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
            return E_FAIL;
        break;
    case 4:
        /* FOR.Com_Model */
        if (FAILED(__super::Add_Component(LEVEL_MARINHOUSE, TEXT("Prototype_Component_Level_TelephoneBox"),
            TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
            return E_FAIL;
        break;
    case 5:
        /* FOR.Com_Model */
        if (FAILED(__super::Add_Component(LEVEL_MARINHOUSE, TEXT("Prototype_Component_Level_Well"),
            TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
            return E_FAIL;
        break;
    }

    if(m_iListIndex != 2)
        m_pTransformCom->Set_Scaled(1.f, 1.f, 1.f);
    return S_OK;
}

CLand* CLand::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CLand* pInstance = new CLand(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CLand"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CLand::Clone(void* pArg)
{
    CLand* pInstance = new CLand(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CLand"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLand::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModelCom);
}
