#include "Includes.hpp"

CAimbot aimbot;

bool CAimbot::IsInFOV( float angle, const D3DXVECTOR3& camera, const D3DXVECTOR3& target )
{
	/*angle /= 2;

	D3DXVECTOR3 new_angles( NULL, NULL, NULL );
	D3DXVECTOR3 cur_angles( NULL, NULL, NULL );
	Math::VectorAngles( target, camera, new_angles );

	new_angles[ YAW ] = Math::AngleNormalize( new_angles[ YAW ] );
	new_angles[ PITCH ] = Math::AngleNormalize( new_angles[ PITCH ] );

	cur_angles[ YAW ] = Math::AngleNormalize( D3DXToDegree( m_pPlayerClient->Yaw ) );
	cur_angles[ PITCH ] = Math::AngleNormalize( D3DXToDegree( m_pPlayerClient->Pitch ) );

	if ( new_angles[ YAW ] >= ( cur_angles[ YAW ] - angle ) && new_angles[ YAW ] <= ( cur_angles[ YAW ] + angle ) && new_angles[ PITCH ] >= ( cur_angles[ PITCH ] - angle ) && new_angles[ PITCH ] <= ( cur_angles[ PITCH ] + angle ) )
		return TRUE;*/
	return FALSE;
}

float CAimbot::GetFOV( const D3DXVECTOR3& viewangles, const D3DXVECTOR3& vTargetPos, const D3DXVECTOR3& vCameraPos )
{
	D3DXVECTOR3 vAimAngles{ };
	Math::VectorAngles( vTargetPos, vCameraPos, vAimAngles );

	D3DXVECTOR3 vTempAngles = vAimAngles - viewangles;

	//	Math::VectorNormalize( vTempAngles );
	Math::AngleNormalize( vTempAngles );

	return sqrt( ( vTempAngles.x * vTempAngles.x ) + ( vTempAngles.y * vTempAngles.y ) + ( vTempAngles.z * vTempAngles.z ) );
}

bool CAimbot::GetClosestBone( HOBJECT obj, D3DXVECTOR3* BoneOut )
{
	float BestDist = 360.f;
	bool Result = false;

	uint32_t node = -1;
	while ( g_pLTModel->GetNextNode( obj, node, node ) == 0 )
	{
		CTransform trans{ };
		if ( g_pLTModel->GetNodeTransform( obj, node, &trans, true ) != 0 )
			continue;

		if ( node == 6 )
			trans.Pos.y += 14.f;

		//if (!engine.IsVisible(obj, trans.Pos))
		//	continue;

		D3DXVECTOR3 ViewAngles
		{
			RAD2DEG( g_pLTClientShell->PlayerClient->GetPitch() ),
			RAD2DEG( g_pLTClientShell->PlayerClient->GetYaw() ),
			0.0f
		};

		auto thisdist = GetFOV( ViewAngles, trans.Pos, g_pLTClientShell->Camera->CameraPos );
		if ( BestDist > thisdist )
		{
			BestDist = thisdist;
			*BoneOut = trans.Pos;
			Result = true;
		}
	}
	return Result;
}

