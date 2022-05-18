#include "Includes.hpp"
#include <random>

CMemoryHacks memory;

void AntiAFKBot( bool bEnabled )
{
	if ( !bEnabled )
		return;

	static DWORD dwLastTick = NULL;

	if ( !dwLastTick )
		dwLastTick = GetTickCount();

	if ( GetTickCount() - dwLastTick >= 7000 )
	{
		dwLastTick = NULL;

		Tools::LeftButtonClick();
	}
}

void ActivateAntiFlash( bool IsActive )
{
	static DWORD dwAntiFlash = Tools::FindPattern< DWORD >( HASH_CSHELL_DLL, ( PBYTE )"\x72\x16\xC6\x46\x14\x00", XS( "xxxxxx" ) );

	if ( dwAntiFlash )
	{
		if ( IsActive )
		{
			if ( *( BYTE* )dwAntiFlash != 0x77 )
				Tools::memcpy_s( dwAntiFlash, "\x77", 1 );
		}
		else
		{
			if ( *( BYTE* )dwAntiFlash != 0x72 )
				Tools::memcpy_s( dwAntiFlash, "\x72", 1 );
		}
	}
}

void ActivateAntiSmoke( bool IsActive )
{
	static DWORD dwAntiSmoke = Tools::FindPattern< DWORD >( HASH_CSHELL_DLL, ( PBYTE )"\x83\x7F\x0C\x00\x77\x08\x57\x8B\xCE\xE8\x00\x00\x00\x00\x8B\x45\xE8", XS( "xxxxxxxxxx????xxx" ) );

	if ( !dwAntiSmoke )
		return;

	static DWORD DefaultValue = *( DWORD* )( dwAntiSmoke );

	static bool IsChanged = false;

	//ON:202145667 OFF:819075

	if ( IsActive )
	{
		if ( !IsChanged )
		{
			if ( *( DWORD* )( dwAntiSmoke ) == DefaultValue )
				*( DWORD* )( dwAntiSmoke ) = 202145667;

			IsChanged = true;
		}
	}
	else
	{
		if ( IsChanged )
		{
			*( DWORD* )( dwAntiSmoke ) = DefaultValue;
			IsChanged = false;
		}
	}
}

void ActivateSpyMode( bool IsActive )
{
	static DWORD dwSpyMode = Tools::FindPattern< DWORD >( HASH_CROSSFIRE_EXE, ( PBYTE )"\x72\x38\x8B\x55\xF4\x8B\x42\x28", XS( "xxxxxxxx" ) );

	if ( dwSpyMode )
	{
		if ( IsActive )
		{
			if ( *( BYTE* )dwSpyMode != 0xEB )
				Tools::memcpy_s( dwSpyMode, "\xEB", 1 );
		}
		else
		{
			if ( *( BYTE* )dwSpyMode != 0x72 )
				Tools::memcpy_s( dwSpyMode, "\x72", 1 );
		}
	}
}

void ActivateNoFallDmg( bool IsActive )
{
	auto TestValues = engine.GetTestValues();

	if ( !TestValues )
		return;

	static float BackupFall = TestValues->DamagePerMeter;

	if ( IsActive )
		TestValues->DamagePerMeter = 0.0f;
	else
		TestValues->DamagePerMeter = BackupFall;
}

void ActivateFastKnife( bool IsActive )
{
	if ( !IsActive )
		return;

	if ( !g_pLTClientShell )
		return;

	if ( !g_pLTClientShell->PlayerClient )
		return;

	if ( !g_pLTClientShell->PlayerClient->PlayerViewManager )
		return;

	if ( !g_pLTClientShell->PlayerClient->PlayerViewManager->ModelInstance )
		return;

	if ( !g_pLTClientShell->PlayerClient->PlayerViewManager->ModelInstance->Animation )
		return;

	if ( !g_pLTClientShell->PlayerClient->PlayerViewManager->ModelInstance->Animation->UNKOWN )
		return;

	g_pLTClientShell->PlayerClient->PlayerViewManager->ModelInstance->Animation->UNKOWN->Knife = 0;
}

