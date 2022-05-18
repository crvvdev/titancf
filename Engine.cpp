#include "Includes.hpp"

CEngine engine;

bool __cdecl CEngine::TraceFilter( HOBJECT Object, void* Data )
{
	UNREFERENCED_PARAMETER( Data );

	if ( !g_pLTClient )
		return false;

	CPlayer* pLocalPlayer = g_pLTClientShell->GetLocalPlayer();

	if ( !pLocalPlayer )
		return false;

	if ( Object == pLocalPlayer->Object )
		return false;

	CLTCommon* pCommon = g_pLTClient->GetLTCommon();
	if ( !pCommon )
		return false;

	uint32_t Flags = 0;
	if ( pCommon->GetObjectFlags( Object, OFT_Flags, Flags ) != LT_OK )
		return false;

	if ( !( Flags & FLAG_VISIBLE ) || !( Flags & FLAG_RAYHIT ) )
		return false;

	return true;
}

bool CEngine::IsBodyVisible( HOBJECT Obj )
{
	D3DXVECTOR3 vPos{ };
	if ( !GetBonePosition( Obj, XS( "M-bone Pelvis" ), &vPos ) )
		return false;

	return IsVisible( Obj, vPos );
}

bool CEngine::IsHeadVisible( HOBJECT Obj )
{
	D3DXVECTOR3 vPos{ };
	if ( !GetBonePosition( Obj, XS( "M-bone Head" ), &vPos ) )
		return false;

	return IsVisible( Obj, vPos );
}

inline bool ASM_Intersect( unsigned long TraceFunction, CIntersectQuery* Query, CIntersectInfo* Result )
{
	_asm push	Result
	_asm push	Query
	_asm mov	eax, TraceFunction
	_asm call	eax
	_asm add	esp, 0x8
}

bool IsVisible( HOBJECT hObject, D3DXVECTOR3 Start, D3DXVECTOR3 End )
{
	if ( !hObject )
		return false;

	if ( !g_pLTClient )
		return false;

	if ( !g_pLTClientShell )
		return false;

	if ( !g_pLTClientShell->Camera )
		return false;

	CIntersectQuery	iQuery;
	CIntersectInfo	iInfo;

	iQuery.m_From = Start;
	iQuery.m_To = End;
	iQuery.m_Flags = INTERSECT_HPOLY | INTERSECT_OBJECTS | IGNORE_NONSOLID;
	iQuery.m_FilterActualIntersectFn = CEngine::TraceFilter;
	iQuery.m_pActualIntersectUserData = &iQuery;

	if ( ASM_Intersect( ( DWORD )g_pLTClient->IntersectSegment, &iQuery, &iInfo ) )
	{
		if ( iInfo.m_hObject == hObject )
		{
			return true;
		}
	}

	return false;
}

bool CEngine::IsVisible( HOBJECT Obj, D3DXVECTOR3& vTo )
{
	return ::IsVisible( Obj, g_pLTClientShell->Camera->CameraPos, vTo );
	//if (!g_pLTClient)
	//	return false;

	//if (!g_pLTClientShell)
	//	return false;

	//if (!g_pLTClientShell->Camera)
	//	return false;

	//CIntersectQuery iQuery;
	//CIntersectInfo iInfo;

	//iQuery.m_From = g_pLTClientShell->Camera->CameraPos;
	//iQuery.m_To = vTo;
	//iQuery.m_Flags = INTERSECT_HPOLY | INTERSECT_OBJECTS | IGNORE_NONSOLID;
	//iQuery.m_FilterActualIntersectFn = TraceFilter;
	//iQuery.m_pActualIntersectUserData = &iQuery;

	//return !g_pLTClient->IntersectSegment(iQuery, &iInfo);// || iInfo.m_hObject == Obj;
}

bool CEngine::IsVisibleNoMask( D3DXVECTOR3& vTo )
{
	if ( !g_pLTClient )
		return false;

	if ( !g_pLTClientShell )
		return false;

	if ( !g_pLTClientShell->Camera )
		return false;

	CIntersectQuery iQuery;
	CIntersectInfo iInfo;

	iQuery.m_From = g_pLTClientShell->Camera->CameraPos;
	iQuery.m_To = vTo;

	return !g_pLTClient->IntersectSegment( iQuery, &iInfo );
}

