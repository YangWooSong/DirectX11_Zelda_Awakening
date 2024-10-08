#include "stdafx.h"
#include "BombExplosion.h"
#include "GameInstance.h"

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
            for (auto& iter : m_Child_List)
                iter->SetActive(true);

            for (auto& iter : m_Child_List)
                iter->Priority_Update(fTimeDelta);

        }
}

void CBombExplosion::Update(_float fTimeDelta)
{
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

    if (m_isActive)
    {
        for (auto& iter : m_Child_List)
            iter->Render();
    }
    return S_OK;
}

HRESULT CBombExplosion::Ready_Child()
{
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

