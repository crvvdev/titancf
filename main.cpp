#include "Includes.hpp"

PBYTE CShellBackup = 0, CFEXEBackup = 0;

std::vector< CHookContext > hm_active_hooks{ };

void AllHooksRestore()
{
	try
	{
		for ( size_t i = 0; i < hm_active_hooks.size(); ++i )
		{
			hm_active_hooks[ i ].UnHook();
			hm_active_hooks[ i ].DumpFirst5();
		}
	}
	catch ( ... )
	{

	}
}

void AllHooksReHook()
{
	try
	{
		for ( size_t i = 0; i < hm_active_hooks.size(); ++i )
		{
			hm_active_hooks[ i ].ReHook();
			hm_active_hooks[ i ].DumpFirst5();
		}
	}
	catch ( ... )
	{

	}
}

////////////////////////////////////////////////////////
// Hooks
void __stdcall zfxAddObject( CCharacterFX* eax )
{
	AIObjects.GetObjectList().push_back( { eax, true } );
}

void __stdcall ClearObjectList()
{
	AIObjects.GetObjectList().clear();
}

DWORD dwAIObject_ctor = NULL, AIObjectJMP = NULL;

__declspec( naked ) void hkZombieFX()
{
	__asm
	{
		mov byte ptr[ eax ], 0
		mov eax, edi
		pushad
		pushfd
		push eax
		call zfxAddObject
		popfd
		popad
		jmp AIObjectJMP
	}
}

void SetupZombies()
{
	static BYTE original_byte[ 5 ]{ };
	static bool _hooked = false;
//	static PBYTE add = nullptr;

	if ( dwAIObject_ctor == NULL )
	{
		dwAIObject_ctor = Tools::FindPattern< DWORD >( HASH_CSHELL_DLL, ( PBYTE )"\xC6\x00\x00\x8B\xC7\xC7\x87\x00\x00\x00\x00\x00\x00\x00\x00\xC6\x87\x00\x00\x00\x00\x00", XS( "xxxxxxx????????xx?????" ) );
		if ( dwAIObject_ctor )
		{
			AIObjectJMP = dwAIObject_ctor + 5;
			std::memcpy( original_byte, PVOID( dwAIObject_ctor ), sizeof( original_byte ) );

			/*std::uint8_t shellcode[] = { 0xC6, 0x00, 0x00, 0x89, 0xF8, 0x60, 0x50, 0xE8, 0xFB, 0xCC, 0xFF, 0xCC, 0x61, 0xE9, 0xFB, 0xCC, 0xFF, 0xCC };

			add = PBYTE( malloc( sizeof( shellcode ) ) );
			memcpy( add, shellcode, sizeof( shellcode ) );

			*( DWORD* )( add + 8 ) = ( DWORD )( ( DWORD )AddObject - ( DWORD )add ) - 12;
			*( DWORD* )( add + 14 ) = ( DWORD )( AIObjectJMP - ( DWORD )add ) - 18;

#ifdef _DEBUG
			Tools::SaveToLog( "hkZombieFX -> 0x%X\n", add );
			Tools::SaveToLog( "AIObjectJMP -> 0x%X\n", AIObjectJMP );
#endif*/
		}
	}

	if ( !g_pLTClientShell )
		return;

	if ( g_pLTClientShell->bInGame <= 0 && _hooked )
	{
		_hooked = false;
		Tools::memcpy_s( dwAIObject_ctor, original_byte, sizeof( original_byte ) );
		ClearObjectList();
	}
	else if ( *( BYTE* )dwAIObject_ctor != 0xE9 && !_hooked && g_pLTClientShell->bInGame > 0 && engine.IsZMRoom() )
	{
		_hooked = true;
		CreateHook( PBYTE( dwAIObject_ctor ), PBYTE( &hkZombieFX ) );
	}
}

inline void RenderMenuWindow( LPDIRECT3DDEVICE9 thisptr )
{
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	////Render
	render.BeginScene();
	render.RenderScene();
	render.EndScene();

	////Menu
	gui.Render();

	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData( ImGui::GetDrawData() );
}

typedef HRESULT( WINAPI* hPresent )( LPDIRECT3DDEVICE9, const RECT*, const RECT*, HWND, const RGNDATA* );
hPresent oPresent = NULL;

bool bReset = false;

HRESULT WINAPI hkPresent( LPDIRECT3DDEVICE9 thisptr, const RECT* pSourceRect, const RECT* pDestRect, HWND hDestWindowOverride, const RGNDATA* pDirtyRegion )
{
	//#ifdef _DEBUG
	//	Tools::SaveToLog( "Present() - _ReturnAddress() = 0x%p\n", _ReturnAddress() );
	//#endif
	return oPresent( thisptr, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion );
}

typedef HRESULT( WINAPI* hReset )( LPDIRECT3DDEVICE9, D3DPRESENT_PARAMETERS* );
hReset oReset = NULL;

HRESULT WINAPI hkReset( LPDIRECT3DDEVICE9 pDevice, D3DPRESENT_PARAMETERS* pD3Dpp )
{
	const auto hResult = oReset( pDevice, pD3Dpp );

	//if ( SUCCEEDED( hResult ) )
	//{
	//	if ( !m_pStateBlockDraw )
	//		pDevice->CreateStateBlock( D3DSBT_ALL, &m_pStateBlockDraw );
	//}

	return hResult;
}

using DrawIndexedPrimitive_ = HRESULT( WINAPI* )( LPDIRECT3DDEVICE9 pDevice, D3DPRIMITIVETYPE, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount );
DrawIndexedPrimitive_ oDrawIndexedPrimitive = NULL;

void MaterialChams( bool OnlyVis, D3DCOLORVALUE Color1, D3DCOLORVALUE Color2, LPDIRECT3DDEVICE9 pDevice, D3DPRIMITIVETYPE pType, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount )
{
	D3DMATERIAL9	mtrl;

	ZeroMemory( &mtrl, sizeof( mtrl ) );

	mtrl.Ambient.r = mtrl.Diffuse.r = mtrl.Emissive.r = mtrl.Specular.r = Color2.r;
	mtrl.Ambient.g = mtrl.Diffuse.g = mtrl.Emissive.g = mtrl.Specular.g = Color2.g;
	mtrl.Ambient.b = mtrl.Diffuse.b = mtrl.Emissive.b = mtrl.Specular.b = Color2.b;
	mtrl.Ambient.a = mtrl.Diffuse.a = mtrl.Emissive.a = mtrl.Specular.a = Color2.a;

	mtrl.Power = 1.0f;

	pDevice->SetMaterial( &mtrl );
	pDevice->SetPixelShader( NULL );
	pDevice->SetRenderState( D3DRS_FOGENABLE, false );
	pDevice->SetTexture( 0, NULL );

	pDevice->SetRenderState( D3DRS_AMBIENT, D3DCOLOR_COLORVALUE( Color2.r, Color2.g, Color2.b, Color2.a ) );

	if ( OnlyVis )
		return;

	pDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
	pDevice->SetRenderState( D3DRS_ZFUNC, D3DCMP_NEVER );

	oDrawIndexedPrimitive( pDevice, pType, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount );

	ZeroMemory( &mtrl, sizeof( mtrl ) );

	mtrl.Ambient.r = mtrl.Diffuse.r = mtrl.Emissive.r = mtrl.Specular.r = Color1.r;
	mtrl.Ambient.g = mtrl.Diffuse.g = mtrl.Emissive.g = mtrl.Specular.g = Color1.g;
	mtrl.Ambient.b = mtrl.Diffuse.b = mtrl.Emissive.b = mtrl.Specular.b = Color1.b;
	mtrl.Ambient.a = mtrl.Diffuse.a = mtrl.Emissive.a = mtrl.Specular.a = Color1.a;

	mtrl.Power = 1.0f;

	pDevice->SetMaterial( &mtrl );
	pDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
	pDevice->SetRenderState( D3DRS_ZFUNC, D3DCMP_LESSEQUAL );
	pDevice->SetRenderState( D3DRS_AMBIENT, D3DCOLOR_COLORVALUE( Color1.r, Color1.g, Color1.b, Color1.a ) );
}

