#include "stdafx.h"
#include "TreasureBox.h"
#include "GameInstance.h"
#include "Link.h"
#include "ItemUI.h"
#include "PlayerCamera.h"

CTreasureBox::CTreasureBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CGameObject(pDevice, pContext)
{
}

CTreasureBox::CTreasureBox(const CTreasureBox& Prototype)
    :CGameObject(Prototype)
{
}
HRESULT CTreasureBox::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CTreasureBox::Initialize(void* pArg)
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
    m_iCellNum = pDesc->iCellNum;

    m_isActive = false;

    m_iCurrentAnimIndex = m_pModelCom->Get_AnimationIndex("close_wait");
    m_pModelCom->SetUp_Animation(m_iCurrentAnimIndex, true);

    Set_Item();

    if (m_iRoomNum == 3 || m_iRoomNum == 6 || m_iRoomNum == 7)
        m_bShow = true;

    if (m_iCellNum == 523)
        m_bShow = false;

    if (m_bShowAction == false)
        m_fBrightness = 1.f;

    return S_OK;
}

void CTreasureBox::Priority_Update(_float fTimeDelta)
{
}

void CTreasureBox::Update(_float fTimeDelta)
{
    if (m_isActive)
    {
        m_pSoundCom->Update(fTimeDelta);
        m_pModelCom->Play_Animation(fTimeDelta);
        m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_Ptr());

        if (m_iCurrentAnimIndex == m_pModelCom->Get_AnimationIndex("open") && m_pModelCom->Get_IsEnd_CurrentAnimation() && m_bCangePlayerState == false)
        {
            m_bCangePlayerState = true;
            static_cast<CLink*>(m_pGameInstance->Find_Player(LEVEL_DUNGEON))->Change_State(CLink::GET_ITEM);
        }

        //활성화 되면 피직스에 추가
        if(m_bAdd == false)
        {
            m_bAdd = true;
            m_pGameInstance->AddScene_ColMesh(this, TEXT("TreasureBox"));
        }
      

        //숨겨있던 상자가 활성화 될 때 셰이더 효과
        if (m_bShowAction)
        {
            Set_Brightness(fTimeDelta);

            if (m_fBrightness == 1.f && m_bGimmickSound == false)
            {
                m_bGimmickSound = true;
                m_pGameInstance->Pause_BGM();
                m_pSoundCom->Play_Sound(TEXT("5_GimmickSolve.wav"), 1.f);
            }
            else if (m_bGimmickSound && m_bBGMSound == false && m_pSoundCom->Get_IsPlaying() == false)
            { 
                m_bBGMSound = true;
                m_pGameInstance->Pause_BGM();
            }
        }


        //153번 타일 상자의 카메라 세팅
        if (m_iCellNum == 523 && m_bCameraAction == false)
        {
            m_bCameraAction = true;
            _float3 v_Offset = _float3{ 0.0f, 20.0f, -11.f };
            static_cast<CPlayerCamera*>(m_pGameInstance->Find_Camera(LEVEL_DUNGEON))->Set_FollowPlayer(false);
            static_cast<CPlayerCamera*>(m_pGameInstance->Find_Camera(LEVEL_DUNGEON))->Set_TargetToOtherPos(m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMLoadFloat3(&v_Offset));
        }
        else if(m_bCameraAction)
            m_fCameraTimer += fTimeDelta;

        if(m_fCameraTimer > 2.5f)
            static_cast<CPlayerCamera*>(m_pGameInstance->Find_Camera(LEVEL_DUNGEON))->Set_FollowPlayer(true);
    }
    else
    {
        if (m_bPlayAlarm && static_cast<CLink*>(m_pGameInstance->Find_Player(LEVEL_DUNGEON))->Get_CurRoomNum() != m_iRoomNum)
            m_bPlayAlarm = false;
    }

    Play_Alarm();

    if (m_bOpened )
        m_pColliderCom->Set_IsActive(false);
}