bool CEngine::IsEntireVisible( HOBJECT Obj )
{
	if ( !g_pLTClient )
		return false;

	if ( !g_pLTClientShell )
		return false;

	if ( !g_pLTClientShell->Camera )
		return false;

	CIntersectQuery iQuery;
	CIntersectInfo iInfo;
	CTransform trans{ };

	HMODELNODE node = INVALID_MODEL_NODE;
	while ( g_pLTModel->GetNextNode( Obj, node, node ) == LT_OK )
	{
		if ( g_pLTModel->GetNodeTransform( Obj, node, &trans, true ) != LT_OK )
			continue;

		if ( ::IsVisible( Obj, g_pLTClientShell->Camera->CameraPos, trans.Pos ) )
			return true;
	}
	return false;
}
/*
bool CEngine::GetFirstVisBone(HOBJECT Obj, D3DXVECTOR3& vOut)
{
	if (!g_pLTClient)
		return false;

	if (!g_pLTClientShell)
		return false;

	if (!g_pLTClientShell->Camera)
		return false;

	CIntersectQuery iQuery;
	CIntersectInfo iInfo;
	CTransform trans{ };

	HMODELNODE node = INVALID_MODEL_NODE;
	while (g_pLTModel->GetNextNode(Obj, node, node) == LT_OK)
	{
		if (g_pLTModel->GetNodeTransform(Obj, node, &trans, true) != LT_OK)
			continue;

		if (::IsVisible(Obj, g_pLTClientShell->Camera->CameraPos, trans.Pos) )//!g_pLTClient->IntersectSegment(iQuery, &iInfo)) //|| iInfo.m_hObject == Obj)
		{
			vOut = trans.Pos;
			return true;
		}
	}
	return false;
}*/

void CEngine::RunConsoleCmd( const CHAR* sVal )
{
	if ( !g_pLTClient )
		return;

	g_pLTClient->RunConsoleCommand( sVal );
}

bool __cdecl CEngine::GetBonePosition( HOBJECT Obj, const char* szBoneName, D3DXVECTOR3* vPos )
{
	if ( !g_pLTModel )
		return false;

	CTransform transform{ };

	HMODELNODE hBoneNode;
	if ( g_pLTModel->GetNode( Obj, szBoneName, hBoneNode ) != LT_OK )
		return false;

	if ( g_pLTModel->GetNodeTransform( Obj, hBoneNode, &transform, true ) != LT_OK )
		return false;

	*vPos = transform.Pos;
	return true;
}

std::int16_t CEngine::GetCurrentWeaponID()
{
	//	DWORD dwLTClientShell = *(DWORD *)(Globals::cshell_dll + 0x166FCE0);
	std::int16_t iCurrentID = 0;

	__asm
	{
		mov     edx, g_pLTClientShell
		mov     ecx, [ edx + 0x78 ]
		mov     eax, [ ecx ]
		mov     edx, [ eax + 0x84 ]
		call    edx
		mov     edx, [ eax ]
		mov     ecx, eax
		mov     eax, [ edx + 0x78 ]
		call    eax
		mov     iCurrentID, ax
	}
	return iCurrentID;
}

CModelNode* CEngine::GetNodeByIndex( int i )
{
	if ( Globals::Address::FnGetNodeByID != NULL )
	{
		auto fnGetNodeByID = reinterpret_cast< CModelNode * ( __cdecl* )( int ) >( Globals::Address::FnGetNodeByID );
		return fnGetNodeByID( i );
	}

	return nullptr;
}

CWeapon* CEngine::GetWeaponByIndex( int i )
{
	if ( Globals::Address::FnGetWeaponByID != NULL )
		return reinterpret_cast< CWeapon * ( __cdecl* )( int ) >( Globals::Address::FnGetWeaponByID )( i );

	/*static DWORD dwWeaponPool = NULL;

	if ( dwWeaponPool == NULL )
		dwWeaponPool = *( DWORD* )( Tools::FindPattern< DWORD >( HASH_CSHELL_DLL, ( PBYTE )"\x8B\x0D\x00\x00\x00\x00\x83\xC4\x0C\x8B\x95\x00\x00\x00\x00", XS("xx????xxxxx????" ) + 0x2 ) );

	if ( dwWeaponPool != NULL )
	{
		return *( CWeapon** )( dwWeaponPool + ( 4 * i ) );
	}*/

	return nullptr;
}

CBasicPlayerInfo* CEngine::GetPlayerInfo( int i )
{
	if ( Globals::Address::FnGetBPIByID != NULL )
		return reinterpret_cast< CBasicPlayerInfo * ( __cdecl* )( int ) >( Globals::Address::FnGetBPIByID )( i );

	return nullptr;
}

CTestValues* CEngine::GetTestValues()
{
	static DWORD GetTest = NULL;

	if ( GetTest == NULL )
		GetTest = *( DWORD* )( Tools::FindPattern< DWORD >( HASH_CSHELL_DLL, PBYTE( "\xA1\x00\x00\x00\x00\x83\xC4\x04\xD9\x18\x8B\x45\xD0" ), XS( "x????xxxxxxxx" ) ) + 0x1 );

	return reinterpret_cast< CTestValues* >( GetTest );
}

bool CEngine::GetScreenSizes( int* w, int* h )
{
	D3DVIEWPORT9 vp{ };
	if ( g_pD3DDevice != nullptr && SUCCEEDED( g_pD3DDevice->GetViewport( &vp ) ) )
	{
		*w = static_cast< int >( vp.Width );
		*h = static_cast< int >( vp.Height );
		return true;
	}
	return false;
}