HRESULT WINAPI hkDrawIndexedPrimitive( IDirect3DDevice9* pDevice, D3DPRIMITIVETYPE Type, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount )
{
	const auto ret_addr = DWORD( _ReturnAddress() );
	if ( !( ret_addr >= Globals::Module::CrossFire && ret_addr <= Globals::Module::CrossFire + Globals::Module::CrossFireSize ) )
		return oDrawIndexedPrimitive( pDevice, Type, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount );

	if ( config.ESP.Chams.Wallhack || config.ESP.Chams.Chams || config.ESP.Chams.Fullbright || config.ESP.Chams.GhostChams )
	{
		IDirect3DVertexBuffer9* pStreamData;
		UINT pOffset, m_Stride;
		HRESULT hRes = pDevice->GetStreamSource( 0, &pStreamData, &pOffset, &m_Stride );

		if ( SUCCEEDED( hRes ) )
		{
			if ( m_Stride == 44 || m_Stride == 40 || m_Stride == 32 )
			{
				//Wallhack
				if ( config.ESP.Chams.Wallhack )
				{
					pDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
					pDevice->SetRenderState( D3DRS_ZFUNC, D3DCMP_NEVER );
				}

				//Chams
				if ( config.ESP.Chams.Chams )
				{
					D3DCOLORVALUE colorValueFront, colorValueBack;

					colorValueFront.r = config.ESP.Chams.EnemyVisCol[ 0 ];
					colorValueFront.g = config.ESP.Chams.EnemyVisCol[ 1 ];
					colorValueFront.b = config.ESP.Chams.EnemyVisCol[ 2 ];
					colorValueFront.a = 1.0f;

					colorValueBack.r = config.ESP.Chams.EnemyCol[ 0 ];
					colorValueBack.g = config.ESP.Chams.EnemyCol[ 1 ];
					colorValueBack.b = config.ESP.Chams.EnemyCol[ 2 ];
					colorValueBack.a = 1.0f;

					MaterialChams( config.ESP.Chams.OnlyVisible, colorValueFront, colorValueBack, pDevice, Type, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount );
				}

				//Full Bright
				if ( config.ESP.Chams.Fullbright )
				{
					pDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
					pDevice->SetRenderState( D3DRS_AMBIENT, D3DCOLOR_ARGB( 255, 255, 255, 255 ) );
				}

				//Phantom Chams
				if ( config.ESP.Chams.GhostChams )
				{
					pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
					pDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVDESTCOLOR );
					pDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_INVSRCALPHA );
					oDrawIndexedPrimitive( pDevice, Type, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount );
					pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
					pDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVDESTCOLOR );
					pDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_INVSRCALPHA );
				}
			}
		}
	}
	return oDrawIndexedPrimitive( pDevice, Type, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount );
}

__declspec( noinline ) IDirect3DDevice9* GetDevice()
{
	static DWORD deviceGame_ = NULL;

	if ( !deviceGame_ )
		deviceGame_ = *( DWORD* )( Tools::FindPattern< DWORD >( HASH_CROSSFIRE_EXE, PBYTE( "\x8B\x0D\x00\x00\x00\x00\x6A\x08\x50\x6A\x00\x8B\x09\x51\x8B\x11\xFF\x92\x00\x00\x00\x00\x85\xC0" ), XS( "xx????xxxxxxxxxxxx????xx" ) ) + 0x2 );

	if ( deviceGame_ )
	{
		DWORD* dwPointer = ( DWORD* )deviceGame_;

		if ( !dwPointer )
			return NULL;

		if ( !*dwPointer )
			return NULL;

		DWORD* dwPointerTwo = ( DWORD* )*dwPointer;

		if ( !dwPointerTwo )
			return NULL;

		if ( !*dwPointerTwo )
			return NULL;

		return ( IDirect3DDevice9* )*dwPointerTwo;
	}
	return nullptr;
}

static bool CreateDevice( DWORD* v_table )
{
	IDirect3DDevice9* p_device = nullptr;

	while ( !( p_device = GetDevice() ) )
		Sleep( 250 );

	auto* vtable = reinterpret_cast< uintptr_t* >( p_device );
	vtable = reinterpret_cast< uintptr_t* >( vtable[ 0 ] );

	v_table[ 0 ] = vtable[ 16 ]; //Reset
	v_table[ 1 ] = vtable[ 17 ]; //Present
	v_table[ 2 ] = vtable[ 82 ]; //DIP

#ifdef _DEBUG
	Tools::SaveToLog( "GameDevice = 0x%p\n", p_device );
	Tools::SaveToLog( "d3d9.Reset = 0x%X\n", v_table[ 0 ] );
	Tools::SaveToLog( "d3d9.Present = 0x%X\n", v_table[ 1 ] );
	Tools::SaveToLog( "d3d9.DIP = 0x%X\n", v_table[ 2 ] );
#endif

	return true;
}

LTRESULT __cdecl hkRenderCamera( HLOCALOBJ hCamera, float fFrameTime )
{
	AIObjects.Update();
	aimbot.UpdateZombies();
	aimbot.Update();
	memory.Update();

	return oRenderCamera( hCamera, fFrameTime );
}

bool __cdecl hkIntersectSegment( CIntersectQuery* query, CIntersectInfo* info )
{
	DWORD ret_addr = DWORD( _ReturnAddress() );
	if ( ret_addr >= Globals::Module::CShell && ret_addr <= Globals::Module::CShell + Globals::Module::CShellSize )
	{
		aimbot.HandleTrigger( query, info );

		const auto ret = aimbot.HandleIntersect( query, info );
		if ( ret )
			return ret;
	}
	return oIntersectSegment( *query, info );
}

using hFlipScreen = LTRESULT( __cdecl* ) ( uint32_t );
hFlipScreen oFlipScreen = NULL;

DWORD dwDllHookCheck = NULL;

LTRESULT __cdecl hkFlipScreen( uint32_t flags )
{
	auto thisptr = GetDevice();
	if ( thisptr )
	{
		while ( g_pD3DDevice != thisptr )
		{
			if ( gui.IsReady() )
			{
				ImGui_ImplDX9_InvalidateDeviceObjects();
				ImGui_ImplDX9_Init( thisptr );
				ImGui_ImplDX9_CreateDeviceObjects();
				gui.UpdateDevice( thisptr );
			}
			g_pD3DDevice = thisptr;
		}

		static bool _was_inited = false;

		if ( !_was_inited )
		{
			_was_inited = true;

			InputSys::ins().Initialize( thisptr );

			InputSys::ins().RegisterHotkey( VK_INSERT, []() {
				gui.MenuVisible() = !gui.MenuVisible();
				} );

			InputSys::ins().RegisterHotkey( config.Misc.SpyModeKey, []()
				{
					if ( !config.Misc.SpyMode )
						gui.DrawToScene( XS( "Spy Mode ativado!" ), 2 );
					else
						gui.DrawToScene( XS( "Spy Mode desativado!" ), 2 );

					config.Misc.SpyMode = !config.Misc.SpyMode;
				} );

			InputSys::ins().RegisterHotkey( config.Misc.LagRoomKey, []()
				{
					gui.DrawToScene( XS( "Lag Room ativado!" ), 2 );
				} );

			InputSys::ins().RegisterHotkey( config.Misc.CrashRoomKey, []()
				{
					gui.DrawToScene( XS( "Crash Room ativado!" ), 2 );
				} );

			gui.Initialize( thisptr, InputSys::ins().GetMainWindow() );
			render.Initialize();
			config_system.LoadConfig( "" );
		}
		else
		{
			if ( Globals::g_bInterfaceInitialized )
			{
				//Menu
				RenderMenuWindow( thisptr );

				//Zombies
				SetupZombies();
			}
		}
	}

	return oFlipScreen( flags );
}

