#include "stdafx.h"
#include "State_Link_Slash_Hold.h"
#include "GameInstance.h"
#include "Model.h"
#include "Link.h"

CState_Link_Slash_Hold::CState_Link_Slash_Hold(CFsm* pFsm, CPlayer* pPlayer)
	:CState{ pFsm }
	, m_pPlayer{ pPlayer }
{
}

HRESULT CState_Link_Slash_Hold::Initialize(_uint iStateNum)
{
	m_iSlash_Hold_b_AnimIndex = m_pPlayer->Get_Model()->Get_AnimationIndex("slash_hold_b");
	m_iSlash_Hold_f_AnimIndex = m_pPlayer->Get_Model()->Get_AnimationIndex("slash_hold_f");
	m_iSlash_Hold_l_AnimIndex = m_pPlayer->Get_Model()->Get_AnimationIndex("slash_hold_l");
	m_iSlash_Hold_r_AnimIndex = m_pPlayer->Get_Model()->Get_AnimationIndex("slash_hold_r");
	m_iSlash_Hold_lp_AnimIndex = m_pPlayer->Get_Model()->Get_AnimationIndex("slash_hold_lp");
	m_iSlash_Hold_ed_AnimIndex = m_pPlayer->Get_Model()->Get_AnimationIndex("slash_hold_ed");
	m_iStateNum = iStateNum;

	m_fPlayerSpeed = m_pPlayer->Get_MoveSpeed();
	m_pPlayer_Navigation = m_pPlayer->Get_Navigation();
	return S_OK;
}

HRESULT CState_Link_Slash_Hold::Start_State()
{
	m_iPlayerDir = m_pPlayer->Get_Player_Dir();
	m_iCurrentAnimIndex = m_iSlash_Hold_lp_AnimIndex;
	m_pPlayer->Get_Model()->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.1f,true);
	m_pGameInstance->Play_Sound(TEXT("1_Sword_chargeStart.wav"), SOUND_PLAYER_EFFECT, 0.8f);
	return S_OK;
}

