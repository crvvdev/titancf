#include "Includes.hpp"

CESP esp;

bool CESP::calculate_dynamic_box( LTObject* entity, box_data& box )
{
	if ( !entity )
		return false;

	auto min = entity->GetMins();
	auto max = entity->GetMaxes();

	D3DXVECTOR3 points[] =
	{
		D3DXVECTOR3( min.x, min.y, min.z ),
		D3DXVECTOR3( min.x, max.y, min.z ),
		D3DXVECTOR3( max.x, max.y, min.z ),
		D3DXVECTOR3( max.x, min.y, min.z ),
		D3DXVECTOR3( max.x, max.y, max.z ),
		D3DXVECTOR3( min.x, max.y, max.z ),
		D3DXVECTOR3( min.x, min.y, max.z ),
		D3DXVECTOR3( max.x, min.y, max.z )
	};

	D3DXVECTOR3 flb, brt, blb, frt, frb, brb, blt, flt;

	if ( Tools::WorldToScreen( points[ 3 ], &flb ) &&
		Tools::WorldToScreen( points[ 5 ], &brt ) &&
		Tools::WorldToScreen( points[ 0 ], &blb ) &&
		Tools::WorldToScreen( points[ 4 ], &frt ) &&
		Tools::WorldToScreen( points[ 2 ], &frb ) &&
		Tools::WorldToScreen( points[ 1 ], &brb ) &&
		Tools::WorldToScreen( points[ 6 ], &blt ) &&
		Tools::WorldToScreen( points[ 7 ], &flt ) )
	{
		D3DXVECTOR3 arr[] = { flb, brt, blb, frt, frb, brb, blt, flt };

		float left = flb.x;        // left
		float top = flb.y;        // top
		float right = flb.x;    // right
		float bottom = flb.y;    // bottom

		for ( int i = 1; i < 8; i++ )
		{
			if ( left > arr[ i ].x )
				left = arr[ i ].x;
			if ( bottom < arr[ i ].y )
				bottom = arr[ i ].y;
			if ( right < arr[ i ].x )
				right = arr[ i ].x;
			if ( top > arr[ i ].y )
				top = arr[ i ].y;
		}

		box.x = static_cast< int >( left );
		box.y = static_cast< int >( top );
		box.w = static_cast< int >( right - left );
		box.h = static_cast< int >( bottom - top );

		box.centerX = static_cast< int >( box.x + ( box.w / 2 ) );
		box.centerY = static_cast< int >( box.y + ( box.h / 2 ) );
		return true;
	}
	return false;
};

void SetPlayerGlow( CPlayer* player, float r, float g, float b, const bool state )
{
	const auto c_player_character_fx = ( uintptr_t )player->CharacterFX;
	const auto lt_object = player->Object;

	FnSetOutline(
		player->Object,
		state,
		static_cast< byte >( std::round( r * 255.f ) ),
		static_cast< byte >( std::round( g * 255.f ) ),
		static_cast< byte >( std::round( b * 255.f ) )
		);

	if ( *reinterpret_cast< uintptr_t* >( c_player_character_fx + 0xEF28 ) )
	{
		FnUpdateOutline(
			static_cast< uintptr_t >( c_player_character_fx + 0xBB1C ),
			lt_object
			);
	}

	FnUpdateOutline(
		static_cast< uintptr_t >( c_player_character_fx + 0xD518 ),
		lt_object
		);

	uintptr_t v5 = { 0 };

	auto v6 = static_cast< uintptr_t >( c_player_character_fx + 0x58C );

	do
	{
		if ( v5 >= *reinterpret_cast< uintptr_t* >( c_player_character_fx + 0x588 ) )
			break;
		FnUpdateOutline( v6, lt_object );
		++v5;
		v6 += 6640;
	} while ( v5 < 7 );

	FnUpdateOutline( static_cast< uintptr_t >( c_player_character_fx ) + 0x12A1C, lt_object );
}