void ActivateWeaponHacks( float FOVChanger, bool ChangeDelay, bool Reload, bool Recoil, bool Spread, bool Range, bool FastKnife, bool FastFire )
{
	//Weapon Hacks
	for ( int i = 0; i < MAX_WEAPONS_COUNT; ++i )
	{
		CWeapon* weapon_backup = GetBackupWeapon( i );
		CWeapon* weapon = engine.GetWeaponByIndex( i );

		if ( weapon != NULL && weapon_backup != NULL )
		{
			//Change Weapon FOV
			if ( FOVChanger >= 15.f )
				weapon->GetWeaponFOV() = FOVChanger;
			else
				weapon->GetWeaponFOV() = weapon_backup->GetWeaponFOV();

			//No change delay
			if ( ChangeDelay )
				weapon->ChangeWeaponAnimRatio() = weapon_backup->ChangeWeaponAnimRatio() * 2.5f;
			else
				weapon->ChangeWeaponAnimRatio() = weapon_backup->ChangeWeaponAnimRatio();

			//if ( weapon->GetWeaponClass() == Knife )
			//{
			//	for ( int x = 0; x < 4; ++x )
			//	{
			//		if ( FastKnife )
			//		{
			//			weapon->FastKnife1()[ x ] = std::round( config.Misc.FastKnifeSpeed ); //KnifeNormalAniRate
			//			weapon->FastKnife2()[ x ] = std::round( config.Misc.FastKnifeSpeed ); //KnifeNormalAniRate2
			//			weapon->FastKnife3()[ x ] = std::round( config.Misc.FastKnifeSpeed ); //KnifeBigshotAniRate
			//			weapon->FastKnife4()[ x ] = std::round( config.Misc.FastKnifeSpeed ); //KnifeBigshotAniRate2
			//		}
			//		else
			//		{
			//			weapon->FastKnife1()[ x ] = weapon_backup->FastKnife1()[ x ]; //KnifeNormalAniRate
			//			weapon->FastKnife2()[ x ] = weapon_backup->FastKnife2()[ x ]; //KnifeNormalAniRate2
			//			weapon->FastKnife3()[ x ] = weapon_backup->FastKnife3()[ x ]; //KnifeBigshotAniRate
			//			weapon->FastKnife4()[ x ] = weapon_backup->FastKnife4()[ x ]; //KnifeBigshotAniRate2
			//		}
			//	}
			//}

			if ( weapon->GetWeaponClass() == Grenades || weapon->GetWeaponClass() == Knife || weapon->GetWeaponClass() == C4 )
				continue;

			if ( FastFire )
			{
				if ( weapon->GetWeaponClass() == Sniper || weapon->GetWeaponClass() == Pistol )
					weapon->RepeatFire() = 2;
				else
				{
					static DWORD timer = 0;

					if ( !timer )
						timer = GetTickCount();

					if ( GetTickCount() - timer >= 70 )
					{
						timer = 0;
						g_pLTClientShell->PlayerClient->RapidFire();
					}
				}
			}
			else
				weapon->RepeatFire() = weapon_backup->RepeatFire();

			//Max Range
			if ( Range )
				weapon->Range() = 99999.f;
			else
				weapon->Range() = weapon_backup->Range();

			//No reload
			if ( Reload )
				weapon->ReloadAnimRatio() = weapon_backup->ReloadAnimRatio() * 2.f;
			else
				weapon->ReloadAnimRatio() = weapon_backup->ReloadAnimRatio();

			for ( int x = 0; x < 9; ++x )
			{
				//No Recoil
				if ( Recoil && weapon->GetWeaponClass() != Sniper && weapon->GetWeaponClass() != Pistol && weapon->GetWeaponClass() != Shotgun )
				{
					//g_pLTClientShell->PlayerClient->Recoil() = 0;
					//g_pLTClientShell->PlayerClient->RecoilZM() = 0;

					weapon->ShotReactPitch()[ x ] = 0.011f;
					weapon->ShotReactYaw()[ x ] = 0.011f;

					weapon->DetailReactPitchShot()[ x ] = 0.011f;
					weapon->DetailReactPitchShot2()[ x ] = 0.011f;
					weapon->DetailReactPitchShot3()[ x ] = 0.011f;

					weapon->DetailReactYawShot()[ x ] = 0.011f;
					weapon->DetailReactYawShot2()[ x ] = 0.011f;
					weapon->DetailReactYawShot3()[ x ] = 0.011f;
				}
				else
				{
					weapon->ShotReactPitch()[ x ] = weapon_backup->ShotReactPitch()[ x ];
					weapon->ShotReactYaw()[ x ] = weapon_backup->ShotReactYaw()[ x ];

					weapon->DetailReactPitchShot()[ x ] = weapon_backup->DetailReactPitchShot()[ x ];
					weapon->DetailReactPitchShot2()[ x ] = weapon_backup->DetailReactPitchShot2()[ x ];
					weapon->DetailReactPitchShot3()[ x ] = weapon_backup->DetailReactPitchShot3()[ x ];

					weapon->DetailReactYawShot()[ x ] = weapon_backup->DetailReactYawShot()[ x ];
					weapon->DetailReactYawShot2()[ x ] = weapon_backup->DetailReactYawShot2()[ x ];
					weapon->DetailReactYawShot3()[ x ] = weapon_backup->DetailReactYawShot3()[ x ];
				}

				//// No spread.
				if ( Spread && weapon->GetWeaponClass() != Sniper )
				{
					weapon->BulletPosOffset()[ x ] = 0.011f;
					weapon->PertubMin()[ x ] = 0.011f;
					weapon->PertubMax()[ x ] = 0.011f;

					weapon->DetailPerturbShot()[ x ] = 0.011f;
					weapon->DetailPerturbShot2()[ x ] = 0.011f;
					weapon->DetailPerturbShot3()[ x ] = 0.011f;

					weapon->CrossHairRatioPerRealSize()[ x ] = 0.0f;
				}
				else
				{
					weapon->BulletPosOffset()[ x ] = weapon_backup->BulletPosOffset()[ x ];
					weapon->PertubMin()[ x ] = weapon_backup->PertubMin()[ x ];
					weapon->PertubMax()[ x ] = weapon_backup->PertubMax()[ x ];

					weapon->DetailPerturbShot()[ x ] = weapon_backup->DetailPerturbShot()[ x ];
					weapon->DetailPerturbShot2()[ x ] = weapon_backup->DetailPerturbShot2()[ x ];
					weapon->DetailPerturbShot3()[ x ] = weapon_backup->DetailPerturbShot3()[ x ];

					weapon->CrossHairRatioPerRealSize()[ x ] = weapon_backup->CrossHairRatioPerRealSize()[ x ];
				}
			}
		}
	}
}