int CAimbot::GetBestTarget( D3DXVECTOR3* vPosOut )
{
	float lowestDist = FLT_MAX;
	int iBest = INVALID_AIM_TARGET;
	float bestFov = config.Aimbot[ m_iCurWeaponType ].FieldOfView;

	for ( int i = 0; i < MAX_PLAYERS_IN_ROOM; ++i )
	{
		auto pEnemy = g_pLTClientShell->GetPlayerByID( i );

		if ( !pEnemy )
			continue;

		if ( !pEnemy->IsValidClient() )
			continue;

		if ( m_pLocalPlayer->bClientID == pEnemy->bClientID )
			continue;

		if ( !config.Aimbot[ m_iCurWeaponType ].TeamKill )
		{
			if ( engine.IsSameTeam( m_pLocalPlayer, pEnemy ) )
				continue;
		}

		if ( pEnemy->CharacterFX->IsSpawnProtected() )
			continue;

		D3DXVECTOR3 Pos{ };
		if ( config.Aimbot[ m_iCurWeaponType ].BoneScan && !GetClosestBone( pEnemy->Object, &Pos ) )
			continue;

		if ( !config.Aimbot[ m_iCurWeaponType ].BoneScan )
		{
			switch ( config.Aimbot[ m_iCurWeaponType ].Hitbox )
			{
			default:
				if ( !engine.GetBonePosition( pEnemy->Object, XS( "M-bone Pelvis" ), &Pos ) )
					return iBest;

				break;
			case 1:
				if ( !engine.GetBonePosition( pEnemy->Object, XS( "M-bone Spine" ), &Pos ) )
					return iBest;

				break;
			case 2:
				if ( !engine.GetBonePosition( pEnemy->Object, XS( "M-bone Neck" ), &Pos ) )
					return iBest;

				break;
			case 3:
				if ( !engine.GetBonePosition( pEnemy->Object, XS( "M-bone Head" ), &Pos ) )
					return iBest;

				Pos.y += 14.f;
				break;
			}
		}

		D3DXVECTOR3 ViewAngles
		{
			RAD2DEG( m_pPlayerClient->GetPitch() ),
			RAD2DEG( m_pPlayerClient->GetYaw() ),
			0.0f
		};

		if ( config.Aimbot[ m_iCurWeaponType ].FOVType <= 0 )
		{
			auto FOV = GetFOV( ViewAngles, Pos, g_pLTClientShell->Camera->CameraPos );

			if ( FOV > bestFov )
				continue;

			*vPosOut = Pos;
			iBest = i;
			bestFov = FOV;
		}
		else
		{
			auto FOV = GetFOV( ViewAngles, Pos, g_pLTClientShell->Camera->CameraPos );
			if ( FOV > bestFov )
				continue;

			auto Distance = Math::GetDistance( g_pLTClientShell->Camera->CameraPos, Pos );
			if ( Distance > lowestDist )
				continue;

			*vPosOut = Pos;
			iBest = i;
			bestFov = FOV;
			lowestDist = Distance;
		}
	}

	return iBest;
}

void CAimbot::Update()
{
	if ( !g_pLTClientShell )
		return;

	if ( g_pLTClientShell->bInGame <= 0 )
		return;

	if ( !g_pLTClientShell->PlayerClient )
		return;

	m_pPlayerClient = g_pLTClientShell->PlayerClient;

	if ( !g_pLTClientShell->Camera )
		return;

	m_pLocalPlayer = g_pLTClientShell->GetLocalPlayer();

	if ( !m_pLocalPlayer )
		return;

	if ( !m_pLocalPlayer->IsValidClient() )
		return;

	//if (m_pLocalPlayer->CharacterFX->IsMutant())
	//	return;

	if ( m_pPlayerClient->GunCurrentAmmo <= 0 )
		return;

	if ( m_pPlayerClient->GetFireState() == 6 )
		return;

	m_pCurrentWeapon = m_pLocalPlayer->CharacterFX->CurrentWeapon;
	if ( !m_pCurrentWeapon )
		return;

	m_iCurWeaponType = m_pCurrentWeapon->GetWeaponClass();
	if ( m_iCurWeaponType == Knife || m_iCurWeaponType == C4 || m_iCurWeaponType == Grenades )
		return;

	if ( !config.Aimbot[ m_iCurWeaponType ].Enabled )
		return;

	if ( config.Aimbot[ m_iCurWeaponType ].FOVType > 1 )
		config.Aimbot[ m_iCurWeaponType ].FOVType = 1;

	if ( config.Aimbot[ m_iCurWeaponType ].SilentAim )
		return;

	D3DXVECTOR3 vPos{ };
	m_iCurrentTarget = GetBestTarget( &vPos );

	if ( m_iCurrentTarget != INVALID_AIM_TARGET )
	{
		auto Target = g_pLTClientShell->GetPlayerByID( m_iCurrentTarget );

		if ( Target )
		{
			if ( config.Aimbot[ m_iCurWeaponType ].VisibleCheck && !engine.IsVisible( Target->Object, vPos ) )
				return;

			m_CurAimedBone = vPos;

			D3DXVECTOR3 Angles{ };
			Math::VectorAngles( vPos, g_pLTClientShell->Camera->CameraPos, Angles );

			D3DXVECTOR3 viewangles = {
				RAD2DEG( m_pPlayerClient->GetPitch() ),
				RAD2DEG( m_pPlayerClient->GetYaw() ),
				0.0f };

			float fSmooth = config.Aimbot[ m_iCurWeaponType ].Smoothness;

			if ( fSmooth < 10.f )
				fSmooth = 10.f;

			fSmooth = fSmooth / 10.f;

			Math::SmoothAngles( viewangles, Angles, Angles, fSmooth );
			Math::AngleNormalize( Angles );

			bool key_is_down = InputSys::ins().IsKeyDown( config.Aimbot[ m_iCurWeaponType ].Key );

			if ( config.Aimbot[ m_iCurWeaponType ].AutoAim )
			{
				m_pPlayerClient->SetLocalAngles( Angles );
			}
			else if ( !config.Aimbot[ m_iCurWeaponType ].AutoAim && key_is_down )
			{
				m_pPlayerClient->SetLocalAngles( Angles );
			}
		}
	}
}

