#include "stdafx.h"
#include "BombExplosion.h"
#include "GameInstance.h"
#include "Particle_Image.h"
#include "Flash_Effect.h"
#include "Client_Defines.h"
CBombExplosion::CBombExplosion(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :C2DEffects(pDevice, pContext)
{
}


CBombExplosion::CBombExplosion(const CBombExplosion& Prototype)
    :C2DEffects(Prototype)
{
}

HRESULT CBombExplosion::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CBombExplosion::Initialize(void* pArg)
{
    /* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Child()))
        return E_FAIL;

    m_isActive = false;


    return S_OK;
}

void CBombExplosion::Priority_Update(_float fTimeDelta)
{
    
    if (m_isActive)
    {
        if(m_bActiveChild == false)
        {
            m_bActiveChild = true;

            for (auto& iter : m_Child_List)
                iter->SetActive(true);
        }

        for (auto& iter : m_Child_List)
            iter->Priority_Update(fTimeDelta);

    }
    else
    {
        if (m_bActiveChild)
        {
            m_bActiveChild = false;

            for (auto& iter : m_Child_List)
                iter->SetActive(false);
        }
    }
}

void CBombExplosion::Update(_float fTimeDelta)
{
    _vector vParentPos = m_pParentObj->Get_Transform()->Get_State(CTransform::STATE_POSITION);
    vParentPos = XMVectorSetZ(vParentPos, (XMVectorGetZ(vParentPos) + 1.f));
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, vParentPos);

    if (m_isActive)
    {
        for (auto& iter : m_Child_List)
            iter->Update(fTimeDelta);
    }
}

void CBombExplosion::Late_Update(_float fTimeDelta)
{
    if (m_isActive)
    {
        for (auto& iter : m_Child_List)
            iter->Late_Update(fTimeDelta);
    }
}

HRESULT CBombExplosion::Render()
{

    return S_OK;
}

HRESULT CBombExplosion::Ready_Child()
{
    CGameObject* pGameObj = m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Smoke_Effect"));
    C2DEffects::EFFECT_DESC pDesc{};
    pDesc.iLevelIndex = m_iLevelIndex;
    pDesc.pParent = this;
    pDesc.iEffectType = m_iEffectType;
    if (pGameObj != nullptr)
    {
        pDesc.fColor = { 1.f, 0.77f, 0.f, 1.f };
        pDesc.vScale = { 5.f,5.f,1.f };
        CGameObject* m_pEffect = dynamic_cast<CGameObject*>(pGameObj->Clone(&pDesc));
        m_Child_List.push_back(m_pEffect);
    }

    pGameObj = m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_Flash_Effect"));

    if (pGameObj != nullptr)
    {
        CFlash_Effect::FLASH_DESC _Desc{};
        _Desc.iEffectType = BOMB_EXPLOSION_EFFECT;
        _Desc.fColor = { 1.f,1.f,0.8f,1.f };
        _Desc.pParent = this;
        _Desc.iLevelIndex = m_iLevelIndex;
        _Desc.iTextureNum = 1;
        _Desc.vScale = { 7.f,7.f,1.f };
        CGameObject* p2DEffect = dynamic_cast<CGameObject*>(pGameObj->Clone(&_Desc));
        m_Child_List.push_back(p2DEffect);

        _Desc.fColor = { 1.f,1.f,0.8f,1.f };
        _Desc.pParent = this;
        _Desc.iLevelIndex = m_iLevelIndex;
        _Desc.iTextureNum = 0;
        _Desc.vScale = { 5.f,5.f,1.f };
        p2DEffect = dynamic_cast<CGameObject*>(pGameObj->Clone(&_Desc));
        m_Child_List.push_back(p2DEffect);
    }

    return S_OK;
}

CBombExplosion* CBombExplosion::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CBombExplosion* pInstance = new CBombExplosion(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CBombExplosion"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CBombExplosion::Clone(void* pArg)
{
    CBombExplosion* pInstance = new CBombExplosion(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CBombExplosion"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBombExplosion::Free()
{
    for (auto& iter : m_Child_List)
        Safe_Release(iter);
    m_Child_List.clear();
    __super::Free();

    Safe_Release(m_pParticle);
}