void DrawSkeleton( CPlayer* player, DWORD color )
{
	if ( !g_pLTModel )
		return;

	HMODELNODE node = INVALID_MODEL_NODE;
	while ( g_pLTModel->GetNextNode( player->Object, node, node ) == LT_OK )
	{
		unsigned int node_child_count{};
		if ( g_pLTModel->GetNumChildren( player->Object, node, node_child_count ) != LT_OK )
			break;

		for ( unsigned int i = 0; i < node_child_count; ++i )
		{
			HMODELNODE node_child = INVALID_MODEL_NODE;
			if ( g_pLTModel->GetChild( player->Object, node, i, node_child ) != LT_OK )
				continue;

			unsigned int parents{};
			if ( g_pLTModel->GetParent( player->Object, node_child, parents ) != LT_OK )
				continue;

			D3DXVECTOR3 start{};
			CTransform transform{};

			if ( g_pLTModel->GetNodeTransform( player->Object, node_child, &transform, true ) != LT_OK )
				continue;

			D3DXVECTOR3 end{};
			CTransform transform_end{};

			if ( g_pLTModel->GetNodeTransform( player->Object, parents, &transform_end, true ) != LT_OK )
				continue;

			if ( Tools::WorldToScreen( transform.Pos, &start ) && Tools::WorldToScreen( transform_end.Pos, &end ) )
				render.RenderLine( start.x, start.y, end.x, end.y, color, 1.6f );
		}
	}
}

void DrawHealthBar( int health, int maxhealth, box_data& box )
{
	health = std::clamp( health, 0, maxhealth );

	int g = int( health * 2.55f );
	int r = 255 - g;

	int boxSpacing = 3;

	float healthPerc = health / 100.f;

	int barx = box.x;
	int bary = box.y;
	int barh = box.h;

	int barw = 4;
	barx -= barw + boxSpacing;

	render.RenderBox( barx, bary, barx + barw, bary + barh, D3DCOLOR_RGBA( 10, 10, 10, 200 ) );

	if ( healthPerc > 0.0F )
		render.RenderBoxFilled( barx + 1, static_cast< int >( bary + ( barh * ( 1.f - healthPerc ) ) + 1 ), barx + barw - 1, bary + barh - 1, D3DCOLOR_XRGB( r, g, 0 ) );
}

void CESP::Render3DBox( CCharacterFX* enemy, DWORD col )
{
	auto Rotate2D = []( D3DXVECTOR3& vec, const float& f ) -> void
	{
		float _x, _y;

		float s, c;

		Math::SinCos( DEG2RAD( f ), &s, &c );

		_x = vec.x;
		_y = vec.y;

		vec.x = ( _x * c ) - ( _y * s );
		vec.y = ( _x * s ) + ( _y * c );
	};

	int iScreenWidth = 0, iScreenHeight = 0;

	if ( !engine.GetScreenSizes( &iScreenWidth, &iScreenHeight ) )
		return;

	auto min = enemy->Object->GetMins();
	auto max = enemy->Object->GetMaxes();

	D3DXVECTOR3 corners[ 8 ] =
	{
		D3DXVECTOR3( min.x,min.y,min.z ),
		D3DXVECTOR3( min.x,max.y,min.z ),
		D3DXVECTOR3( max.x,max.y,min.z ),
		D3DXVECTOR3( max.x,min.y,min.z ),
		D3DXVECTOR3( min.x,min.y,max.z ),
		D3DXVECTOR3( min.x,max.y,max.z ),
		D3DXVECTOR3( max.x,max.y,max.z ),
		D3DXVECTOR3( max.x,min.y,max.z )
	};

	float ang = enemy->ViewAngles().y;

	for ( int i = 0; i <= 7; i++ )
		Rotate2D( corners[ i ], ang );

	D3DXVECTOR3 origin{ };
	if ( g_pLTClient->GetObjectPos( enemy->Object, &origin ) != LT_OK )
		return;

	D3DXVECTOR3 _corners[ 8 ]{ };
	for ( int i = 0; i <= 7; i++ )
	{
		if ( !Tools::WorldToScreen( corners[ i ], &_corners[ i ] ) )
			return;
	}

	int x1 = iScreenWidth * 2, y1 = iScreenHeight * 2, x2 = -iScreenWidth, y2 = -iScreenHeight;

	for ( int i = 0; i <= 7; i++ )
	{
		x1 = std::min< int >( x1, _corners[ i ].x );
		y1 = std::min< int >( y1, _corners[ i ].y );
		x2 = std::max< int >( x2, _corners[ i ].x );
		y2 = std::max< int >( y2, _corners[ i ].y );
	}

	for ( int i = 1; i <= 4; i++ )
	{
		render.RenderLine( ( int )( _corners[ i - 1 ].x ), ( int )( _corners[ i - 1 ].y ), ( int )( _corners[ i % 4 ].x ), ( int )( _corners[ i % 4 ].y ), col );
		render.RenderLine( ( int )( _corners[ i - 1 ].x ), ( int )( _corners[ i - 1 ].y ), ( int )( _corners[ i + 3 ].x ), ( int )( _corners[ i + 3 ].y ), col );
		render.RenderLine( ( int )( _corners[ i + 3 ].x ), ( int )( _corners[ i + 3 ].y ), ( int )( _corners[ i % 4 + 4 ].x ), ( int )( _corners[ i % 4 + 4 ].y ), col );
	}
}