bool CEngine::ChatSendMessage( const char* msg )
{
	static DWORD dwECX = NULL;
	if ( dwECX == NULL )
	{
		dwECX = Tools::FindPattern< DWORD >( HASH_CSHELL_DLL, ( PBYTE )"\xB9\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x8B\xC7\x2D\x00\x00\x00\x00\x0F\x84\x00\x00\x00\x00\x83\xE8\x02", XS( "x????x????xxx????xx????xxx" ) );
		if ( dwECX ) dwECX = *( DWORD* )( dwECX + 0x1 );
	}

	static DWORD dwCALL1 = NULL;
	if ( dwCALL1 == NULL )
	{
		dwCALL1 = Tools::FindPattern< DWORD >( HASH_CSHELL_DLL, ( PBYTE )"\xE8\x00\x00\x00\x00\x8A\x4F\x40", XS( "x????xxx" ) );
		dwCALL1 = Tools::ResolveRelative( dwCALL1 );
	}

	static DWORD dwCALL2 = NULL;
	if ( dwCALL2 == NULL )
	{
		dwCALL2 = Tools::FindPattern< DWORD >( HASH_CSHELL_DLL, ( PBYTE )"\xE8\x00\x00\x00\x00\xE9\x00\x00\x00\x00\x3B\xF3", XS( "x????x????xx" ) );
		dwCALL2 = Tools::ResolveRelative( dwCALL2 );
	}

	if ( dwECX != NULL && dwCALL1 != NULL && dwCALL2 != NULL )
	{
		DWORD dwEAX = 0;
		__asm
		{
			push    0x401
			mov     ecx, dwECX
			call[ dwCALL1 ]
			mov     ecx, eax
			add     ecx, 8
			mov     eax, msg
			push	eax
			call[ dwCALL2 ]
			mov     dwEAX, eax
		}
		return ( dwEAX == 11 );
	}
	return false;
}

CRoomInfo* CEngine::GetRoomInfo()
{
	static DWORD dwRoomInfo = NULL;
	if ( dwRoomInfo == NULL )
	{
		dwRoomInfo = Tools::FindPattern< DWORD >( HASH_CSHELL_DLL, ( PBYTE )"\xE8\x00\x00\x00\x00\x8B\x48\x24", XS( "x????xxx" ) );
		if ( dwRoomInfo )	dwRoomInfo = Tools::ResolveRelative( dwRoomInfo );
	}

	if ( dwRoomInfo != NULL )
	{
		auto fnGetRoomManager = reinterpret_cast< CRoomManager * ( * )( void ) >( dwRoomInfo );

		CRoomManager* room = fnGetRoomManager();

		if ( room )
			return room->RoomInfo;
	}
	return nullptr;
}

void CEngine::UseCursor( bool bUseCursor )
{
	return;

	static float flCursorCenter = 0.0f;
	g_pLTClient->GetSConValueFloat( XS( "CursorCenter" ), &flCursorCenter );

	if ( bUseCursor && flCursorCenter > 0.f )
	{
		g_pLTClient->RunConsoleCommand( XS( "CursorCenter 0" ) );
		g_pLTClient->GetLTCursor()->SetCursorMode( CM_Hardware );
	}
	else if ( !bUseCursor && flCursorCenter < 1.f )
	{
		g_pLTClient->RunConsoleCommand( XS( "CursorCenter 1" ) );
		g_pLTClient->GetLTCursor()->SetCursorMode( CM_None );
	}
}

bool CEngine::IsGDMRoom()
{
	auto Info = GetRoomInfo();

	if ( !Info )
		return false;

	return ( Info->GameMode == GDM );
}

bool CEngine::IsMutantRoom()
{
	auto Info = GetRoomInfo();

	if ( !Info )
		return false;

	return ( Info->GameMode == HeroModeX || Info->GameMode == ZombieKnightMode || Info->GameMode == ZombieVsGhost || Info->GameMode == ZombieMode || Info->GameMode == HeroMode || Info->GameMode == MutantChallenge );
}

bool CEngine::IsZMRoom()
{
	auto Info = GetRoomInfo();

	if ( !Info )
		return false;

	return ( Info->GameMode == ZA || Info->GameMode == ZA2 || Info->GameMode == ZA3 );
}

bool CEngine::IsInGame()
{
	if ( !g_pLTClient )
		return false;

	if ( !g_pLTClientShell )
		return false;

	if ( g_pLTClientShell->bInGame <= 0 )
		return false;

	if ( !g_pLTClientShell->PlayerClient )
		return false;

	return true;
}

bool CEngine::IsSameTeam( CPlayer* you, CPlayer* enemy )
{
	bool same_team = ( you->bTeam == enemy->bTeam );

	if ( IsMutantRoom() )
	{
		if ( enemy->CharacterFX->IsMutant() )
			same_team = false;
		else
			same_team = true;

		if ( you->CharacterFX->IsMutant() )
		{
			same_team = false;

			if ( enemy->CharacterFX->IsMutant() )
				same_team = true;
		}
	}

	return same_team;
}

bool CEngine::IsPlayerMutant( CPlayer* other )
{
	if ( !IsMutantRoom() )
		return false;

	return other->CharacterFX->IsMutant();
}

std::uint8_t &CEngine::GetS2SCounter()
{
	return *( uint8* )( Globals::Module::CShell + 0x1ADD014 );
}