#pragma warning( push )
#pragma warning( disable : 4740)
DWORD g_dwLatency = 0, g_dwLatencyOrig = 0, g_dwLatencyJmp = 0;

/*typedef void( *Latency_ )( void );
Latency_ hkLatency = NULL;

void SetupLatency()
{
	std::uint8_t shellcode[] = { 0x89, 0x4D, 0xF0, 0x60, 0x9C, 0x0F, 0xB6, 0x05, 0xEF, 0xBE, 0xAD, 0xDE, 0x85, 0xC0, 0x74, 0x08, 0x9D, 0x61, 0xFF, 0x25, 0xEF, 0xBE, 0xAD, 0xDE, 0x9D, 0x61, 0xFF, 0x25, 0xEF, 0xBE, 0xAD, 0xDE };
	*( DWORD* )( shellcode + 8 ) = DWORD( &Globals::g_bSpyModeActive );
	*( DWORD* )( shellcode + 20 ) = g_dwLatencyOrig;
	*( DWORD* )( shellcode + 28 ) = g_dwLatencyJmp;

	auto exec = VirtualAlloc( nullptr, sizeof( shellcode ), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE );
	memcpy( exec, shellcode, sizeof( shellcode ) );

	hkLatency = ( Latency_ )exec;

	asmjit::JitRuntime rt;

	asmjit::CodeHolder code;
	code.init( rt.codeInfo() );

	asmjit::x86::Assembler a( &code );

	const auto ret_exec = a.newLabel();

	a.mov( asmjit::x86::dword_ptr( asmjit::x86::ebp, -0x10 ), asmjit::x86::ecx );
	a.pushad();
	a.pushfd();
	a.movzx( asmjit::x86::ax, asmjit::x86::byte_ptr( std::uint32_t( &Globals::g_bSpyModeActive ) ) );
	a.test( asmjit::x86::ax, asmjit::x86::ax );
	a.je( ret_exec );
	a.popfd();
	a.popad();
	a.jmp( g_dwLatencyOrig );

	a.bind( ret_exec );
	a.popfd();
	a.popad();
	a.jmp( g_dwLatencyJmp );

	rt.add( &hkLatency, &code );
}*/

__declspec( naked ) void hkLatency()
{
	__asm
	{
		mov[ ebp - 16 ], ecx
		pushad
		pushfd

		movzx ax, byte ptr[ Globals::g_bSpyModeActive ]
		test ax, ax
		je ret_exec
		popfd
		popad
		jmp g_dwLatencyOrig

		ret_exec :
		popfd
			popad
			jmp g_dwLatencyJmp
	}
}

#pragma warning( pop ) 
#ifdef _DEBUG
void __cdecl hkConsolePrintf( int a1, int a2, LPCSTR lpOutputString )
{
	printf( "%s", lpOutputString );
}
#endif

////////////////////////////////////////////////////
// Bypass PlayerBasicInfo checks

PLAYER_BASICINFO_STRUCT* BackupPlayerInfo[ MAX_PLAYERINFO_COUNT ];

void CreateBackupForPlayer()
{
	for ( int i = 0; i < MAX_PLAYERINFO_COUNT; ++i )
	{
		auto player = engine.GetPlayerInfo( i );

		if ( player != NULL )
		{
			BackupPlayerInfo[ i ] = new PLAYER_BASICINFO_STRUCT;
			memcpy( BackupPlayerInfo[ i ], ( void* )player, sizeof( PLAYER_BASICINFO_STRUCT ) );
		}
	}

#ifdef _DEBUG
	Tools::SaveToLog( "Player Backup was created!\n" );
#endif
}

PLAYER_BASICINFO_STRUCT* BypassPlayerInfo( int id )
{
	if ( id > MAX_PLAYERINFO_COUNT )
		return nullptr;

	return BackupPlayerInfo[ id ];
}

////////////////////////////////////////////////////
// Bypass WeaponManager checks

WEAPON_MGR_STRUCT* BackupWeapons[ MAX_WEAPONS_COUNT ];

void CreateBackupForWeapons()
{
	for ( int i = 0; i < MAX_WEAPONS_COUNT; ++i )
	{
		auto weapon = engine.GetWeaponByIndex( i );

		if ( weapon != NULL )
		{
			BackupWeapons[ i ] = new WEAPON_MGR_STRUCT;
			memcpy( ( void* )BackupWeapons[ i ], ( void* )weapon, sizeof( WEAPON_MGR_STRUCT ) );
		}
	}

#ifdef _DEBUG
	Tools::SaveToLog( "Weapon Backup was created!\n" );
#endif
}

WEAPON_MGR_STRUCT* BypassWeaponMgr( __int16 id )
{
	if ( id > MAX_WEAPONS_COUNT )
		return nullptr;

	return BackupWeapons[ id ];
}

////////////////////////////////////////////////////
// Bypass ModelNodeManager checks

NODE_MGR_STRUCT* BackupNodes[ MAX_NODES_COUNT ];

void CreateBackupForNode()
{
	for ( int i = 0; i < MAX_NODES_COUNT; ++i )
	{
		auto node = engine.GetNodeByIndex( i );

		if ( node != NULL )
		{
			BackupNodes[ i ] = new NODE_MGR_STRUCT;
			memcpy( ( void* )BackupNodes[ i ], ( void* )node, sizeof( NODE_MGR_STRUCT ) );
		}
	}

#ifdef _DEBUG
	Tools::SaveToLog( "Node Backup was created!\n" );
#endif
}

NODE_MGR_STRUCT* BypassNodeMgr( int id )
{
	if ( id > MAX_NODES_COUNT )
		return nullptr;

	return BackupNodes[ id ];
}

#ifdef _DEBUG
void CaptureCrash( _EXCEPTION_POINTERS* ex )
{
	Tools::SaveToCrashLog( "My Module: 0x%X - Size: 0x%X\n", Globals::Module::OwnModule, Globals::Module::OwnModuleSize );
	Tools::SaveToCrashLog( "Crash at: 0x%p - Code: %X\n", ex->ExceptionRecord->ExceptionAddress, ex->ExceptionRecord->ExceptionCode );
	Tools::SaveToCrashLog( "EAX: %X\n", ex->ContextRecord->Eax );
	Tools::SaveToCrashLog( "ECX: %X\n", ex->ContextRecord->Ecx );
	Tools::SaveToCrashLog( "EDX: %X\n", ex->ContextRecord->Edx );
	Tools::SaveToCrashLog( "EIP: %X\n", ex->ContextRecord->Eip );
	Tools::SaveToCrashLog( "ESI: %X\n", ex->ContextRecord->Esi );
	Tools::SaveToCrashLog( "ESP: %X\n", ex->ContextRecord->Esp );
	Tools::SaveToCrashLog( "EBX: %X\n", ex->ContextRecord->Ebx );
	Tools::SaveToCrashLog( "EBP: %X\n\n", ex->ContextRecord->Ebp );

	HMODULE hModule = 0;
	if (
		GetModuleHandleExA( GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, LPCSTR( ex->ExceptionRecord->ExceptionAddress ), &hModule )
		)
	{
		char szModuleName[ MAX_PATH ]{ };
		GetModuleBaseNameA( NtCurrentProcess(), hModule, szModuleName, sizeof szModuleName );
		Tools::SaveToCrashLog( "Module: %p - Name: %s\n", hModule, szModuleName );
	}
	else
		Tools::SaveToCrashLog( "Module not linked!\n" );

	PVOID Stack[ 64 ] = { 0 };
	const auto StackSize = RtlCaptureStackBackTrace( 0, 63, Stack, 0 );

	if ( StackSize > 0 )
	{
		Tools::SaveToCrashLog( "Stack Log. Size: %d\n\n", StackSize );

		for ( WORD i = 0; i < StackSize; ++i )
		{
			auto Addr = Stack[ i ];

			if (
				GetModuleHandleExA( GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, LPCSTR( Addr ), &hModule )
				)
			{
				char szModuleName[ MAX_PATH ]{ };
				GetModuleBaseNameA( NtCurrentProcess(), hModule, szModuleName, sizeof szModuleName );

				Tools::SaveToCrashLog( "VA: 0x%p - RVA: 0x%X (%s)\n", Addr, DWORD( Addr ) - DWORD( hModule ), szModuleName );
			}
			else
				Tools::SaveToCrashLog( "VA: 0x%p\n", Addr );
		}
	}

	const DWORD Flags =
		MiniDumpWithIndirectlyReferencedMemory |
		MiniDumpWithDataSegs |
		MiniDumpWithHandleData |
		MiniDumpWithFullMemory;

	HANDLE hFile = CreateFileA( "C:\\CF.dmp", GENERIC_ALL, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr );
	if ( hFile != INVALID_HANDLE_VALUE )
	{
		_MINIDUMP_EXCEPTION_INFORMATION ei{ };
		ei.ThreadId = GetCurrentThreadId();
		ei.ExceptionPointers = ex;
		ei.ClientPointers = TRUE;

		BOOL Result = MiniDumpWriteDump(
			NtCurrentProcess(),
			GetCurrentProcessId(),
			hFile,
			( MINIDUMP_TYPE )Flags,
			&ei,
			nullptr,
			nullptr );

		CloseHandle( hFile );

		if ( !Result )
			Tools::SaveToCrashLog( "Failed to MiniDumpWriteDump 0x%X\n", GetLastError() );
		else
			Tools::SaveToCrashLog( "Crash Minidump created successfully!\n" );
	}
	else
		Tools::SaveToCrashLog( "Failed to CreateFileA 0x%X\n", GetLastError() );

	Tools::SaveToCrashLog( "\n\n" );
}
#endif

