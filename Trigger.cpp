#include "Includes.hpp"

CTriggerbot trigger;

//int CTriggerbot::GetBestTarget(CPlayer* m_pLocalPlayer)
//{
//	float lowestFOV = 999999.f;
//	int iBest = INVALID_AIM_TARGET;
//
//	for (int i = 0; i < MAX_PLAYERS_IN_ROOM; ++i)
//	{
//		auto pEnemy = g_pLTClientShell->GetPlayerByID(i);
//
//		if (!pEnemy)
//			continue;
//
//		if (!pEnemy->IsValidClient())
//			continue;
//
//		if (m_pLocalPlayer->bClientID == pEnemy->bClientID)
//			continue;
//
//		if (m_pLocalPlayer->bTeam == pEnemy->bTeam)
//			continue;
//
//		if (pEnemy->CharacterFX->IsSpawnProtected())
//			continue;
//
//		D3DXVECTOR3 Pos{ };
//		if (!engine.GetFirstVisBone(pEnemy->Object, Pos))
//			continue;
//
//		D3DXVECTOR3 HeadScreen;
//		if (!Tools::WorldToScreen(Pos, &HeadScreen))
//			continue;
//
//		D3DXVECTOR3 Screen(static_cast<float>(_ScreenW2), static_cast<float>(_ScreenH2), 0.0f);
//
//		auto Distance = D3DXVec3Length(&(HeadScreen - Screen));
//
//		if (HeadScreen.x >= _ScreenW2 - _radiusx && HeadScreen.x <= _ScreenW2 + _radiusx && HeadScreen.y >= _ScreenH2 - _radiusy && HeadScreen.y <= _ScreenH2 + _radiusy)
//		{
//			if (Distance < lowestFOV)
//			{
//				lowestFOV = Distance;
//				iBest = i;
//			}
//		}
//	}
//
//	return iBest;
//}

//void CTriggerbot::Run(CPlayer* m_pLocalPlayer, int W, int H, bool AutoShoot, int Key, FireType FType)
//{
//	_ScreenW2 = W;
//	_ScreenH2 = H;
//
//	_radiusx = 5 * (_ScreenW2 / 100);
//	_radiusy = 5 * (_ScreenH2 / 100);
//
//	auto Best = GetBestTarget(m_pLocalPlayer);
//	if (Best != INVALID_AIM_TARGET)
//	{
//		bool key_is_down = (GetAsyncKeyState(Key) & 0x8000) != 0;
//
//		if (AutoShoot)
//		{
//			mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
//			mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
//		}
//		else if (key_is_down && !AutoShoot)
//		{
//			mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
//			mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
//		}
//	}
//}

void CTriggerbot::Run( CPlayer* m_pLocalPlayer, CIntersectInfo* Info, CPlayerClient* m_pPlayerClient, bool AutoShoot, int Key, unsigned int Delay )
{
	/*if (m_pPlayerClient->GunCurrentAmmo <= 0)
		return;

	if (m_pPlayerClient->GetFireState() == 6)
		return;

	if (!Info->m_hObject)
		return;*/

	if ( !m_pLocalPlayer )
		return;

	if ( !m_pLocalPlayer->IsValidClient() )
		return;

	for ( int i = 0; i < MAX_PLAYERS_IN_ROOM; ++i )
	{
		auto pEnemy = g_pLTClientShell->GetPlayerByID( i );

		if ( !pEnemy )
			continue;

		if ( !pEnemy->IsValidClient() )
			continue;

		if ( m_pLocalPlayer->bClientID == pEnemy->bClientID )
			continue;

		if ( m_pLocalPlayer->bTeam == pEnemy->bTeam )
			continue;

		if ( pEnemy->CharacterFX->IsSpawnProtected() )
			continue;

		if ( Info->m_hObject == pEnemy->Object )
		{
			if ( !engine.IsEntireVisible( Info->m_hObject ) )
				continue;

			static DWORD dwDelayTick = GetTickCount();

			if ( GetTickCount() - dwDelayTick >= Delay )
			{
				dwDelayTick = GetTickCount();

				bool key_is_down = InputSys::ins().IsKeyDown( Key );

				if ( AutoShoot )
				{
					static DWORD dwLastTick = NULL;

					if ( !dwLastTick )
						dwLastTick = GetTickCount();

					if ( GetTickCount() - dwLastTick >= 100 )
					{
						dwLastTick = NULL;
						Tools::LeftButtonClick();
					}
				}
				else if ( key_is_down && !AutoShoot )
				{
					static DWORD dwLastTick = NULL;

					if ( !dwLastTick )
						dwLastTick = GetTickCount();

					if ( GetTickCount() - dwLastTick >= 100 )
					{
						dwLastTick = NULL;
						Tools::LeftButtonClick();
					}
				}
			}
		}
	}
}