void ActivateBunnyhop( bool IsActive )
{
	static DWORD dwBunnyHop = NULL;

	if ( !dwBunnyHop )
		dwBunnyHop = *( DWORD* )( Tools::FindPattern< DWORD >( HASH_CSHELL_DLL, ( PBYTE )"\x0F\xB6\x05\x00\x00\x00\x00\x8B\xCF\x50\x8D\x45\xEC\x50\x8D\x45\xE4", XS( "xxx????xxxxxxxxxx" ) ) + 0x3 );

	if ( dwBunnyHop != NULL )
	{
		if ( IsActive )
			*( BYTE* )( dwBunnyHop ) = 0;
	}
}

//void ActivateNoBugDamage( bool IsActive )
//{
//	if ( !IsActive )
//		return;
//
//	*( float* )( Globals::cshell_dll + 0x79318240 - 0x78230000 + 0x4 ) = -99999999.0f && 'G';
//	*( float* )( Globals::cshell_dll + 0x7931824C - 0x78230000 + 0x4 ) = -99999999.0f && 'G';
//	*( float* )( Globals::cshell_dll + 0x79318204 - 0x78230000 + 0x4 ) = -99999999.0f && 'G';
//	*( float* )( Globals::cshell_dll + 0x793181CC - 0x78230000 + 0x4 ) = -99999999.0f && 'G';
//	*( float* )( Globals::cshell_dll + 0x793181D4 - 0x78230000 + 0x4 ) = -99999999.0f && 'G';
//	*( float* )( Globals::cshell_dll + 0x79197510 - 0x78230000 + 0x4 ) = -99999999.0f && 'G';
//	*( float* )( Globals::cshell_dll + 0x793181C4 - 0x78230000 + 0x4 ) = -99999999.0f && 'G';
//	*( float* )( Globals::cshell_dll + 0x793181EC - 0x78230000 + 0x4 ) = -99999999.0f && 'G';
//	*( float* )( Globals::cshell_dll + 0x79318218 - 0x78230000 + 0x4 ) = -99999999.0f && 'G';
//	*( float* )( Globals::cshell_dll + 0x79318220 - 0x78230000 + 0x4 ) = -99999999.0f && 'G';
//	*( float* )( Globals::cshell_dll + 0x79300AA0 - 0x78230000 + 0x4 ) = -99999999.0f && 'G';
//	*( float* )( Globals::cshell_dll + 0x79300A94 - 0x78230000 + 0x4 ) = -99999999.0f && 'G';
//	*( float* )( Globals::cshell_dll + 0x79300A70 - 0x78230000 + 0x4 ) = -99999999.0f && 'G';
//	*( float* )( Globals::cshell_dll + 0x79300A7C - 0x78230000 + 0x4 ) = -99999999.0f && 'G';
//	*( float* )( Globals::cshell_dll + 0x79318234 - 0x78230000 + 0x4 ) = -99999999.0f && 'G';
//}