void CAimbot::HandleTrigger( CIntersectQuery* query, CIntersectInfo* info )
{
	const auto original = oIntersectSegment( *query, info );

	if ( config.Aimbot[ m_iCurWeaponType ].TriggerEnabled && original )
		trigger.Run( m_pLocalPlayer, info, m_pPlayerClient, config.Aimbot[ m_iCurWeaponType ].TriggerAutoShoot, config.Aimbot[ m_iCurWeaponType ].TriggerKey, config.Aimbot[ m_iCurWeaponType ].TriggerDelay );
}

bool CAimbot::HandleIntersect( CIntersectQuery* query, CIntersectInfo* info )
{
	const auto original = oIntersectSegment( *query, info );

	if ( !config.Aimbot[ m_iCurWeaponType ].Enabled )
		return original;

	if ( !m_pLocalPlayer )
		return original;

	if ( !m_pLocalPlayer->IsValidClient() )
		return original;

	if ( config.Aimbot[ m_iCurWeaponType ].ZM && m_bValidZombie )
	{
		bool key_is_down = InputSys::ins().IsKeyDown( config.Aimbot[ m_iCurWeaponType ].Key );
		if ( key_is_down )
		{
			query->m_To = m_CurAimedBone;
			return oIntersectSegment( *query, info );
		}
	}

	if ( m_pLocalPlayer->CharacterFX->IsMutant() )
		return original;

	if ( !config.Aimbot[ m_iCurWeaponType ].SilentAim )
		return original;

	if ( m_pPlayerClient->GunCurrentAmmo <= 0 )
		return original;

	if ( m_pPlayerClient->GetFireState() == 6 )
		return original;

	D3DXVECTOR3 Pos{ };
	m_iCurrentTarget = GetBestTarget( &Pos );

	if ( m_iCurrentTarget != INVALID_AIM_TARGET )
	{
		auto Target = g_pLTClientShell->GetPlayerByID( m_iCurrentTarget );

		if ( Target )
		{
			bool IsVis = engine.IsVisible( Target->Object, Pos );

			m_CurAimedBone = Pos;

			bool key_is_down = InputSys::ins().IsKeyDown( config.Aimbot[ m_iCurWeaponType ].Key );

			if ( !key_is_down )
				return original;

			//Ignorar Visiveis.
			if ( config.Aimbot[ m_iCurWeaponType ].VisibleCheck )
			{
				query->m_To = Pos;
				return oIntersectSegment( *query, info );
			}
			else if ( !config.Aimbot[ m_iCurWeaponType ].VisibleCheck && IsVis )
			{
				query->m_To = Pos;
				return oIntersectSegment( *query, info );
			}
		}
	}
	return original;
}

