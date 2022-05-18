#pragma once

#include <cstdint>
#include <string>
#include <algorithm>
#include <locale>
#include <codecvt>

#define INRANGE(x,a,b)		(x >= a && x <= b) 
#define getBits( x )		(INRANGE(x,'0','9') ? (x - '0') : ((x&(~0x20)) - 'A' + 0xa))
#define getByte( x )		(getBits(x[0]) << 4 | getBits(x[1]))

namespace Tools
{
	struct CachedModule_t
	{
		CachedModule_t( DWORD Base, DWORD Size, DWORD ModuleHash )
		{
			this->Base = Base;
			this->Size = Size;
			this->ModuleHash = ModuleHash;
		}

		DWORD Base;
		DWORD Size;
		DWORD ModuleHash;

		bool operator==( const DWORD& ModuleHash )
		{
			return this->ModuleHash == ModuleHash;
		}
	};

	extern std::vector< CachedModule_t > v_cached_modules;

	DWORD FindModuleHandle( const DWORD& dwModuleHash, DWORD* dwModuleSize = NULL );
	uintptr_t FindExportAddress( void* hModule, const char* wAPIName );
	DWORD GetCodeHash( DWORD Base );
	void GetCodeBaseAndSize( DWORD Base, PBYTE* OutBase, DWORD* OutSize );

	void __cdecl SaveToLog( const char* fmt, ... );
	void __cdecl SaveToCrashLog( const char* fmt, ... );

	void MoveToRecycle( const std::string& szFilePath );

	bool WorldToScreen( D3DXVECTOR3& vWorld, D3DXVECTOR3* Pos );

	byte* FindSignature( byte* pBaseAddress, byte* pbMask, const char* pszMask, size_t nLength );

	template< typename T >
	T FindPattern( const DWORD& dwModuleHash, PBYTE Signature, const char* Mask )
	{
		DWORD dwModuleSize = 0;
		DWORD dwModule = 0;

		if ( ( dwModule = FindModuleHandle( dwModuleHash, &dwModuleSize ) ) != 0 && dwModuleSize != 0 )
		{
			return reinterpret_cast< T >( FindSignature( reinterpret_cast< PBYTE >( dwModule ), Signature, Mask, dwModuleSize ) );
		}
		return 0;
	}

	static inline void memcpy_s( DWORD addr, LPCVOID data, size_t size )
	{
		SYSTEM_INFO si{ };
		GetNativeSystemInfo( &si );

		DWORD dwOld = 0;
		if ( VirtualProtect( PVOID( addr ), si.dwPageSize, PAGE_EXECUTE_READWRITE, &dwOld ) )
		{
			::memcpy( PVOID( addr ), data, size );
			VirtualProtect( PVOID( addr ), si.dwPageSize, dwOld, &dwOld );
			FlushInstructionCache( ( HANDLE )-1, PVOID( addr ), si.dwPageSize );
		}
	}

	inline void EraseMyHeader( PBYTE hDll )
	{
		PIMAGE_DOS_HEADER pDosHeader = ( PIMAGE_DOS_HEADER )hDll;

		if ( pDosHeader->e_magic != IMAGE_DOS_SIGNATURE )
			return;

		PIMAGE_NT_HEADERS pNTHeader = ( PIMAGE_NT_HEADERS )( ( DWORD )pDosHeader + ( DWORD )pDosHeader->e_lfanew );

		if ( pNTHeader->Signature != IMAGE_NT_SIGNATURE )
			return;

		const auto header_size = pNTHeader->OptionalHeader.SizeOfHeaders;
		if ( header_size > 0 )
		{
			srand( GetTickCount() );

			DWORD dwOld = NULL;
			VirtualProtect( hDll, header_size, PAGE_EXECUTE_READWRITE, &dwOld );

			RtlSecureZeroMemory( hDll, header_size );
			//for (auto i = 0ul; i < header_size; ++i)
			//	*(BYTE *)(hDll + i) = rand() % 254;

			VirtualProtect( hDll, header_size, dwOld, &dwOld );
		}
	}

	inline DWORD ResolveRelative( DWORD sig1 )
	{
		return sig1 = sig1 + *reinterpret_cast< PDWORD >( sig1 + 1 ) + 5;
	}

	inline DWORD NtRand( ULONG Seed )
	{
		srand( Seed );
		return 1 + rand() % 254;
	}

	void LeftButtonClick();

	static inline void MouseMove( float x, float y )
	{
		INPUT Input = { 0 };

		::ZeroMemory( &Input, sizeof( INPUT ) );
		Input.type = INPUT_MOUSE;
		Input.mi.dx = ( LONG )x;
		Input.mi.dy = ( LONG )y;
		Input.mi.dwFlags = MOUSEEVENTF_MOVE;
		SendInput( 1, &Input, sizeof( INPUT ) );
	}

	static inline PVOID ReplaceTable( PDWORD* dwVTable, PBYTE dwHook, int Index )
	{
		DWORD dwOld = 0;
		VirtualProtect( ( void* )( ( *dwVTable ) + ( Index * 4 ) ), 4, PAGE_EXECUTE_READWRITE, &dwOld );

		PBYTE pOrig = ( ( PBYTE )( *dwVTable )[ Index ] );
		( *dwVTable )[ Index ] = ( DWORD )dwHook;

		VirtualProtect( ( void* )( ( *dwVTable ) + ( Index * 4 ) ), 4, dwOld, &dwOld );

#ifdef _DEBUG
		printf( "\n=== VMT HOOK ===\n" );
		printf( "Entry Location: %p\n", ( void* )( ( *dwVTable ) + ( Index * 4 ) ) );
		printf( "Original: %p\n\n", pOrig );
#endif

		return pOrig;
	}

	static inline DWORD GetModuleSize( DWORD Module )
	{
		PIMAGE_DOS_HEADER pDosHeader = PIMAGE_DOS_HEADER( Module );

		if ( pDosHeader->e_magic != IMAGE_DOS_SIGNATURE )
		{
			Module += 4096;

			MEMORY_BASIC_INFORMATION mbi{ };
			if ( VirtualQuery( PVOID( Module ), &mbi, sizeof mbi ) )
				return mbi.RegionSize;

			return 0;
		}

		PIMAGE_NT_HEADERS pNTHeader = PIMAGE_NT_HEADERS( Module + pDosHeader->e_lfanew );

		if ( pNTHeader->Signature != IMAGE_NT_SIGNATURE )
			return 0;

		if ( pNTHeader->FileHeader.SizeOfOptionalHeader <= 0 )
			return 0;

		return pNTHeader->OptionalHeader.SizeOfImage; 
	}

	std::string DateToString();
	std::string ReadRemoteRuntimeTypeInformation( DWORD address );
	VOID GetD3Device( DWORD* dwVTable );
};