//void ActivateInfiniteSpray( bool IsActive )
//{
//	if ( !IsActive )
//		return;
//
//	if ( !g_pLTClientShell )
//		return;
//
//	if ( !g_pLTClientShell->PlayerClient )
//		return;
//
//	auto PlayerClient = g_pLTClientShell->PlayerClient;
//
//	*( BYTE* )( DWORD( PlayerClient ) + 0x33F54 ) = 1;//->SprayValue() = 2;
//}

__forceinline void InGame()
{
	//AFK Bot
	AntiAFKBot( config.Misc.AFKBot );

	//Remove Fog
	static bool _fog = true;

	if ( config.Misc.RemoveFog && _fog )
	{
		_fog = false;
		engine.RunConsoleCmd( XS( "FogEnable 0" ) );
	}
	else if ( !config.Misc.RemoveFog && !_fog )
	{
		_fog = true;
		engine.RunConsoleCmd( XS( "FogEnable 1" ) );
	}

	//Remove Flash
	ActivateAntiFlash( config.Misc.NoFlash );

	//Remove Smoke
	ActivateAntiSmoke( config.Misc.NoSmoke );

	//No Bug Damage
//	ActivateNoBugDamage( config.Misc.NoBugDamage );

	CPlayer* pLocal = g_pLTClientShell->GetLocalPlayer();
	if ( pLocal && pLocal->Object && pLocal->CharacterFX && pLocal->Health > 0 )
	{
		//Fast Knife
		ActivateFastKnife( config.Misc.FastKnife );

		//Spy Mode
		ActivateSpyMode( Globals::g_bSpyModeActive );

		//No Fall Dmg
		ActivateNoFallDmg( config.Misc.NoFallDamage );

		//Weapon Hacks
		ActivateWeaponHacks(
			config.Misc.WeaponFOV,
			config.Misc.FastWeaponChange,
			config.Misc.FastReload,
			config.Misc.LessRecoil,
			config.Misc.LessSpread,
			config.Misc.MaxWeaponRange,
			config.Misc.FastKnife,
			config.Misc.FastFire );

		//static bool bInit = false;
		//static float old = 0.0f;
		//auto bEnable = config.misc[18] && (GetAsyncKeyState(config.custom_keys[2]) & 0x8000) != 0;

		//if (!bInit && bEnable)
		//{
		//	old = g_pLTClientShell->PlayerClient->Gravity();
		//	bInit = true;
		//}

		////Fly
		//static bool bWasEnabled = false;
		//if (bEnable)
		//{
		//	g_pLTClientShell->PlayerClient->Gravity() = -2.125f;
		//	bWasEnabled = true;
		//}
		//else if (bWasEnabled)
		//{
		//	g_pLTClientShell->PlayerClient->Gravity() = old;
		//	bWasEnabled = false;
		//}

		//Bunnyhop
		ActivateBunnyhop( config.Misc.Bunnyhop );
	}
	else
	{
		Globals::g_bSpyModeActive = false;
		ActivateSpyMode( false );
		ActivateWeaponHacks( 0.f, false, false, false, false, false, false, false );
		ActivateBunnyhop( false );
	}
}