void CESP::Draw2DRadar( CCharacterFX* enemy )
{
	if ( !config.ESP.Radar )
		return;

	if ( !g_pLTClientShell )
		return;

	if ( !g_pLTClientShell->PlayerClient )
		return;

	int x = 0, y = 0;
	if ( !engine.GetScreenSizes( &x, &y ) )
		return;

	x /= 2;
	y -= 200;

	render.RenderCircle( x, y, 80.f, 35, D3DCOLOR_XRGB( 255, 255, 255 ) );

	CPlayer* pLocal = g_pLTClientShell->GetLocalPlayer();

	if ( !pLocal )
		return;

	D3DXVECTOR3 vLocalPos{ };
	g_pLTClient->GetObjectPos( pLocal->Object, &vLocalPos );

	if ( config.ESP.UseOnZM && engine.IsZMRoom() )
	{
		if ( !enemy->IsDead() )
		{
			D3DXVECTOR3 vEnemyPos{ };
			g_pLTClient->GetObjectPos( enemy->Object, &vEnemyPos );

			const auto fDistance = Math::GetDistance( vLocalPos, vEnemyPos ) / 100;
			if ( fDistance >= 71.0f )
				return;

			D3DXVECTOR3 Angles;

			Math::VectorAngles( vEnemyPos, g_pLTClientShell->Camera->CameraPos, Angles );

			float yaw = g_pLTClientShell->PlayerClient->GetYaw();
			float ang = ( DEG2RAD( Angles[ YAW ] ) - yaw ) + DEG2RAD( 90 );

			float dx = x - cos( ang ) * fDistance;
			float dy = y - sin( ang ) * fDistance;

			render.RenderBoxFilled( dx, dy, 3.f, 3.f, D3DCOLOR_XRGB( 195, 0, 255 ) );
		}
	}
	else
	{
		for ( int i = 0; i < MAX_PLAYERS_IN_ROOM; ++i )
		{
			D3DXVECTOR3 Angles, TheirPos;

			g_pLTClient->GetObjectPos( enemy->Object, &TheirPos );

			Math::VectorAngles( TheirPos, g_pLTClientShell->Camera->CameraPos, Angles );

			float Dist = Math::GetDistance( vLocalPos, TheirPos ) / 100;

			if ( Dist > 71.0f )
				continue;

			float yaw = g_pLTClientShell->PlayerClient->GetYaw();
			float ang = ( DEG2RAD( Angles[ YAW ] ) - yaw ) + DEG2RAD( 90 );

			float dx = x - cos( ang ) * Dist;
			float dy = y - sin( ang ) * Dist;

			render.RenderBoxFilled( dx, dy, 3.f + dx, 3.f + dy, D3DCOLOR_XRGB( 255, 0, 0 ) );
		}
	}
}