LONG WINAPI NullExceptionHandler( _EXCEPTION_POINTERS* ex )
{
	if ( ex->ExceptionRecord->ExceptionCode == EXCEPTION_BREAKPOINT )
		return EXCEPTION_EXECUTE_HANDLER;

#ifdef _DEBUG
	CaptureCrash( ex );

	MSGBOXPARAMSW mbp{ };
	mbp.cbSize = sizeof mbp;
	mbp.hwndOwner = GetActiveWindow();
	mbp.hInstance = GetModuleHandleA( NULL );
	mbp.lpszCaption = L"Crash Report";
	mbp.lpszText = L"O CrossFire crashou e um log de crash foi gerado em C:\\";
	mbp.dwStyle = MB_ICONSTOP | MB_OK;
	mbp.lpszIcon = nullptr;
	MessageBoxIndirectW( &mbp );
#else
	ExitProcess( 0 );
#endif

	return EXCEPTION_EXECUTE_HANDLER;
}

///CheckSum DLL Hook
using CSCheckDLL_ = int* ( * )( );
CSCheckDLL_ oCSCheckDLL = NULL;

int* hkCheckDLLHook()
{
	Globals::g_bIsSafeToHook = false;
#ifdef _DEBUG
	Tools::SaveToLog( "[PRE CHECKDLL]" );
#endif
	AllHooksRestore();
	const auto res = oCSCheckDLL();
#ifdef _DEBUG
	Tools::SaveToLog( "CheckDLL performed and returned: %p\n", res );
#endif
	AllHooksReHook();
	Globals::g_bIsSafeToHook = true;
	return res;
}

///CheckSum 1
using CSChecksum1_ = int( * )( );
CSChecksum1_ oCSChecksum1 = NULL;

int hkCSChecksum()
{
#ifdef _DEBUG
	Tools::SaveToLog( "[PRE CSCHKSUM]" );
#endif
	AllHooksRestore();
	const auto ret = oCSChecksum1();
#ifdef _DEBUG
	Tools::SaveToLog( "ClientChecksum check performed and returned: %p\n", ret );
#endif
	AllHooksReHook();
	return ret;
}

///CheckSum 2
using CSChecksum2_ = int( * )( );
CSChecksum2_ oCSChecksum2 = NULL;

int hkCSChecksum2()
{
	AllHooksRestore();
	const auto ret = oCSChecksum2();
	AllHooksReHook();
	return ret;
}

///CheckSum 3
using CSChecksum3_ = int( __cdecl* )( int );
CSChecksum3_ oCSChecksum3 = NULL;

int __cdecl hkCSChecksum3( int a1 )
{
	AllHooksRestore();
	const auto ret = oCSChecksum3( a1 );
	AllHooksReHook();
	return ret;
}

PVOID __cdecl hkGetWeaponByIndex( __int16 a1 )
{
	DWORD ret_addr = DWORD( _ReturnAddress() );
	if ( ret_addr >= Globals::Module::CShell && ret_addr <= Globals::Module::CShell + Globals::Module::CShellSize )
	{
		static DWORD Ret01 = Tools::FindPattern< DWORD >( HASH_CSHELL_DLL, PBYTE( "\x8B\xD8\x83\xC4\x04\x85\xDB\x0F\x84\x00\x00\x00\x00\x8B\x0D\x00\x00\x00\x00\x8D\x93\x00\x00\x00\x00\x6A\x3C\x52\x8B\x39" ), XS( "xxxxxxxxx????xx????xx????xxxxx" ) );
		static DWORD Ret02 = Tools::FindPattern< DWORD >( HASH_CSHELL_DLL, PBYTE( "\x8B\xF0\x83\xC4\x04\x85\xF6\x0F\x84\x00\x00\x00\x00\xF3\x0F\x10\x86\x00\x00\x00\x00" ), XS( "xxxxxxxxx????xxxx????" ) );

		if (
			DWORD( _ReturnAddress() ) == Ret01 ||
			DWORD( _ReturnAddress() ) == Ret02 )
		{
			return BackupWeapons[ a1 ];
		}
	}
	return oGetWeaponByIndex( a1 );
}

PVOID __cdecl hkGetBasicPlayerInfo( int a1 )
{
	DWORD ret_addr = DWORD( _ReturnAddress() );
	if ( ret_addr >= Globals::Module::CShell && ret_addr <= Globals::Module::CShell + Globals::Module::CShellSize )
	{
		CBasicPlayerInfo* player = ( CBasicPlayerInfo* )oGetBasicPlayerInfo( a1 );
		CBasicPlayerInfo* backup_player = GetBackupPlayer( a1 );

		static DWORD Ret01 = Tools::FindPattern< DWORD >( HASH_CSHELL_DLL, PBYTE( "\x8B\x5D\x0C\x83\xC4\x04\x85\xC0\x74\x18\x68\x00\x00\x00\x00" ), XS( "xxxxxxxxxxx????" ) );
		if ( DWORD( _ReturnAddress() ) == Ret01 )
			return backup_player;

		if ( player && backup_player )
		{
			if ( config.Misc.FastWalk )
				player->MovementWalkRate = 1.15f;
			else
				player->MovementWalkRate = backup_player->MovementWalkRate;

			/*if ( config.Misc.InstaDefuse )
				player->C4DefuseTime = 2.f;
			else
				player->C4DefuseTime = backup_player->C4DefuseTime;

				if (config.misc[18])
					player->MovementDuckWalkRate = config.misc_misc[2];
				else
					player->MovementDuckWalkRate = backup_player->MovementDuckWalkRate;
			*/
		}
	}
	return oGetBasicPlayerInfo( a1 );
}

typedef LTRESULT( __thiscall* hSendToServer )( void*, ILTMessage_Read*, uint32_t );
hSendToServer oSendToServer = 0;

auto GetMessageNameByID( uint16 i )
{
	return *reinterpret_cast< char** >( Globals::Module::CShell + 0x12AF008 + ( sizeof( DWORD ) * i ) );
}