void ActivateBypasses( bool IsActive )
{
	static DWORD Bypass1 = NULL;

	//0xC3
	if ( !Bypass1 )
	{
		Bypass1 = Tools::FindPattern< DWORD >( HASH_CSHELL_DLL, PBYTE( "\xE8\x00\x00\x00\x00\x56\xE8\x00\x00\x00\x00\x8A\x87\x00\x00\x00\x00" ), XS( "x????xx????xx????" ) );
		if ( Bypass1 ) Bypass1 = Tools::ResolveRelative( Bypass1 );
	}

	//0xEB
	static DWORD Bypass2 = Tools::FindPattern< DWORD >( HASH_CSHELL_DLL, PBYTE( "\x74\x40\x8B\x01\xFF\x90\x00\x00\x00\x00" ), XS( "xxxxxx????" ) );

	static bool OldActive = false;

	if ( IsActive && !OldActive )
	{
		OldActive = true;

		if ( Bypass1 && *( BYTE* )Bypass1 != 0xC3 )
			Tools::memcpy_s( Bypass1, "\xC3", 1 );

		if ( Bypass2 && *( BYTE* )Bypass2 != 0xEB )
			Tools::memcpy_s( Bypass2, "\xEB", 1 );
	}
	else if ( !IsActive && OldActive )
	{
		OldActive = false;

		if ( Bypass1 )
			Tools::memcpy_s( Bypass1, "\x55", 1 );

		if ( Bypass2 )
			Tools::memcpy_s( Bypass2, "\x74", 1 );
	}
}

void Marquee( std::string& clantag )
{
	std::string temp = clantag;
	clantag.erase( 0, 1 );
	clantag += temp[ 0 ];
}

void RadioSpammer()
{
	static std::string clantag = " [lol] ";
	static DWORD lasttick = 0;

	if ( !config.Misc.RadioSpam )
	{
		lasttick = 0;
		return;
	}

	if ( GetTickCount() - lasttick >= 250 )
	{
		lasttick = GetTickCount();
		Marquee( clantag );
	}

	CAutoMessage msg;

	uint8 v30 = engine.GetS2SCounter();
	engine.GetS2SCounter() = v30 + 1;

	msg.WriteBits( 131, 16 );

	v30 = 16 * v30 | ( v30 >> 4 ) & 0xF;

	msg.WriteBits( 0xB27AD5F7, 5 );
	msg.Writeuint8( v30 );
	msg.Writeuint8( rand() % 15 );

	msg.WriteString( clantag.c_str() );

	msg.Writeuint8( 0 );
	msg.Writeuint8( 0 );
	msg.Writeuint8( 0 );

	msg.Writebool( FALSE );

	auto v19 = msg.Size();
	msg.Writeuint8( ( uint8 )( v19 * v30 ) );

	g_pLTClient->SendToServer( msg.Read(), MESSAGE_GUARANTEED );
}

//void DamageTest()
//{
//	CAutoMessage msg;
//
//	auto local_player = g_pLTClientShell->GetLocalPlayer();
//
//	if ( !local_player )
//		return;
//
//	uint16 v17 = 0;
//
//	uint8 v16 = *( uint8* )( Globals::cshell_dll + 0x1A7D218 );
//	*( uint8* )( Globals::cshell_dll + 0x1A7D218 ) = v16 + 1;
//
//	msg.WriteBits( 68, 16 );
//
//	v16 = 16 * v16 | ( v16 >> 4 ) & 0xF;
//
//	msg.WriteBits( 0xB27AD5F7, 5 );
//	msg.Writeuint8( v16 );
//	msg.Writeuint16( 0 );
//	msg.Writeuint16( v17 );
//	msg.Writeuint8( 7 );
//	msg.Writeuint8( local_player->bClientID );
//	msg.Writebool( FALSE );
//	msg.WriteTimer( 0 );
//	
//	for ( int i = 0; i < MAX_PLAYERS_IN_ROOM; ++i )
//	{
//		auto player = g_pLTClientShell->GetPlayerByID( i );
//
//		if ( player && player->IsValidClient() && player->bClientID != local_player->bClientID && player->bTeam != local_player->bTeam )
//		{
//			D3DXVECTOR3 bone{ };
//			engine.GetBonePosition( player->Object, "M-bone Head", &bone );
//
//			msg.WriteType( bone );
//			break;
//		}
//	}
//
//	auto v19 = msg.Size();
//	msg.Writeuint8( ( uint8 )( v19 * v16 ) );
//
//	g_pLTClient->SendToServer( msg.Read(), MESSAGE_GUARANTEED );
//}