void CESP::RenderScene()
{
	if ( !g_pLTClientShell )
		return;

	if ( g_pLTClientShell->bInGame <= 0 )
	{
		FnGlowSetActive( FALSE );
		return;
	}

	auto pLocal = g_pLTClientShell->GetLocalPlayer();

	if ( !pLocal )
		return;

	if ( config.ESP.Names || config.ESP.Boxes > 0 || config.ESP.Health > 0 || config.ESP.Skeleton || config.ESP.Weapons || config.ESP.Glow || config.ESP.ShowC4 )
	{
		if ( config.ESP.UseOnZM && engine.IsZMRoom() )
		{
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

				uint32_t Flags = 0;
				if ( g_pLTClient->GetLTCommon()->GetObjectFlags( enemy->Object, OFT_Flags, Flags ) != LT_OK )
					continue;

				if ( !( Flags & FLAG_VISIBLE ) )
					continue;

			//	this->Draw2DRadar( enemy );

				if ( enemy->IsDead() )
					continue;

				if ( !esp.calculate_dynamic_box( enemy->Object, box ) )
					continue;

				int r = static_cast< int >( config.ESP.EnemyCol[ 0 ] * 255 );
				int g = static_cast< int >( config.ESP.EnemyCol[ 1 ] * 255 );
				int b = static_cast< int >( config.ESP.EnemyCol[ 2 ] * 255 );

				//Boxes 2D
				switch ( config.ESP.Boxes )
				{
				case 1://2D
					render.RenderBox( box.x, box.y, box.x + box.w, box.y + box.h, D3DCOLOR_XRGB( r, g, b ), 1.5f );
					break;
				case 2://3D
					esp.Render3DBox( enemy, D3DCOLOR_XRGB( r, g, b ) );
					break;
				case 3://Edge
					render.RenderCoalBox( box.x, box.y, box.x + box.w, box.y + box.h, D3DCOLOR_XRGB( r, g, b ), 1.5f );
					break;
				default:
					break;
				}

				//Names
				if ( config.ESP.Names )
					render.RenderText( XS( "NPC" ), ImVec2( static_cast< float >( box.centerX ), static_cast< float >( box.y - 13.f ) ), 12.5f, D3DCOLOR_XRGB( r, g, b ), true, g_pESPFont );

				//Healthbar
				if ( config.ESP.Health > 0 )
				{
					char HP[ 64 ];
					sprintf_s( HP, sizeof HP, XS( "Vida: %d" ), enemy->ZMHealth() );
					render.RenderText( HP, ImVec2( static_cast< float >( box.centerX ), static_cast< float >( box.y + box.h ) ), 12.5f, D3DCOLOR_XRGB( 255, 0, 0 ), true, g_pESPFont );
				}
			}
		}
		else
		{
			for ( int i = 0; i < MAX_PLAYERS_IN_ROOM; ++i )
			{
				auto pEnemy = g_pLTClientShell->GetPlayerByID( i );

				if ( !pEnemy )
					continue;

				if ( !pEnemy->IsValidClient() )
					continue;

				if ( pLocal->bClientID == pEnemy->bClientID )
					continue;

			//	this->Draw2DRadar( pEnemy->CharacterFX );

				bool same_team = engine.IsSameTeam( pLocal, pEnemy );
				bool visible = false;

				int r = static_cast< int >( config.ESP.EnemyCol[ 0 ] * 255 );
				int g = static_cast< int >( config.ESP.EnemyCol[ 1 ] * 255 );
				int b = static_cast< int >( config.ESP.EnemyCol[ 2 ] * 255 );

				if ( engine.IsPlayerMutant( pEnemy ) )
					r = 66, g = 135, b = 245;

				if ( engine.IsHeadVisible( pEnemy->Object ) )
				{
					visible = true;
					r = static_cast< int >( config.ESP.EnemyVisCol[ 0 ] * 255 );
					g = static_cast< int >( config.ESP.EnemyVisCol[ 1 ] * 255 );
					b = static_cast< int >( config.ESP.EnemyVisCol[ 2 ] * 255 );
				}

				if ( config.ESP.OnlyVisible && !visible )
					continue;

				if ( pEnemy->CharacterFX->IsSpawnProtected() )
					r = 255, g = 255, b = 255;

				DWORD Color = D3DCOLOR_XRGB( r, g, b );

				static bool _glow = true;

				if ( config.ESP.Glow && _glow )
				{
					//Enable Glow
					_glow = false;

					FnGlowSetActive( TRUE );
				}

				if ( config.ESP.Glow )
				{
					//Glow Thickness
					*( float* )( Globals::Address::dwSetGlowStrenght ) = config.ESP.GlowThickness;

					if ( same_team )
					{
						//dont glow allies
						SetPlayerGlow( pEnemy, 0.0f, 0.0f, 0.0f, false );
						SetPlayerGlow( pLocal, 0.0f, 0.0f, 0.0f, false );
					}
					else
					{
						if ( !visible )
							SetPlayerGlow( pEnemy, config.ESP.GlowEnemyCol[ 0 ], config.ESP.GlowEnemyCol[ 1 ], config.ESP.GlowEnemyCol[ 2 ], true );
						else
							SetPlayerGlow( pEnemy, config.ESP.GlowEnemyVisCol[ 0 ], config.ESP.GlowEnemyVisCol[ 1 ], config.ESP.GlowEnemyVisCol[ 2 ], true );
					}
				}
				else if ( !config.ESP.Glow && !_glow )
				{
					_glow = true;

					SetPlayerGlow( pEnemy, 0.0f, 0.0f, 0.0f, false );
					SetPlayerGlow( pLocal, 0.0f, 0.0f, 0.0f, false );

					FnGlowSetActive( FALSE );
				}

				if ( same_team )
					continue;

				if ( !calculate_dynamic_box( pEnemy->Object, box ) )
					continue;

				if ( config.ESP.Names )
				{
					//Names
					render.RenderText( pEnemy->szName, ImVec2( static_cast< float >( box.centerX ), static_cast< float >( box.y - 13.f ) ), 12.5f, Color, true, g_pESPFont );
				}

				if ( config.ESP.Skeleton )
				{
					//Bones
					DrawSkeleton( pEnemy, Color );
				}

				//2D Boxes
				switch ( config.ESP.Boxes )
				{
				case 1://2D
					render.RenderBox( box.x, box.y, box.x + box.w, box.y + box.h, Color, 1.5f );
					break;
				case 2://3D
					Render3DBox( pEnemy->CharacterFX, D3DCOLOR_XRGB( r, g, b ) );
					break;
				case 3://Edge
					render.RenderCoalBox( box.x, box.y, box.x + box.w, box.y + box.h, Color, 1.5f );
					break;
				default:
					break;
				}

				std::vector< std::string > vec_render{ };

				switch ( config.ESP.Health )
				{
				case 1:
					//Text
					char HP[ 64 ];
					sprintf_s( HP, sizeof HP, XS( "Vida: %d" ), pEnemy->Health );
					vec_render.push_back( HP );
					break;
				case 2:
					//Healthbar
					DrawHealthBar( pEnemy->Health, 100, box );
					break;
				default:
					break;
				}

				if ( config.ESP.Weapons )
				{
					//Weapons
					auto CurWeapon = pEnemy->CharacterFX->CurrentWeapon;

					if ( CurWeapon != NULL )
						vec_render.push_back( CurWeapon->WeaponName );
				}

				if ( config.ESP.ShowC4 && pEnemy->bHasC4 )
				{
					//C4
					render.RenderText( XS( "Tem C4" ), ImVec2( static_cast< float >( box.x + box.w + 2.f ), static_cast< float >( box.y ) ), 12.5f, D3DCOLOR_XRGB( 255, 255, 255 ), false, g_pESPFont );
				}

				for ( int i = 0; i < vec_render.size(); ++i )
					render.RenderText( vec_render[ i ], ImVec2( static_cast< float >( box.centerX ), static_cast< float >( box.y + box.h + 2 ) + ( i * 13.f ) ), 12.5f, D3DCOLOR_XRGB( r, g, b ), true, g_pESPFont );
			}
		}
	}
}