void SaveIdsToEnum()
{
	/*static bool first = false;

	FILE* f = nullptr;
	std::remove( "MsgIDs.hpp" );
	fopen_s( &f, "MsgIDs.hpp", "a" );
	fprintf_s( f, "enum\n{\n" );

	for ( int i = 0; i < 3000; ++i )
	{
		auto cur = GetMessageNameByID( i );
		if ( cur != nullptr && !IsBadReadPtr( cur, sizeof PVOID ) && strlen( cur ) > 0 && strstr( cur, "CS" ) )
		{
			if ( !first )
			{
				first = true;
				fprintf_s( f, "\t%s = %d", cur, i );
			}
			else
				fprintf_s( f, ",\n\t%s = %d", cur, i  );
		}
	}
	fprintf_s( f, "\n}" );
	fclose( f );*/
}

LTRESULT __fastcall hkSendToServer( void* thisptr, void* edx, ILTMessage_Read* msg, uint32_t flags )
{
	const uint16 id = msg->Readuint16();
	/*const auto msg_size = msg->Size();

	//32864  137  16384

	if ( id != 5 && id != 3 && id != 29 && id != 31 && id != 274 && id != 82 && id != 209 )
	{
		Tools::SaveToLog( "[S2S] ID: %d [%s] Size: %d - RetAddr: 0x%X\n", id, GetMessageNameByID( id ), msg_size, DWORD( _ReturnAddress() ) - Globals::Module::CShell );
	}
	*/

	/*auto msg_name = GetMessageNameByID( id );
	if (
		strstr( msg_name, "NANO") ||
		id == MSG_CS_THROWGRENADE ||
		id == MSG_CS_DAMAGE_OBJECT ||
		id == MSG_CS_USEAMMOSUPPLYSITE_REQ ||
		id == MSG_CS_DZBOXSIZE_RESULT ||
		id == MSG_CS_CHANGE_C4_SKIN )
	{
		Tools::SaveToLog( "[S2S] ID: %d [%s] Size: %d - RetAddr: 0x%X\n", id, GetMessageNameByID( id ), msg_size, DWORD( _ReturnAddress() ) - Globals::Module::CShell );
	}*/

	/*if ( id == MSG_CS_GUNDIRROT )
	{
		oSendToServer( thisptr, msg, flags );

		CAutoMessage msg;

		uint8 counter = *( uint8* )( Globals::Module::CShell + 0x1A7D218 );
		*( uint8* )( Globals::Module::CShell + 0x1A7D218 ) = counter + 1;

		msg.Writeuint16( MSG_CS_GUNDIRROT );
		counter = 16 * counter | ( counter >> 4 ) & 0xF;
		msg.WriteBits( 0xB27AD5F7, 5 );
		msg.Writeuint8( counter );

		msg.Writefloat( -10.f );
		msg.Writefloat( 90.f );

		auto msg_len = msg.Size();
		msg.Writeuint8( ( uint8 )( msg_len * counter ) );

		return oSendToServer( thisptr, msg.Read(), MESSAGE_GUARANTEED );
	}*/

	if ( config.Misc.InstaDefuse && ( id == MSG_CS_REQ_CANDEFUSEC4 || id == MSG_CS_KINGS_REQ_C4DEFUSE ) )
	{
		oSendToServer( thisptr, msg, flags );

		CAutoMessage msg;

		uint8 counter = engine.GetS2SCounter();
		engine.GetS2SCounter() = counter + 1;

		msg.Writeuint16( MSG_CS_DEFUSEC4_SUCCESS );
		counter = 16 * counter | ( counter >> 4 ) & 0xF;
		msg.WriteBits( 0xB27AD5F7, 5 );
		msg.Writeuint8( counter );
		auto msg_len = msg.Size();
		msg.Writeuint8( ( uint8 )( msg_len * counter ) );

		return oSendToServer( thisptr, msg.Read(), MESSAGE_GUARANTEED );
	}

	if ( config.Misc.NoNadeDamage && id == MSG_CS_DAMAGE )
	{
		uint8 v30 = engine.GetS2SCounter();
		engine.GetS2SCounter() = v30 - 1;
		return LT_OK;
	}

	return oSendToServer( thisptr, msg, flags );
}

void __stdcall DirectXHook( PVOID )
{
	IDirect3DDevice9* device = nullptr;

	while ( true )
	{
		if ( !( device = GetDevice() ) )
			continue;

		auto* vtbl = reinterpret_cast< uintptr_t* >( device );
		vtbl = reinterpret_cast< uintptr_t* >( vtbl[ 0 ] );

		if ( vtbl[ 82 ] != ( DWORD )&hkDrawIndexedPrimitive )
		{
			oDrawIndexedPrimitive = ( DrawIndexedPrimitive_ )
				vtbl[ 82 ];

			DWORD dwOld = NULL;
			VirtualProtect( &vtbl[ 82 ], sizeof( DWORD ), PAGE_READWRITE, &dwOld );
			vtbl[ 82 ] = ( DWORD )&hkDrawIndexedPrimitive;
			VirtualProtect( &vtbl[ 82 ], sizeof( DWORD ), dwOld, &dwOld );
		}
		Sleep( 1 );
	}

	//DWORD dwDXTable[ 5 ]{ };
	//if ( CreateDevice( dwDXTable ) )
	//	//GetD3Device(dwDXTable);
	//{
	//	//	oReset = ( hReset )CreateHook( PBYTE( dwDXTable[ 0 ] ), PBYTE( &hkReset ) );
	//	// oPresent = ( hPresent )CreateHook( PBYTE( dwDXTable[ 1 ] ), PBYTE( &hkPresent ) );
	//	oDrawIndexedPrimitive = ( DrawIndexedPrimitive_ )CreateHook( PBYTE( dwDXTable[ 2 ] ), PBYTE( &hkDrawIndexedPrimitive ) );
	//}
}