void RegenNanoHP()
{
	if ( !engine.IsMutantRoom() )
		return;

	CPlayer* pLocal = g_pLTClientShell->GetLocalPlayer();

	if ( !pLocal )
		return;

	if ( pLocal->Health <= 0 )
		return;

	if ( pLocal->Health > 100 )
	{
		CAutoMessage msg;

		uint8 counter = engine.GetS2SCounter();
		engine.GetS2SCounter() = counter + 1;

		msg.Writeuint16( MSG_CS_NANO_REGAIN_HP_START );
		counter = 16 * counter | ( counter >> 4 ) & 0xF;
		msg.WriteBits( 0xB27AD5F7, 5 );
		msg.Writeuint8( counter );
		auto msg_len = msg.Size();
		msg.Writeuint8( ( uint8 )( msg_len * counter ) );

		g_pLTClient->SendToServer( msg.Read(), MESSAGE_GUARANTEED );
	}
}

void LagRoom()
{
	static bool a = false;

	if ( InputSys::ins().WasKeyPressed( config.Misc.LagRoomKey ) )
		a = !a;

	if ( !a )
		return;

	for ( int i = 0; i < 4096 * 4; ++i )
	{
		if ( !a )
			break;

		CAutoMessage msg;

		msg.Writeuint16( MSG_CS_BOOMGRENADE );
		msg.WriteBits( 0xB27AD5F7, 5 );
		msg.Writeuint8( 0 );
		msg.Writeuint16( 6 );
		msg.Writeuint8( 0 );

		g_pLTClient->SendToServer( msg.Read(), MESSAGE_GUARANTEED );
	}
}

void NanoDecide()
{
	CPlayer* pLocal = g_pLTClientShell->GetLocalPlayer();

	if ( pLocal && pLocal->Health > 0 && pLocal->CharacterFX->IsMutant() )
	{
		CAutoMessage msg;

		uint8 counter = engine.GetS2SCounter();
		engine.GetS2SCounter() = counter + 1;

		msg.Writeuint16( MSG_CS_INCREASE_DECIDE_NANO );
		counter = 16 * counter | ( counter >> 4 ) & 0xF;
		msg.WriteBits( 0xB27AD5F7, 5 );
		msg.Writeuint8( counter );
		auto msg_len = msg.Size();
		msg.Writeuint8( ( uint8 )( msg_len * counter ) );

		g_pLTClient->SendToServer( msg.Read(), MESSAGE_GUARANTEED );
	}
}

void RoomCrasher()
{
	static bool a = false;

	if ( InputSys::ins().WasKeyPressed( config.Misc.CrashRoomKey ) )
		a = !a;

	if ( !a )
		return;

	a = false;

	CAutoMessage msg;
	msg.Writeuint16( MSG_CS_TUTORIAL_END );
	g_pLTClient->SendToServer( msg.Read(), MESSAGE_GUARANTEED );
}

void CMemoryHacks::Update()
{
	Globals::g_bSpyModeActive = config.Misc.SpyMode;

	if ( engine.IsInGame() && Globals::g_bIsSafeToHook )
	{
		InGame();
		ActivateBypasses( true );
		RadioSpammer();
		LagRoom();
		RoomCrasher();

		//	RegenNanoHP();
		//	NanoDecide();
	}
	else
	{
		//Reset all values to default.
		ActivateBypasses( false );
		ActivateAntiSmoke( false );
		ActivateSpyMode( false );
		ActivateAntiFlash( false );
		ActivateNoFallDmg( false );
		ActivateWeaponHacks( 0.f, false, false, false, false, false, false, false );
		ActivateBunnyhop( false );
	}
}