#include "Includes.hpp"

CAIObjectManager AIObjects;

void CAIObjectManager::Update()
{
	if ( !engine.IsZMRoom() )
		return;

	if ( !g_pLTClientShell )
		return;

	if ( g_pLTClientShell->bInGame <= 0 )
		return;

	if ( !g_pLTClientShell->PlayerClient )
		return;

	auto pLocal = g_pLTClientShell->GetLocalPlayer();

	if ( !pLocal )
		return;

	for ( size_t i = 0; i < vAIObjectList.size(); ++i )
	{
		auto it = &vAIObjectList[ i ];

		if ( !it->bIsValid )
			continue;

		if ( it->pEntity == nullptr )
		{
			it->bIsValid = false;
			continue;
		}

		if ( IsBadReadPtr( it->pEntity, sizeof( PVOID ) ) )
		{
			it->bIsValid = false;
			continue;
		}

		if ( !it->pEntity->Object )
		{
			it->bIsValid = false;
			continue;
		}
	}
}

//	if ( !it->IsValid )
//		continue;

//	if ( it->pCharFX->IsDead() )
//		continue;

//	auto r = 255, g = 0, b = 0;
//	bool skip = false;

//	uint32_t Flags = 0;
//	if ( g_pLTClient->GetLTCommon()->GetObjectFlags( it->pCharFX->Object, OFT_Flags, Flags ) == LT_OK )
//	{
//		if ( !( Flags & FLAG_VISIBLE ) )
//			skip = true;
//	}

//	if ( !skip )
//	{
//		if ( config.ESP.UseOnZM && ( config.ESP.Boxes > 0 || config.ESP.Health > 0 || config.ESP.Names ) )
//		{
//			if ( esp.calculate_dynamic_box( it->pCharFX->Object, box ) )
//			{
//				//Boxes 2D
//				switch ( config.ESP.Boxes )
//				{
//				case 1://2D
//					render.RenderBox( box.x, box.y, box.x + box.w, box.y + box.h, D3DCOLOR_XRGB( r, g, b ), 1.5f );
//					break;
//				case 2://3D
//					esp.Render3DBox( it->pCharFX, D3DCOLOR_XRGB( r, g, b ) );
//					break;
//				case 3://Edge
//					render.RenderCoalBox( box.x, box.y, box.x + box.w, box.y + box.h, D3DCOLOR_XRGB( r, g, b ), 1.5f );
//					break;
//				default:
//					break;
//				}

//				//Names
//				if ( config.ESP.Names )
//					render.RenderText( XS( "NPC" ), ImVec2( static_cast< float >( box.centerX ), static_cast< float >( box.y - 13.f ) ), 12.5f, D3DCOLOR_XRGB( r, g, b ), true, g_pESPFont );

//				//Healthbar
//				if ( config.ESP.Health > 0 )
//				{
//					char HP[ 64 ];
//					sprintf_s( HP, sizeof HP, XS( "Vida: %d" ), it->pCharFX->ZMHealth() );
//					render.RenderText( HP, ImVec2( static_cast< float >( box.centerX ), static_cast< float >( box.y + box.h ) ), 12.5f, D3DCOLOR_XRGB( 255, 0, 0 ), true, g_pESPFont );
//				}
//			}
//		}
//	}

//	/*D3DXVECTOR3 Pos_1 = pLocal->pObject->Maxes;

//	Pos_1.y -= 50.0f;
//	Pos_1.x += 50.0f;

//	g_pLTClient->SetObjectPos(fx->iObject, &Pos_1);*/
//}