////////////////////////////////////////////////////////
// Main Thread
void __stdcall StartThread( PVOID )
{
	VIRTUALIZER_TIGER_RED_START;

	srand( ( UINT )std::time( 0 ) );

	//Crash Log
	//std::filesystem::remove( "C:\\CRASH.log" );

	while ( !Tools::FindModuleHandle( HASH_X3_XEM ) )
		Sleep( 500 );

	while ( !Tools::FindModuleHandle( HASH_CSHELL_DLL ) )
		Sleep( 500 );

	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

	Globals::Module::CrossFire = Tools::FindModuleHandle( HASH_CROSSFIRE_EXE, &Globals::Module::CrossFireSize );
	Globals::Module::CShell = Tools::FindModuleHandle( HASH_CSHELL_DLL, &Globals::Module::CShellSize );

	/*{
		PBYTE CShellCodeBase = 0;
		DWORD CShellCodeSize = 0;
		Tools::GetCodeBaseAndSize( Globals::Module::CShell, &CShellCodeBase, &CShellCodeSize );
		CShellBackup = new BYTE[ CShellCodeSize ];
		memcpy( CShellBackup, CShellCodeBase, CShellCodeSize );
	}

	PBYTE CFEXECodeBase = 0;
	DWORD CFEXECodeSize = 0;
	Tools::GetCodeBaseAndSize( Globals::Module::CrossFire, &CFEXECodeBase, &CFEXECodeSize );
	CFEXEBackup = new BYTE[ CFEXECodeSize ];
	memcpy( CFEXEBackup, CFEXECodeBase, CFEXECodeSize );*/

	//Disable Crash Reporting
	DWORD CrashReport = Tools::FindPattern< DWORD >( HASH_CROSSFIRE_EXE, ( PBYTE )"\x55\x8B\xEC\x83\xEC\x10\x6A\x08", XS( "xxxxxxxx" ) );
	if ( CrashReport )
	{
		//	CrashReport = Tools::ResolveRelative(CrashReport);
		CreateHook( PBYTE( CrashReport ), PBYTE( &NullExceptionHandler ) );
#ifdef _DEBUG
		Tools::SaveToLog( "CrashReport -> 0x%X\n", CrashReport );
#endif
	}
#ifdef _DEBUG
	else
	{
		Tools::SaveToLog( "!!! CrashReport not found !!!" );
	}
#endif

	//Hook mapped ntdll.dll
//	_beginthread( HookAntiCheat, 0, 0 );

	//Latency Hook
	g_dwLatency = Tools::FindPattern< DWORD >( HASH_CROSSFIRE_EXE, ( PBYTE )"\x89\x4D\xF0\x33\xC0\x74\x6C", XS( "xxxxxxx" ) );
	if ( g_dwLatency )
	{
		g_dwLatencyOrig = g_dwLatency + 0x7;
		g_dwLatencyJmp = Tools::FindPattern< DWORD >( HASH_CROSSFIRE_EXE, ( PBYTE )"\x8B\x45\x10\x50\x8B\x4D\x0C\x51\x8B\x55\x08\x52\x8B\x4D\xF0\xE8\x00\x00\x00\x00\x88\x45\xFF\x8A\x45\xFF", XS( "xxxxxxxxxxxxxxxx????xxxxxx" ) );
#ifdef _DEBUG
		Tools::SaveToLog( "Latency -> 0x%X\n", g_dwLatency );
		Tools::SaveToLog( "LatencyJMP -> 0x%X\n", g_dwLatencyJmp );
#endif
		CreateHook( PBYTE( g_dwLatency ), PBYTE( &hkLatency ) );
		/*SetupLatency();
		if ( hkLatency )
		{
#ifdef _DEBUG
			Tools::SaveToLog( "hkLatency -> 0x%X\n", hkLatency );
#endif
			CreateHook( PBYTE( g_dwLatency ), PBYTE( hkLatency ) );
		}*/
	}
#ifdef _DEBUG
	else
	{
		Tools::SaveToLog( "!!! LatencyPTR not found !!!" );
	}
#endif

	while ( !Tools::FindModuleHandle( HASH_D3D9_DLL ) )
		Sleep( 500 );

#ifdef _DEBUG
	Tools::SaveToLog( "[!] d3d9.dll found!\n" );
#endif

	CreateRemoteThread( ( HANDLE )-1, nullptr, NULL, PTHREAD_START_ROUTINE( DirectXHook ), nullptr, NULL, nullptr );

	while ( !Tools::FindModuleHandle( HASH_CLIENTFX_FXD ) )
		Sleep( 250 );

#ifdef _DEBUG
	Tools::SaveToLog( "[!] ClientFx.fxd found!\n" );
#endif

	//LTClientShell
	Globals::Pointer::dwLTClientShell = Tools::FindPattern< DWORD >( HASH_CSHELL_DLL, ( PBYTE )"\x8B\x0D\x00\x00\x00\x00\x6A\x00\x6A\x00\x6A\x00\xFF\xB5\x00\x00\x00\x00\x8B\x89\x00\x00\x00\x00", XS( "xx????xxxxxxxx????xx????" ) );
	if ( Globals::Pointer::dwLTClientShell )
		Globals::Pointer::dwLTClientShell = *( std::uintptr_t* )( Globals::Pointer::dwLTClientShell + 0x2 );
#ifdef _DEBUG
	else
	{
		Tools::SaveToLog( "!!! LTClientShell not found !!!\n" );
	}
#endif

	//LTModelClient
	Globals::Pointer::dwLTModelClient = Tools::FindPattern< DWORD >( HASH_CSHELL_DLL, ( PBYTE )"\x8B\x0D\x00\x00\x00\x00\x8D\x95\x00\x00\x00\x00\x6A\x01\x52\xFF\x77\x10\x8B\x01", XS( "xx????xx????xxxxxxxx" ) );
	if ( Globals::Pointer::dwLTModelClient )
		Globals::Pointer::dwLTModelClient = *( std::uintptr_t* )( Globals::Pointer::dwLTModelClient + 0x2 );
#ifdef _DEBUG
	else
	{
		Tools::SaveToLog( "!!! LTModel not found !!!\n" );
	}
#endif

	//LTClient
	Globals::Pointer::dwLTClient = Tools::FindPattern< DWORD >( HASH_CSHELL_DLL, ( PBYTE )"\x8B\x0D\x00\x00\x00\x00\x52\x8B\x01\xFF\x76\x20", XS( "xx????xxxxxx" ) );
	if ( Globals::Pointer::dwLTClient != NULL )
		Globals::Pointer::dwLTClient = *( std::uintptr_t* )( Globals::Pointer::dwLTClient + 0x2 );
#ifdef _DEBUG
	else
	{
		Tools::SaveToLog( "!!! LTClient not found !!!\n" );
	}
#endif

#ifdef _DEBUG
	Tools::SaveToLog( "LTClientDLL -> 0x%X\n", Globals::Pointer::dwLTClient );
	Tools::SaveToLog( "LTClientShell -> 0x%X\n", Globals::Pointer::dwLTClientShell );
	Tools::SaveToLog( "LTModelClient -> 0x%X\n", Globals::Pointer::dwLTModelClient );
#endif

	//GetCurrentPlayerID
	Globals::Address::FnGetLocalPlayerIndex = Tools::FindPattern< DWORD >( HASH_CSHELL_DLL, ( PBYTE )"\x56\x8B\xF1\x0F\xB6\x86\x00\x00\x00\x00\x50\xE8\x00\x00\x00\x00\x83\xC4\x04\x84\xC0\x75\x04", XS( "xxxxxx????xx????xxxxxxx" ) );
	if ( Globals::Address::FnGetLocalPlayerIndex )
	{
		GetLocalPlayerIndex = ( hGetLocalPlayerIndex )Globals::Address::FnGetLocalPlayerIndex;
#ifdef _DEBUG
		Tools::SaveToLog( "GetLocalPlayerIndex -> 0x%X\n", Globals::Address::FnGetLocalPlayerIndex );
#endif
	}
#ifdef _DEBUG
	else
	{
		Tools::SaveToLog( "!!! GetLocalPlayerIndex not found !!!\n" );
	}
#endif

	//ESP
	Globals::Offset::uPlayerSize = Tools::FindPattern< DWORD >( HASH_CSHELL_DLL, ( PBYTE )"\x69\xC0\x00\x00\x00\x00\x8A\x84\x30\x00\x00\x00\x00\x5E", XS( "xx????xxx????x" ) );
	if ( Globals::Offset::uPlayerSize )
		Globals::Offset::uPlayerSize = *( DWORD* )( Globals::Offset::uPlayerSize + 0x2 );
#ifdef _DEBUG
	else
	{
		Tools::SaveToLog( "!!! players_size not found !!!\n" );
	}
#endif

	//Players offset in class
	Globals::Offset::uPlayerOffset = Tools::FindPattern< DWORD >( HASH_CSHELL_DLL, ( PBYTE )"\x0F\xB6\x86\x00\x00\x00\x00\x69\xC0\x00\x00\x00\x00\x8A\x84\x30\x00\x00\x00\x00", XS( "xxx????xx????xxx????" ) );
	if ( Globals::Offset::uPlayerOffset )
		Globals::Offset::uPlayerOffset = *( DWORD* )( Globals::Offset::uPlayerOffset + 0x3 );
#ifdef _DEBUG
	else
	{
		Tools::SaveToLog( "!!! players_offset not found !!!\n" );
	}
#endif

#ifdef _DEBUG
	Tools::SaveToLog( "PlayerSize -> 0x%X\n", Globals::Offset::uPlayerSize );
	Tools::SaveToLog( "PlayerOff -> 0x%X\n", Globals::Offset::uPlayerOffset );
#endif

	//GlowStrenght
	Globals::Address::dwSetGlowStrenght = Tools::FindPattern< DWORD >( HASH_CROSSFIRE_EXE, ( PBYTE )"\xE8\x00\x00\x00\x00\xEB\x0E\x83\x3D\x00\x00\x00\x00\x00", XS( "x????xxxx?????" ) );
	if ( Globals::Address::dwSetGlowStrenght )
	{
		Globals::Address::dwSetGlowStrenght = Tools::ResolveRelative( Globals::Address::dwSetGlowStrenght );
		Globals::Address::dwSetGlowStrenght += 0x5E;
		Globals::Address::dwSetGlowStrenght = *( DWORD* )( Globals::Address::dwSetGlowStrenght + 0x4 );
#ifdef _DEBUG
		Tools::SaveToLog( "SetGlowStrenght -> 0x%X\n", Globals::Address::dwSetGlowStrenght );
#endif
	}
#ifdef _DEBUG
	else
	{
		Tools::SaveToLog( "!!! SetGlowStrenght not found !!!\n" );
	}
#endif

	//SetGlowOutline
	Globals::Address::FnSetGlowOutline = Tools::FindPattern< DWORD >( HASH_CROSSFIRE_EXE, ( PBYTE )"\x55\x8B\xEC\x83\xEC\x1C\x89\x4D\xE4\x83\x7D\x08\x00\x74\x27", XS( "xxxxxxxxxxxxxxx" ) );
	if ( Globals::Address::FnSetGlowOutline )
	{
		FnSetOutline = ( SetObjectOutline_ )Globals::Address::FnSetGlowOutline;
#ifdef _DEBUG
		Tools::SaveToLog( "SetGlowOutline -> 0x%X\n", Globals::Address::FnSetGlowOutline );
#endif
	}
#ifdef _DEBUG
	else
	{
		Tools::SaveToLog( "!!! SetGlowOutline not found !!!" );
	}
#endif

	//SetGlowActive
	Globals::Address::FnSetGlowActive = Tools::FindPattern< DWORD >( HASH_CSHELL_DLL, ( PBYTE )"\x55\x8B\xEC\x51\xA1\x00\x00\x00\x00\x53\x8B\x5D\x08\x56\x8B\x80\x00\x00\x00\x00", XS( "xxxxx????xxxxxxx????" ) );
	if ( Globals::Address::FnSetGlowActive )
	{
		FnGlowSetActive = ( GlowSetActive_ )Globals::Address::FnSetGlowActive;
#ifdef _DEBUG
		Tools::SaveToLog( "SetGlowActive -> 0x%X\n", Globals::Address::FnSetGlowActive );
#endif
	}
#ifdef _DEBUG
	else
	{
		Tools::SaveToLog( "!!! SetGlowActive not found !!!\n" );
	}
#endif

	//UpdateGlowOutline
	Globals::Address::FnUpdateGlowOutine = Tools::FindPattern< DWORD >( HASH_CSHELL_DLL, ( PBYTE )"\xE8\x00\x00\x00\x00\x33\xFF\x8D\x9E\x00\x00\x00\x00\x66\x0F\x1F\x44\x00\x00", XS( "x????xxxx????xxxxx?" ) );
	if ( Globals::Address::FnUpdateGlowOutine )
	{
		Globals::Address::FnUpdateGlowOutine = Tools::ResolveRelative( Globals::Address::FnUpdateGlowOutine );
		FnUpdateOutline = ( UpdateOutline_ )Globals::Address::FnUpdateGlowOutine;
#ifdef _DEBUG
		Tools::SaveToLog( "UpdateGlowOutline -> 0x%X\n", Globals::Address::FnUpdateGlowOutine );
#endif
	}
#ifdef _DEBUG
	else
	{
		Tools::SaveToLog( "!!! UpdateGlowOutline not found !!!\n" );
	}
#endif

	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	auto diff = std::chrono::duration_cast< std::chrono::seconds >( end - begin ).count();
	//Tools::SaveToLog( "Time to initialize everything: %lld (seconds)\n", diff );
	//SaveIdsToEnum();

	g_pLTClientShell = *( CLTClientShell** )Globals::Pointer::dwLTClientShell;
	g_pLTModel = *( CLTModel** )Globals::Pointer::dwLTModelClient;
	g_pLTClient = *( CLTClient** )Globals::Pointer::dwLTClient;

	while ( !g_pLTClient )
		Sleep( 150 );

	g_pLTCommon = g_pLTClient->GetLTCommon();

	while ( !g_pLTCommon )
		Sleep( 150 );

#ifdef _DEBUG
	Tools::SaveToLog( "lt.RenderCamera = 0x%X\n", g_pLTClient->RenderCamera );
	Tools::SaveToLog( "lt.IntersectSegment = 0x%X\n", g_pLTClient->IntersectSegment );
	Tools::SaveToLog( "lt.FlipScreen = 0x%X\n", g_pLTClient->FlipScreen );
#endif

	oRenderCamera = ( hRenderCamera )
		CreateHook( PBYTE( g_pLTClient->RenderCamera ), PBYTE( &hkRenderCamera ) );

	oIntersectSegment = ( hIntersectSegment )
		CreateHook( PBYTE( g_pLTClient->IntersectSegment ), PBYTE( &hkIntersectSegment ) );

	oFlipScreen = ( hFlipScreen )
		CreateHook( PBYTE( g_pLTClient->FlipScreen ), PBYTE( &hkFlipScreen ) );

	oSendToServer = ( hSendToServer )
		CreateHook( PBYTE( 0x699C50 ), PBYTE( &hkSendToServer ) );

	//Check DLL Hook
	auto dwDllHookCheck = Tools::FindPattern< DWORD >( HASH_CSHELL_DLL, PBYTE( "\x55\x8B\xEC\x6A\xFF\x68\x00\x00\x00\x00\x64\xA1\x00\x00\x00\x00\x50\x81\xEC\x00\x00\x00\x00\xA1\x00\x00\x00\x00\x33\xC5\x89\x45\xF0\x53\x56\x57\x50\x8D\x45\xF4\x64\xA3\x00\x00\x00\x00\xA1\x00\x00\x00\x00\x8B\x80\x00\x00\x00\x00\xFF\xD0\x85\xC0\x0F\x84\x00\x00\x00\x00" ), XS( "xxxxxx????xx????xxx????x????xxxxxxxxxxxxxx????x????xx????xxxxxx????" ) );
	if ( dwDllHookCheck )
	{
		oCSCheckDLL = ( CSCheckDLL_ )CreateHook( PBYTE( dwDllHookCheck ), PBYTE( &hkCheckDLLHook ) );
#ifdef _DEBUG
		Tools::SaveToLog( "ClientDLLHook 0x%X ( %X )\n", dwDllHookCheck, *( BYTE* )dwDllHookCheck );
#endif
	}
#ifdef _DEBUG
	else
	{
		Tools::SaveToLog( "!!! ClientDLLHook not found !!!\n" );
	}
#endif

	//ClientShell Checksum
	auto dwClientCheck1 = Tools::FindPattern< DWORD >( HASH_CSHELL_DLL, PBYTE( "\x55\x8B\xEC\x81\xEC\x00\x00\x00\x00\xA1\x00\x00\x00\x00\x33\xC5\x89\x45\xFC\x56\x8B\x35\x00\x00\x00\x00\x57\x8B\x3D\x00\x00\x00\x00\x3B\xFE\x0F\x84\x00\x00\x00\x00\x68\x00\x00\x00\x00\x8D\x85\x00\x00\x00\x00\xC7\x85\x00\x00\x00\x00\x00\x00\x00\x00" ), XS( "xxxxx????x????xxxxxxxx????xxx????xxxx????x????xx????xx????????" ) );
	if ( dwClientCheck1 )
	{
		oCSChecksum1 = ( CSChecksum1_ )CreateHook( PBYTE( dwClientCheck1 ), PBYTE( &hkCSChecksum ) );
#ifdef _DEBUG
		Tools::SaveToLog( "ClientChecksum1 0x%X ( %X )\n", dwClientCheck1, *( BYTE* )dwClientCheck1 );
#endif
	}
#ifdef _DEBUG
	else
	{
		Tools::SaveToLog( "!!! ClientChecksum1 not found !!!\n" );
	}
#endif

#ifdef _DEBUG
	/*while ( !g_pLTClient->GetLTPhysics() )
		Sleep( 150 );

	oGetObjectDim = ( hGetObjectDim )
		Tools::ReplaceTable( ( PDWORD* )g_pLTClient->GetLTPhysics(), PBYTE( &hkGetObjectDim ), 14 ); */
#endif

		/*	auto ClientFxPlayer = DWORD( Tools::GeGetProcAddress( GetModuleHandleW( HASH_CLIENTFX_FXD ), XS( "fxSetPlayer" ) ) );
		if ( ClientFxPlayer )
		{
			ClientFxPlayer = *( DWORD* )( ClientFxPlayer + 0x7 );
	#ifdef _DEBUG
			Tools::SaveToLog( "ClientFxPlayer 0x%p\n", ClientFxPlayer );
	#endif
		}*/

		//GetWeaponByIndex
	Globals::Address::FnGetWeaponByID = Tools::FindPattern< DWORD >( HASH_CSHELL_DLL, PBYTE( "\xE8\x00\x00\x00\x00\x8B\xF0\x83\xC4\x04\x85\xF6\x0F\x84\x00\x00\x00\x00\x8B\x8B\x00\x00\x00\x00" ), XS( "x????xxxxxxxxx????xx????" ) );
	if ( Globals::Address::FnGetWeaponByID )
	{
		Globals::Address::FnGetWeaponByID = Tools::ResolveRelative( Globals::Address::FnGetWeaponByID );
#ifdef _DEBUG
		Tools::SaveToLog( "GetWeaponByIndex 0x%p\n", Globals::Address::FnGetWeaponByID );
#endif
	}
#ifdef _DEBUG
	else
	{
		Tools::SaveToLog( "!!! GetWeaponByIndex not found !!!\n" );
	}
#endif

	//GetPlayerByIndex
	Globals::Address::FnGetBPIByID = Tools::FindPattern< DWORD >( HASH_CSHELL_DLL, PBYTE( "\xE8\x00\x00\x00\x00\x83\xC4\x08\xD9\x40\x10" ), XS( "x????xxxxxx" ) );
	if ( Globals::Address::FnGetBPIByID )
	{
		Globals::Address::FnGetBPIByID = Tools::ResolveRelative( Globals::Address::FnGetBPIByID );
#ifdef _DEBUG
		Tools::SaveToLog( "GetBasicPlayerInfo 0x%p\n", Globals::Address::FnGetBPIByID );
#endif
	}
#ifdef _DEBUG
	else
	{
		Tools::SaveToLog( "!!! GetBasicPlayerInfo not found !!!\n" );
	}
#endif

	Globals::g_bInterfaceInitialized = true;

	//Create Player Bypasses
	CreateBackupForPlayer();
	CreateBackupForWeapons();

	if ( Globals::Address::FnGetWeaponByID )
	{
		oGetWeaponByIndex = ( GetWeaponByIndex_ )
			CreateHook( PBYTE( Globals::Address::FnGetWeaponByID ), PBYTE( &hkGetWeaponByIndex ) );
	}

	if ( Globals::Address::FnGetBPIByID )
	{
		oGetBasicPlayerInfo = ( GetBasicPlayerInfo_ )
			CreateHook( PBYTE( Globals::Address::FnGetBPIByID ), PBYTE( &hkGetBasicPlayerInfo ) );
	}

#ifdef _DEBUG
	Tools::SaveToLog( "CShell .code AF hash: 0x%X\n", Tools::GetCodeHash( Globals::Module::CShell ) );
	Tools::SaveToLog( "CF.EXE .code AF hash: 0x%X\n", Tools::GetCodeHash( Globals::Module::CrossFire ) );
#endif
	//AddVectoredExceptionHandler(1, CBreakpointManager::ExceptionHandler);
	//BPManager.SetBreakPointAllThreads(BREAKPOINT_TYPE::TYPE_READWRITE, BREAKPOINT_SIZE::SIZE_1, PVOID(own_module + 0x1000));
	//BPManager.SetBreakPointAllThreads(BREAKPOINT_TYPE::TYPE_READWRITE, BREAKPOINT_SIZE::SIZE_4, PVOID(own_module + 0x1000));
	//BPManager.SetBreakPointAllThreads(BREAKPOINT_TYPE::TYPE_READWRITE, BREAKPOINT_SIZE::SIZE_1, PVOID(dwDXTable[2]));
	//BPManager.SetBreakPointAllThreads(BREAKPOINT_TYPE::TYPE_READWRITE, BREAKPOINT_SIZE::SIZE_4, PVOID(dwDXTable[2]));

	VIRTUALIZER_TIGER_RED_END;
}

