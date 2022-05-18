#include "Includes.hpp"
#include "CRC32.hpp"

namespace Tools
{
	std::vector< CachedModule_t > v_cached_modules{ };

	DWORD FindModuleHandle( const DWORD& dwModuleHash, DWORD* dwModuleSize )
	{
		auto hash = []( const std::wstring& str )
		{
			unsigned long hash = 5381;
			for ( size_t i = 0; i < str.size(); ++i )
				hash = 33 * hash + ( unsigned short )str[ i ];
			return hash;
		};

		auto it = std::find( v_cached_modules.begin(), v_cached_modules.end(), dwModuleHash );
		if ( it != v_cached_modules.end() )
		{
			if ( dwModuleSize )
				*dwModuleSize = it->Size;

			return it->Base;
		}

		const auto pPeb = ( PEB* )__readfsdword( 0x30 );
		if ( pPeb )
		{
			for ( auto pListEntry = pPeb->Ldr->InLoadOrderModuleList.Flink;
				pListEntry != &pPeb->Ldr->InLoadOrderModuleList;
				pListEntry = pListEntry->Flink )
			{
				const auto pEntry = CONTAINING_RECORD( pListEntry, LDR_DATA_TABLE_ENTRY, InLoadOrderLinks );

				std::wstring module_name( pEntry->BaseDllName.Buffer );
				std::transform( module_name.begin(), module_name.end(), module_name.begin(), ::towlower );

				if ( hash( module_name ) == dwModuleHash )
				{
					if ( dwModuleSize )
						*dwModuleSize = pEntry->SizeOfImage;

					v_cached_modules.push_back( CachedModule_t( DWORD( pEntry->DllBase ), pEntry->SizeOfImage, dwModuleHash ) );
					return DWORD( pEntry->DllBase );
				}
			}
		}
		return 0;
	}

	DWORD GetCodeHash( DWORD Base )
	{
		IMAGE_DOS_HEADER* dh = reinterpret_cast< IMAGE_DOS_HEADER* >( Base );
		if ( dh->e_magic != IMAGE_DOS_SIGNATURE )
			return 0;

		IMAGE_NT_HEADERS32* nt = reinterpret_cast< IMAGE_NT_HEADERS32* >( Base + dh->e_lfanew );
		if ( nt->Signature != IMAGE_NT_SIGNATURE )
			return 0;

		auto OptHeader = &nt->OptionalHeader;
		if ( IsBadReadPtr( OptHeader, sizeof( PVOID ) ) )
			return 0;

		PBYTE RVA = PBYTE( Base ) + OptHeader->BaseOfCode;
		if ( IsBadReadPtr( RVA, sizeof( PVOID ) ) )
			return 0;

		return CRC32_ProcessSingleBuffer( RVA, OptHeader->SizeOfCode );
	}

	void GetCodeBaseAndSize( DWORD Base, PBYTE* OutBase, DWORD* OutSize )
	{
		IMAGE_DOS_HEADER* dh = reinterpret_cast< IMAGE_DOS_HEADER* >( Base );
		if ( dh->e_magic != IMAGE_DOS_SIGNATURE )
			return;

		IMAGE_NT_HEADERS32* nt = reinterpret_cast< IMAGE_NT_HEADERS32* >( Base + dh->e_lfanew );
		if ( nt->Signature != IMAGE_NT_SIGNATURE )
			return;

		auto OptHeader = &nt->OptionalHeader;
		if ( IsBadReadPtr( OptHeader, sizeof( PVOID ) ) )
			return;

		PBYTE RVA = PBYTE( Base ) + OptHeader->BaseOfCode;
		if ( IsBadReadPtr( RVA, sizeof( PVOID ) ) )
			return;

		if ( OutBase )
			*OutBase = RVA;

		if ( OutSize )
			*OutSize = OptHeader->SizeOfCode;
	}

	uintptr_t FindExportAddress( void* hModule, const char* wAPIName )
	{
#if defined( _WIN32 )   
		unsigned char* lpBase = reinterpret_cast< unsigned char* >( hModule );
		IMAGE_DOS_HEADER* idhDosHeader = reinterpret_cast< IMAGE_DOS_HEADER* >( lpBase );
		if ( idhDosHeader->e_magic == IMAGE_DOS_SIGNATURE )
		{
#if defined( _M_IX86 )  
			IMAGE_NT_HEADERS32* inhNtHeader = reinterpret_cast< IMAGE_NT_HEADERS32* >( lpBase + idhDosHeader->e_lfanew );
#elif defined( _M_AMD64 )  
			IMAGE_NT_HEADERS64* inhNtHeader = reinterpret_cast< IMAGE_NT_HEADERS64* >( lpBase + idhDosHeader->e_lfanew );
#endif  
			if ( inhNtHeader->Signature == IMAGE_NT_SIGNATURE )
			{
				if ( inhNtHeader->OptionalHeader.DataDirectory[ IMAGE_DIRECTORY_ENTRY_EXPORT ].VirtualAddress <= 0 )
					return 0;

				IMAGE_EXPORT_DIRECTORY* iedExportDirectory = reinterpret_cast< IMAGE_EXPORT_DIRECTORY* >( lpBase + inhNtHeader->OptionalHeader.DataDirectory[ IMAGE_DIRECTORY_ENTRY_EXPORT ].VirtualAddress );

				if ( IsBadReadPtr( iedExportDirectory, sizeof( DWORD ) ) )
					return 0;

				for ( auto uiIter = 0ul; uiIter < iedExportDirectory->NumberOfNames; ++uiIter )
				{
					char* szNames = reinterpret_cast< char* >( lpBase + reinterpret_cast< unsigned long* >( lpBase + iedExportDirectory->AddressOfNames )[ uiIter ] );
					if ( !strcmp( szNames, wAPIName ) )
					{
						unsigned short usOrdinal = reinterpret_cast< unsigned short* >( lpBase + iedExportDirectory->AddressOfNameOrdinals )[ uiIter ];
						return reinterpret_cast< uintptr_t >( lpBase + reinterpret_cast< unsigned long* >( lpBase + iedExportDirectory->AddressOfFunctions )[ usOrdinal ] );
					}
				}
			}
		}
#endif  
		return 0;
	}