void CTreasureBox::Late_Update(_float fTimeDelta)
{
    if (m_isActive)
    {
        __super::Late_Update(fTimeDelta);
        m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
        m_pGameInstance->Add_ColliderList(m_pColliderCom);

#ifdef _DEBUG
        m_pGameInstance->Add_DebugObject(m_pColliderCom);
#endif
    }
}

HRESULT CTreasureBox::Render()
{
    if (m_isActive)
    {
        if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
            return E_FAIL;
        if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_RawValue("g_fBrightness", &m_fBrightness, sizeof(_float))))
            return E_FAIL;

        _uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

        for (size_t i = 0; i < iNumMeshes; i++)
        {
            m_pModelCom->Bind_MeshBoneMatrices(m_pShaderCom, "g_BoneMatrices", (_uint)i);

            if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", TEXTURE_TYPE::DIFFUSE, (_uint)i)))
                return E_FAIL;


            if (FAILED(m_pShaderCom->Begin(4)))
                return E_FAIL;

            if (FAILED(m_pModelCom->Render((_uint)i)))
                return E_FAIL;
        }
    }

    return S_OK;
}

void CTreasureBox::OnCollisionEnter(CGameObject* pOther)
{
}

void CTreasureBox::OnCollisionStay(CGameObject* pOther)
{
    if (m_pColliderCom->Get_IsColl())
    {
        if (pOther->Get_LayerTag() == TEXT("Layer_Player"))
        {
            if (KEY_TAP(E) && m_bOpened == false)
            {
                m_bOpened = true;
                m_iCurrentAnimIndex = m_pModelCom->Get_AnimationIndex("open");
                m_pModelCom->SetUp_Animation(m_iCurrentAnimIndex, false);
                m_pModelCom->Set_AnimationSpeed(m_iCurrentAnimIndex, 60.f);
                Change_PlayerUI_TextureNum();
                m_pSoundCom->Play_Sound(TEXT("4_Obj_Unlock Treasure Box.wav"), 0.8f);
            }
        }
    }
}


void CTreasureBox::OnCollisionExit(CGameObject* pOther)
{
}