/*
LONG WINAPI CBreakpointManager::ExceptionHandler( EXCEPTION_POINTERS* ex )
{
	if ( ex->ExceptionRecord->ExceptionCode == STATUS_SINGLE_STEP )
	{
		HMODULE hModule = NULL;
		GetModuleHandleExA( GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, ( LPCSTR )ex->ExceptionRecord->ExceptionAddress, &hModule );

		char szModule[ MAX_PATH ]{ };
		if ( hModule != NULL )
		{
			GetModuleBaseNameA( ( HANDLE )-1, hModule, szModule, sizeof szModule );

			Tools::SaveToLog( "Breakpoint hit on %s+0x%X (0x%p) - ( Return: 0x%X - 0x%X ) (BaseAddress: 0x%p)\n", szModule, DWORD( ex->ExceptionRecord->ExceptionAddress ) - DWORD( hModule ), ex->ExceptionRecord->ExceptionAddress, *( DWORD* )( ex->ContextRecord->Ebp + 0x4 ), *( DWORD* )( ex->ContextRecord->Esp ), hModule );
		}
		else
		{
			Tools::SaveToLog( "Breakpoint hit on 0x%p - ( Return: 0x%X - 0x%X )\n", ex->ExceptionRecord->ExceptionAddress, *( DWORD* )( ex->ContextRecord->Ebp + 0x4 ), *( DWORD* )( ex->ContextRecord->Esp ) );
		}

		return EXCEPTION_CONTINUE_EXECUTION;
	}
	return EXCEPTION_CONTINUE_SEARCH;
}
*/