	byte* FindSignature( byte* pBaseAddress, byte* pbMask, const char* pszMask, size_t nLength )
	{
		auto DataCompare = []( const byte* pData, const byte* mask, const char* cmask, byte chLast, size_t iEnd ) -> bool 
		{
			if ( pData[ iEnd ] != chLast ) return false;
			for ( ; *cmask; ++cmask, ++pData, ++mask ) {
				if ( *cmask == 'x' && *pData != *mask ) {
					return false;
				}
			}

			return true;
		};

		auto iEnd = strlen( pszMask ) - 1;
		auto chLast = pbMask[ iEnd ];

		auto* pEnd = pBaseAddress + nLength - strlen( pszMask );
		for ( ; pBaseAddress < pEnd; ++pBaseAddress ) {
			if ( DataCompare( pBaseAddress, pbMask, pszMask, chLast, iEnd ) ) {
				return pBaseAddress;
			}
		}

		return nullptr;
	}

	bool WorldToScreen( D3DXVECTOR3& vWorld, D3DXVECTOR3* Pos )
	{
		if ( !g_pD3DDevice )
			return false;

		D3DXMATRIX projection, view, world;
		if ( FAILED( g_pD3DDevice->GetTransform( D3DTS_VIEW, &view ) ) )
			return false;

		if ( FAILED( g_pD3DDevice->GetTransform( D3DTS_PROJECTION, &projection ) ) )
			return false;

		if ( FAILED( g_pD3DDevice->GetTransform( D3DTS_WORLD, &world ) ) )
			return false;

		D3DVIEWPORT9 vp = { 0 };
		if ( FAILED( g_pD3DDevice->GetViewport( &vp ) ) )
			return false;

		D3DXVec3Project( Pos, &vWorld, &vp, &projection, &view, &world );

		return( Pos->z < 1.0f );
	}

	void __cdecl SaveToLog( const char* fmt, ... )
	{
#ifdef _DEBUG
		static bool run = false;
		static char szDesktop[ MAX_PATH ]{};

		if ( !run )
		{
			run = true;
			if ( SUCCEEDED( SHGetFolderPathA( NULL, CSIDL_DESKTOP, NULL, 0, szDesktop ) ) )
			{
				strcat_s( szDesktop, XS( "\\cflog.log" ) );
				DeleteFileA( szDesktop );
			}
		}

		va_list arglist;
		char buffer[ 4096 ];

		va_start( arglist, fmt );
		vsprintf_s( buffer, fmt, arglist );
		va_end( arglist );

		FILE* f = nullptr;
		fopen_s( &f, szDesktop, "a" );
		if ( f )
		{
			fprintf_s( f, "%s", buffer );
			fclose( f );
		}
#endif
	}

	void __cdecl SaveToCrashLog( const char* fmt, ... )
	{
#ifdef _DEBUG
		va_list arglist;
		char buffer[ 2048 ];

		va_start( arglist, fmt );
		vsprintf_s( buffer, fmt, arglist );
		va_end( arglist );

		FILE* f = nullptr;
		fopen_s( &f, "C:\\CRASH.log", "a" );
		if ( f )
		{
			fprintf_s( f, "%s", buffer );
			fclose( f );
		}
#endif
	}

	std::string DateToString()
	{
		time_t rawtime;
		struct tm* timeinfo;
		char buffer[ 80 ];

		time( &rawtime );
		timeinfo = localtime( &rawtime );

		strftime( buffer, sizeof( buffer ), XS( "%d-%m-%Y %H:%M:%S" ), timeinfo );

		return std::string( buffer );
	}

	void LeftButtonClick()
	{
		if ( gui.MenuVisible() )
			return;

		if ( GetForegroundWindow() != InputSys::ins().GetMainWindow() )
			return;

		mouse_event( MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0 );
		mouse_event( MOUSEEVENTF_LEFTUP, 0, 0, 0, 0 );
	}

	//VOID GetD3Device( DWORD* dwVTable )
	//{
	//	LPDIRECT3D9 pD3d9;
	//	LPDIRECT3DDEVICE9 pD3DDevice;
	//	pD3d9 = Direct3DCreate9( D3D_SDK_VERSION );
	//	if ( pD3d9 == NULL )
	//		return;
	//	D3DPRESENT_PARAMETERS pPresentParms;
	//	ZeroMemory( &pPresentParms, sizeof( pPresentParms ) );
	//	pPresentParms.Windowed = TRUE;
	//	pPresentParms.BackBufferFormat = D3DFMT_UNKNOWN;
	//	pPresentParms.SwapEffect = D3DSWAPEFFECT_DISCARD;
	//	if ( FAILED( pD3d9->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, GetDesktopWindow(), D3DCREATE_SOFTWARE_VERTEXPROCESSING, &pPresentParms, &pD3DDevice ) ) )
	//		return;
	//	DWORD* dwTable = ( DWORD* )pD3DDevice;
	//	dwTable = ( DWORD* )dwTable[ 0 ];
	//	dwVTable[ 0 ] = dwTable[ 16 ];//Reset
	//	dwVTable[ 1 ] = dwTable[ 17 ];//Present
	//	dwVTable[ 2 ] = dwTable[ 82 ];//DIP
	//}
};