HRESULT CTreasureBox::Ready_Components()
{
    /* FOR.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* FOR.Com_Model */
    if (FAILED(__super::Add_Component(LEVEL_DUNGEON, TEXT("Prototype_Component_Model_TreasureBox"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    /* For.Com_Collider */
    CBounding_AABB::BOUNDING_AABB_DESC			ColliderDesc{};
    ColliderDesc.vExtents = _float3(0.65f, 1.f, 1.f);
    ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vExtents.y, -0.5f);

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

void CTreasureBox::Change_PlayerUI_TextureNum()
{
    switch (m_iRoomNum)
    {
    case 3:
        static_cast<CPlayer*>(m_pGameInstance->Find_Player(LEVEL_DUNGEON))->Change_PlayerUI_TextureNum(CLink::ITEM_ICON_UI, CItemUI::CAMPUS);
        break;
    case 4:
        static_cast<CPlayer*>(m_pGameInstance->Find_Player(LEVEL_DUNGEON))->Change_PlayerUI_TextureNum(CLink::ITEM_ICON_UI, CItemUI::SMALLKEY);
        break;
    case 5:
        static_cast<CPlayer*>(m_pGameInstance->Find_Player(LEVEL_DUNGEON))->Change_PlayerUI_TextureNum(CLink::ITEM_ICON_UI, CItemUI::MAP);
        break;
    case 6:
        static_cast<CPlayer*>(m_pGameInstance->Find_Player(LEVEL_DUNGEON))->Change_PlayerUI_TextureNum(CLink::ITEM_ICON_UI, CItemUI::LUPEE);
        break;
    case 7:
        if(m_iCellNum == 540 || m_iCellNum == 541)
            static_cast<CPlayer*>(m_pGameInstance->Find_Player(LEVEL_DUNGEON))->Change_PlayerUI_TextureNum(CLink::ITEM_ICON_UI, CItemUI::SMALLKEY);
        if(m_iCellNum == 621 || m_iCellNum == 622)
            static_cast<CPlayer*>(m_pGameInstance->Find_Player(LEVEL_DUNGEON))->Change_PlayerUI_TextureNum(CLink::ITEM_ICON_UI, CItemUI::BOSSKEY);
        if (m_iCellNum == 523)
            static_cast<CPlayer*>(m_pGameInstance->Find_Player(LEVEL_DUNGEON))->Change_PlayerUI_TextureNum(CLink::ITEM_ICON_UI, CItemUI::LUPEE);
        break;
    case 14:
        static_cast<CPlayer*>(m_pGameInstance->Find_Player(LEVEL_DUNGEON))->Change_PlayerUI_TextureNum(CLink::ITEM_ICON_UI, CItemUI::STONEBEAK);
        break;
    case 15:
        static_cast<CPlayer*>(m_pGameInstance->Find_Player(LEVEL_DUNGEON))->Change_PlayerUI_TextureNum(CLink::ITEM_ICON_UI, CItemUI::FEATHER);
        break;
    default:
        break;
    }
}

void CTreasureBox::Set_Item()
{
    switch (m_iRoomNum)
    {
    case 3:
        m_iItemIndex = CItemUI::CAMPUS;
        break;
    case 4:
        m_iItemIndex = CItemUI::SMALLKEY;
        m_bShowAction = true;
        break;
    case 5:
        m_iItemIndex = CItemUI::MAP;
        m_bShowAction = true;
        break;
    case 6:
        m_iItemIndex = CItemUI::LUPEE;
        break;
    case 7:
        if (m_iCellNum == 540 || m_iCellNum == 541)
            m_iItemIndex = CItemUI::SMALLKEY;
        if (m_iCellNum == 621 || m_iCellNum == 622)
            m_iItemIndex = CItemUI::BOSSKEY;
        if (m_iCellNum == 523)
        {
            m_iItemIndex = CItemUI::LUPEE;
            m_bShowAction = true;
        }
        break;
    case 14:
        m_iItemIndex = CItemUI::STONEBEAK;
        m_bShowAction = true;
        break;
    case 15:
        m_iItemIndex = CItemUI::FEATHER;
        m_bShowAction = true;
        break;
    default:
        break;
    }
}

void CTreasureBox::Play_Alarm()
{
    if (m_bOpened == false && 
        static_cast<CLink*>(m_pGameInstance->Find_Player(LEVEL_DUNGEON))->IsGetCampus() && 
        static_cast<CLink*>(m_pGameInstance->Find_Player(LEVEL_DUNGEON))->Get_CurRoomNum() == m_iRoomNum&&
        m_bPlayAlarm == false && m_iItemIndex == CItemUI::SMALLKEY)
    {
        m_pSoundCom->Play_Sound(TEXT("5_UI_Campus_Alarm.wav", 1.f));
        m_bPlayAlarm = true;
    }
}

void CTreasureBox::Set_Brightness(_float fTimeDelta)
{
    if(m_fBrightness == 15.f)
        m_pSoundCom->Play_Sound(TEXT("4_Obj_TreasureBoxShow.wav", 1.f));

    m_fBrightness = max(1.f, m_fBrightness - fTimeDelta* 12.f );
}

CTreasureBox* CTreasureBox::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CTreasureBox* pInstance = new CTreasureBox(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CTreasureBox"));
        Safe_Release(pInstance);
    }

    return pInstance;
}
CGameObject* CTreasureBox::Clone(void* pArg)
{
    CTreasureBox* pInstance = new CTreasureBox(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CTreasureBox"));
        Safe_Release(pInstance);
    }

    return pInstance;
}
void CTreasureBox::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModelCom);
    Safe_Release(m_pColliderCom);
    Safe_Release(m_pSoundCom);
}