BOOL WINAPI DllMain( HMODULE hDll, DWORD dwReason, LPVOID )
{
	VIRTUALIZER_TIGER_RED_START;
	if ( dwReason == DLL_PROCESS_ATTACH )
	{
		Globals::Module::OwnModule = DWORD( hDll );
		Globals::Module::OwnModuleSize = Tools::GetModuleSize( Globals::Module::OwnModule );

#ifdef _DEBUG
		Tools::SaveToLog( "My Module: 0x%X - Size: 0x%X\n", Globals::Module::OwnModule, Globals::Module::OwnModuleSize );
#endif
		SetupInstrumentationHook();
		CreateRemoteThread( ( HANDLE )-1, nullptr, NULL, PTHREAD_START_ROUTINE( StartThread ), nullptr, NULL, nullptr );

		char szPath[ MAX_PATH ]{ };
		if ( GetModuleFileNameA( GetModuleHandleA( NULL ), szPath, sizeof szPath ) )
		{
			std::string filename( szPath );
			std::string cf_path{ };

			try
			{
				const size_t last_slash_idx = filename.rfind( '\\' );
				if ( std::string::npos != last_slash_idx )
					cf_path = filename.substr( 0, last_slash_idx );

				std::filesystem::remove_all( "C:\\CFLog" );

				std::filesystem::remove( cf_path + "\\XIGNCODE\\xigncode.log" );
				std::filesystem::remove_all( cf_path + "\\Report" );
				std::filesystem::remove_all( cf_path + "\\hgwc" );
				std::filesystem::remove_all( cf_path + "\\Log" );
			}
			catch ( ... ) { }
		}
	}
	VIRTUALIZER_TIGER_RED_END;
	return true;
}