int CAimbot::FindBestTarget( D3DXVECTOR3* vPosOut, bool PrioritizeHealth )
{
	float lowestDist = 999999.f;
	int bestHp = 999999;
	float bestFov = config.Aimbot[ m_iCurWeaponType ].FieldOfView;
	int iBest = INVALID_AIM_TARGET;

	*vPosOut = D3DXVECTOR3{ 0.0f, 0.0f, 0.0f };

	const auto entities = AIObjects.GetObjectList();
	for ( size_t i = 0; i < entities.size(); i++ )
	{
		if ( IsBadReadPtr( &entities[ i ], sizeof( PVOID ) ) )
			continue;

		if ( !entities[ i ].bIsValid )
			continue;

		if ( !entities[ i ].pEntity )
			continue;

		if ( IsBadReadPtr( entities[ i ].pEntity, sizeof( PVOID ) ) )
			continue;

		CCharacterFX* enemy = entities[ i ].pEntity;

		if ( !enemy->Object )
			continue;

		if ( enemy->IsDead() || enemy->ZMHealth() <= 0 )
			continue;

		uint32_t Flags = 0;
		if ( g_pLTClient->GetLTCommon()->GetObjectFlags( enemy->Object, OFT_Flags, Flags ) != LT_OK )
			continue;

		if ( !( Flags & FLAG_VISIBLE ) )
			continue;

		D3DXVECTOR3 Pos{ };
		if ( config.Aimbot[ m_iCurWeaponType ].BoneScan && !GetClosestBone( enemy->Object, &Pos ) )
			continue;

		if ( !config.Aimbot[ m_iCurWeaponType ].BoneScan )
		{
			switch ( config.Aimbot[ m_iCurWeaponType ].Hitbox )
			{
			default:
				if ( !engine.GetBonePosition( enemy->Object, XS( "M-bone Pelvis" ), &Pos ) )
					continue;

				break;
			case 1:
				if ( !engine.GetBonePosition( enemy->Object, XS( "M-bone Spine" ), &Pos ) )
					continue;

				break;
			case 2:
				if ( !engine.GetBonePosition( enemy->Object, XS( "M-bone Neck" ), &Pos ) )
					continue;

				break;
			case 3:
				if ( !engine.GetBonePosition( enemy->Object, XS( "M-bone Head" ), &Pos ) )
					continue;

				//	Pos.y += 14.f;
				break;
			}
		}

		D3DXVECTOR3 ViewAngles
		{
			RAD2DEG( m_pPlayerClient->GetPitch() ),
			RAD2DEG( m_pPlayerClient->GetYaw() ),
			0.0f
		};

		if ( PrioritizeHealth && enemy->ZMHealth() > bestHp )
			continue;

		/*D3DXVECTOR3 MyPos{ }, EnemyPos{ };
		g_pLTClient->GetObjectPos( m_pLocalPlayer->Object, &MyPos );
		g_pLTClient->GetObjectPos( enemy->Object, &EnemyPos );

		auto Distance = Math::GetDistance( MyPos, EnemyPos );
		if ( Distance < lowestDist )
		{
			bestHp = enemy->ZMHealth();
			*vPosOut = Pos;
			lowestDist = Distance;
			iBest = i;
		}*/

		if (config.Aimbot[m_iCurWeaponType].FOVType == 0)
		{
			auto FOV = GetFOV(ViewAngles, Pos, g_pLTClientShell->Camera->CameraPos);
			if ( FOV < bestFov )
			{
				*vPosOut = Pos;
				bestFov = FOV;
				iBest = i;
				bestHp = enemy->ZMHealth();
			}
		}
		else
		{
			auto FOV = GetFOV(ViewAngles, Pos, g_pLTClientShell->Camera->CameraPos);
			if ( FOV < bestFov )
			{
				auto Distance = Math::GetDistance( g_pLTClientShell->Camera->CameraPos, Pos );
				if ( Distance < lowestDist )
				{
					*vPosOut = Pos;
					bestFov = FOV;
					lowestDist = Distance;
					iBest = i;
					bestHp = enemy->ZMHealth();
				}
			}
		}
	}
	return iBest;
};

void CAimbot::UpdateZombies()
{
	if ( !g_pLTClientShell )
		return;

	if ( g_pLTClientShell->bInGame <= 0 )
		return;

	if ( !g_pLTClientShell->PlayerClient )
		return;

	if ( !config.Aimbot[ m_iCurWeaponType ].Enabled )
		return;

	if ( !config.Aimbot[ m_iCurWeaponType ].ZM )
		return;

	D3DXVECTOR3 vPos{ };

	const auto iBestTarget = aimbot.FindBestTarget( &vPos, config.Aimbot[ m_iCurWeaponType ].PrioritizeHealth );
	if ( iBestTarget != INVALID_AIM_TARGET )
	{
		m_bValidZombie = engine.IsVisibleNoMask( vPos );
		m_CurAimedBone = vPos;
	}
}