void CState_Link_Slash_Hold::Update(_float fTimeDelta)
{
	//충전 타이머
	if(m_iCurrentAnimIndex != m_iSlash_Hold_ed_AnimIndex)
		m_fTimer += fTimeDelta;

	if(m_fTimer > 1.f && m_bChargeEnd == false)
	{
		m_bChargeEnd = true;
		m_pGameInstance->Play_Sound(TEXT("1_Link_ChargeEnd.wav"), SOUND_PLAYER_EFFECT, 1.f);
	}

	//가만히 있는 애니로 돌아가기
	if (!(GetAsyncKeyState(0x57) & 0x8000) && !(GetAsyncKeyState(0x41) & 0x8000) && !(GetAsyncKeyState(0x53) & 0x8000) && !(GetAsyncKeyState(0x44) & 0x8000) && !(GetAsyncKeyState(0xA0) & 0x8000))
	{
		if(m_iCurrentAnimIndex != m_iSlash_Hold_lp_AnimIndex && m_iCurrentAnimIndex != m_iSlash_Hold_ed_AnimIndex)
		{
			m_iCurrentAnimIndex = m_iSlash_Hold_lp_AnimIndex;
			m_pPlayer->Get_Model()->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.1f);
			return;
		}
	}

	//걷는 소리
	if ((GetAsyncKeyState(0x57) & 0x8000)|| (GetAsyncKeyState(0x41) & 0x8000) || (GetAsyncKeyState(0x53) & 0x8000) || (GetAsyncKeyState(0x44) & 0x8000) || (GetAsyncKeyState(0xA0) & 0x8000))
	{
		if (m_iCurrentAnimIndex != m_iSlash_Hold_lp_AnimIndex && m_iCurrentAnimIndex != m_iSlash_Hold_ed_AnimIndex)
		{
			if (m_pPlayer->Get_LevelIndex() == LEVEL_FIELD)
				m_pGameInstance->Play_SoundRepeat(TEXT("1_Field_FootStep_Slow.wav"), SOUND_PLAYER, 0.3f);
			else
				m_pGameInstance->Play_SoundRepeat(TEXT("1_Inside_FootStep_Slow.wav"), SOUND_PLAYER, 0.7f);
		}
	}

	//휘두르기
	if (KEY_AWAY(KEY::P) &&m_iButttonAwayCount == 0)
	{
		m_iButttonAwayCount++;
		m_pGameInstance->Play_Sound(TEXT("1_Link_Sword_Charge_Slash.wav"), SOUND_PLAYER, 1.f);
		m_pGameInstance->Play_Sound(TEXT("1_Sword_AttackCharging.wav"), SOUND_PLAYER_EFFECT, 1.f);
		m_iCurrentAnimIndex = m_iSlash_Hold_ed_AnimIndex;
		m_pPlayer->Get_Model()->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.1f);
		m_pPlayer->Get_Model()->Set_AnimationSpeed(m_iCurrentAnimIndex, 50.f);
	}

	if (m_iPlayerDir == CPlayer::FRONT)
	{
		if (KEY_TAP(KEY::W))
		{
			if (m_iCurrentAnimIndex == m_iSlash_Hold_ed_AnimIndex)
				m_pPlayer->Change_State(CLink::IDLE);
			m_iCurrentAnimIndex = m_iSlash_Hold_f_AnimIndex;
			m_pPlayer->Get_Model()->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.1f, true);
			m_pPlayer->Get_Model()->Set_AnimationSpeed(m_iCurrentAnimIndex, 35.f);
		}
		else if (KEY_TAP(KEY::S))
		{
			if (m_iCurrentAnimIndex == m_iSlash_Hold_ed_AnimIndex)
				m_pPlayer->Change_State(CLink::IDLE);
			m_iCurrentAnimIndex = m_iSlash_Hold_b_AnimIndex;
			m_pPlayer->Get_Model()->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.1f, true);
			m_pPlayer->Get_Model()->Set_AnimationSpeed(m_iCurrentAnimIndex, 35.f);
		}
		else if (KEY_TAP(KEY::D))
		{
			if (m_iCurrentAnimIndex == m_iSlash_Hold_ed_AnimIndex)
				m_pPlayer->Change_State(CLink::IDLE);
			m_iCurrentAnimIndex = m_iSlash_Hold_r_AnimIndex;
			m_pPlayer->Get_Model()->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.1f, true);
			m_pPlayer->Get_Model()->Set_AnimationSpeed(m_iCurrentAnimIndex, 35.f);
		}
		else if (KEY_TAP(KEY::A))
		{
			if (m_iCurrentAnimIndex == m_iSlash_Hold_ed_AnimIndex)
				m_pPlayer->Change_State(CLink::IDLE);
			m_iCurrentAnimIndex = m_iSlash_Hold_l_AnimIndex;
			m_pPlayer->Get_Model()->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.1f, true);
			m_pPlayer->Get_Model()->Set_AnimationSpeed(m_iCurrentAnimIndex, 35.f);
		}
	}
	if (m_iPlayerDir == CPlayer::RIGHT)
	{
		if (KEY_TAP(KEY::W))
		{
			if (m_iCurrentAnimIndex == m_iSlash_Hold_ed_AnimIndex)
				m_pPlayer->Change_State(CLink::IDLE);
			m_iCurrentAnimIndex = m_iSlash_Hold_l_AnimIndex;
			m_pPlayer->Get_Model()->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.1f, true);
			m_pPlayer->Get_Model()->Set_AnimationSpeed(m_iCurrentAnimIndex, 35.f);
		}
		else if (KEY_TAP(KEY::S))
		{
			if (m_iCurrentAnimIndex == m_iSlash_Hold_ed_AnimIndex)
				m_pPlayer->Change_State(CLink::IDLE);
			m_iCurrentAnimIndex = m_iSlash_Hold_r_AnimIndex;
			m_pPlayer->Get_Model()->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.1f, true);
			m_pPlayer->Get_Model()->Set_AnimationSpeed(m_iCurrentAnimIndex, 35.f);
		}
		else if (KEY_TAP(KEY::D))
		{
			if (m_iCurrentAnimIndex == m_iSlash_Hold_ed_AnimIndex)
				m_pPlayer->Change_State(CLink::IDLE);
			m_iCurrentAnimIndex = m_iSlash_Hold_f_AnimIndex;
			m_pPlayer->Get_Model()->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.1f, true);
			m_pPlayer->Get_Model()->Set_AnimationSpeed(m_iCurrentAnimIndex, 35.f);
		}
		else if (KEY_TAP(KEY::A))
		{
			if (m_iCurrentAnimIndex == m_iSlash_Hold_ed_AnimIndex)
				m_pPlayer->Change_State(CLink::IDLE);
			m_iCurrentAnimIndex = m_iSlash_Hold_b_AnimIndex;
			m_pPlayer->Get_Model()->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.1f, true);
			m_pPlayer->Get_Model()->Set_AnimationSpeed(m_iCurrentAnimIndex, 35.f);
		}
	}
	if (m_iPlayerDir == CPlayer::LEFT)
	{
		if (KEY_TAP(KEY::W))
		{
			if (m_iCurrentAnimIndex == m_iSlash_Hold_ed_AnimIndex)
				m_pPlayer->Change_State(CLink::IDLE);
			m_iCurrentAnimIndex = m_iSlash_Hold_r_AnimIndex;
			m_pPlayer->Get_Model()->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.1f, true);
			m_pPlayer->Get_Model()->Set_AnimationSpeed(m_iCurrentAnimIndex, 35.f);
		}
		else if (KEY_TAP(KEY::S))
		{
			if (m_iCurrentAnimIndex == m_iSlash_Hold_ed_AnimIndex)
				m_pPlayer->Change_State(CLink::IDLE);
			m_iCurrentAnimIndex = m_iSlash_Hold_l_AnimIndex;
			m_pPlayer->Get_Model()->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.1f, true);
			m_pPlayer->Get_Model()->Set_AnimationSpeed(m_iCurrentAnimIndex, 35.f);
		}
		else if (KEY_TAP(KEY::D))
		{
			if (m_iCurrentAnimIndex == m_iSlash_Hold_ed_AnimIndex)
				m_pPlayer->Change_State(CLink::IDLE);
			m_iCurrentAnimIndex = m_iSlash_Hold_b_AnimIndex;
			m_pPlayer->Get_Model()->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.1f, true);
			m_pPlayer->Get_Model()->Set_AnimationSpeed(m_iCurrentAnimIndex, 35.f);
		}
		else if (KEY_TAP(KEY::A))
		{
			if (m_iCurrentAnimIndex == m_iSlash_Hold_ed_AnimIndex)
				m_pPlayer->Change_State(CLink::IDLE);
			m_iCurrentAnimIndex = m_iSlash_Hold_f_AnimIndex;
			m_pPlayer->Get_Model()->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.1f, true);
			m_pPlayer->Get_Model()->Set_AnimationSpeed(m_iCurrentAnimIndex, 40.f);
		}
	}
	if (m_iPlayerDir == CPlayer::BACK)
	{
		if (KEY_TAP(KEY::W))
		{
			if (m_iCurrentAnimIndex == m_iSlash_Hold_ed_AnimIndex)
				m_pPlayer->Change_State(CLink::IDLE);
			m_iCurrentAnimIndex = m_iSlash_Hold_b_AnimIndex;
			m_pPlayer->Get_Model()->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.1f, true);
			m_pPlayer->Get_Model()->Set_AnimationSpeed(m_iCurrentAnimIndex, 35.f);
		}
		else if (KEY_TAP(KEY::S))
		{
			if (m_iCurrentAnimIndex == m_iSlash_Hold_ed_AnimIndex)
				m_pPlayer->Change_State(CLink::IDLE);
			m_iCurrentAnimIndex = m_iSlash_Hold_f_AnimIndex;
			m_pPlayer->Get_Model()->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.1f, true);
			m_pPlayer->Get_Model()->Set_AnimationSpeed(m_iCurrentAnimIndex, 35.f);
		}
		else if (KEY_TAP(KEY::D))
		{
			if (m_iCurrentAnimIndex == m_iSlash_Hold_ed_AnimIndex)
				m_pPlayer->Change_State(CLink::IDLE);
			m_iCurrentAnimIndex = m_iSlash_Hold_l_AnimIndex;
			m_pPlayer->Get_Model()->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.1f, true);
			m_pPlayer->Get_Model()->Set_AnimationSpeed(m_iCurrentAnimIndex, 35.f);
		}
		else if (KEY_TAP(KEY::A))
		{
			if (m_iCurrentAnimIndex == m_iSlash_Hold_ed_AnimIndex)
				m_pPlayer->Change_State(CLink::IDLE);
			m_iCurrentAnimIndex = m_iSlash_Hold_r_AnimIndex;
			m_pPlayer->Get_Model()->SetUp_NextAnimation(m_iCurrentAnimIndex, 0.1f, true);
			m_pPlayer->Get_Model()->Set_AnimationSpeed(m_iCurrentAnimIndex, 35.f);
		}
	}

	if (KEY_HOLD(KEY::W))
	{
		m_pPlayer->Go_World_Straight(fTimeDelta, m_fPlayerSpeed-2.f, m_pPlayer_Navigation);
	}
	if (KEY_HOLD(KEY::S))
	{
		
		m_pPlayer->Go_World_Backward(fTimeDelta, m_fPlayerSpeed - 2.f, m_pPlayer_Navigation);
	}
	if (KEY_HOLD(KEY::D))
	{
		m_pPlayer->Go_World_Right(fTimeDelta, m_fPlayerSpeed - 2.f, m_pPlayer_Navigation);
	}
	if (KEY_HOLD(KEY::A))
	{
		m_pPlayer->Go_World_Left(fTimeDelta, m_fPlayerSpeed - 2.f, m_pPlayer_Navigation);
	}

	if (m_iCurrentAnimIndex == m_iSlash_Hold_ed_AnimIndex && m_pPlayer->Get_IsEnd_CurrentAnimation())
	{
		m_pPlayer->Change_State(CLink::IDLE);
		return;
	}
}

void CState_Link_Slash_Hold::End_State()
{
	m_fTimer = 0.f;
	m_bChargeEnd = false;
	m_iButttonAwayCount = 0;
}

CState_Link_Slash_Hold* CState_Link_Slash_Hold::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum)
{
	CState_Link_Slash_Hold* pInstance = new CState_Link_Slash_Hold(pFsm, pPlayer);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Link_Slash_Hold"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CState_Link_Slash_Hold::Free()
{
	__super::Free();
}
