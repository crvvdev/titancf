#include "Includes.hpp"

#define LODWORD(l) ((uint32_t)(((uint64_t)(l)) & 0xffffffff))
#define HIDWORD(l) ((uint32_t)((((uint64_t)(l)) >> 32) & 0xffffffff))

inline bool IsSuspeciousMemory( MEMORY_BASIC_INFORMATION* mbi )
{
	//if ( DWORD( mbi->AllocationBase ) == Globals::mapped_ntdll )
	//	return false;

	if ( ( mbi->State & MEM_COMMIT ) && ( ( mbi->Protect & PAGE_EXECUTE_READWRITE ) || ( mbi->Protect & PAGE_EXECUTE_READ ) || ( mbi->Protect & PAGE_EXECUTE ) ) && !( mbi->Protect & PAGE_NOACCESS ) && !( mbi->Protect & PAGE_GUARD ) && !( mbi->State & MEM_RELEASE ) )
	{
		PIMAGE_DOS_HEADER pDOSHeader = PIMAGE_DOS_HEADER( mbi->AllocationBase );

		return ( pDOSHeader->e_magic != IMAGE_DOS_SIGNATURE );
	}

	return false;
}

NtQueryVirtualMemory2_ Old_NtQueryVirtualMemory2 = NULL;

NTSTATUS NTAPI hkNtQueryVirtualMemory2( HANDLE ProcessHandle, DWORD BaseAddress, MEMORY_INFORMATION_CLASS Type, PVOID Out, ULONG Length, ULONG* NumberOfBytesRead )
{
	const auto Temp = Old_NtQueryVirtualMemory2( ProcessHandle, BaseAddress, Type, Out, Length, NumberOfBytesRead );

	if ( NT_SUCCESS( Temp ) )
	{
		if ( ProcessHandle == GetCurrentProcess() || GetProcessId( ProcessHandle ) == GetCurrentProcessId() )
		{
			if ( Type == MemoryBasicInformation )
			{
				MEMORY_BASIC_INFORMATION* mbiOut = ( MEMORY_BASIC_INFORMATION* )Out;

				if ( BaseAddress >= Globals::Module::OwnModule && BaseAddress <= Globals::Module::OwnModule + Globals::Module::OwnModuleSize )
				{
#ifdef _DEBUG
					Tools::SaveToLog( "[VQEX] Query on my module from: %p\n", _ReturnAddress() );
#endif
					mbiOut->AllocationBase = nullptr;
					mbiOut->AllocationProtect = PAGE_NOACCESS;
					mbiOut->RegionSize = Globals::Module::OwnModuleSize;
					mbiOut->State = MEM_FREE;
					mbiOut->Protect = PAGE_NOACCESS;
					mbiOut->Type = 0;
				}
			}

			if ( Type == MemorySectionName )
			{
				if ( BaseAddress >= Globals::Module::OwnModule && BaseAddress <= Globals::Module::OwnModule + Globals::Module::OwnModuleSize )
				{
					PUNICODE_STRING SectionName = ( PUNICODE_STRING )Out;
					if ( SectionName )
					{
						memset( SectionName->Buffer, 0, SectionName->MaximumLength );
					}
					return STATUS_UNSUCCESSFUL;
				}
			}
		}
	}
	return Temp;
}

NtQueryVirtualMemory_ Old_NtQueryVirtualMemory = NULL;

NTSTATUS NTAPI hkNtQueryVirtualMemory( HANDLE ProcessHandle, DWORD BaseAddress, MEMORY_INFORMATION_CLASS Type, PVOID Out, ULONG Length, ULONG* NumberOfBytesRead )
{
	const auto Temp = Old_NtQueryVirtualMemory( ProcessHandle, BaseAddress, Type, Out, Length, NumberOfBytesRead );

	if ( NT_SUCCESS( Temp ) )
	{
		if ( ProcessHandle == GetCurrentProcess() || GetProcessId( ProcessHandle ) == GetCurrentProcessId() )
		{
			if ( Type == MemoryBasicInformation )
			{
				MEMORY_BASIC_INFORMATION* mbiOut = ( MEMORY_BASIC_INFORMATION* )Out;

				if ( BaseAddress >= Globals::Module::OwnModule && BaseAddress <= Globals::Module::OwnModule + Globals::Module::OwnModuleSize + Globals::Module::OwnModuleSize ) //|| IsSuspeciousMemory( mbiOut ) )
				{
#ifdef _DEBUG
					Tools::SaveToLog( "[QVM] Query on my module from: %p\n", _ReturnAddress() );
#endif
					mbiOut->AllocationBase = nullptr;
					mbiOut->AllocationProtect = PAGE_NOACCESS;
					mbiOut->RegionSize = Globals::Module::OwnModuleSize;
					mbiOut->State = MEM_FREE;
					mbiOut->Protect = PAGE_NOACCESS;
					mbiOut->Type = 0;
					return STATUS_INVALID_PARAMETER;
				}
			}

			if ( Type == MemorySectionName )
			{
				if ( BaseAddress >= Globals::Module::OwnModule && BaseAddress <= Globals::Module::OwnModule + Globals::Module::OwnModuleSize )
				{
					PUNICODE_STRING SectionName = ( PUNICODE_STRING )Out;
					if ( SectionName )
					{
						memset( SectionName->Buffer, 0, SectionName->MaximumLength );
					}
					return STATUS_UNSUCCESSFUL;
				}
			}
		}
	}
	return Temp;
}

_VirtualQuery Old_VirtualQuery = NULL;

SIZE_T WINAPI hkVirtualQuery( DWORD lpAddress, PMEMORY_BASIC_INFORMATION lpBuffer, SIZE_T dwLength )
{
	SIZE_T shit = Old_VirtualQuery( lpAddress, lpBuffer, dwLength );

	if ( lpAddress >= Globals::Module::OwnModule && lpAddress <= Globals::Module::OwnModule + Globals::Module::OwnModuleSize )
	{
		lpBuffer->AllocationBase = 0;
		lpBuffer->AllocationProtect = PAGE_NOACCESS;
		lpBuffer->RegionSize = Globals::Module::OwnModuleSize;
		lpBuffer->Protect = PAGE_NOACCESS;
		lpBuffer->Type = 0;
		lpBuffer->State = MEM_FREE;
		SetLastError( ERROR_INVALID_PARAMETER );
		return 0;
	}
	return shit;
}

_ZwProtectVirtualMemory Old_ZwProtectVirtualMemory = NULL;

NTSTATUS NTAPI hkZwProtectVirtualMemory( IN HANDLE ProcessHandle, IN PVOID* BaseAddress, IN SIZE_T* NumberOfBytesToProtect, IN ULONG NewAccessProtection, OUT PULONG OldAccessProtection )
{
	if ( ProcessHandle == GetCurrentProcess() || GetProcessId( ProcessHandle ) == GetCurrentProcessId() )
	{
		if ( BaseAddress && DWORD( *BaseAddress ) >= Globals::Module::OwnModule && DWORD( *BaseAddress ) <= Globals::Module::OwnModule + Globals::Module::OwnModuleSize )
			return STATUS_UNSUCCESSFUL;
	}
	return Old_ZwProtectVirtualMemory( ProcessHandle, BaseAddress, NumberOfBytesToProtect, NewAccessProtection, OldAccessProtection );
}

_ZwQueryInformationThread Old_ZwQueryInformationThread = NULL;

NTSTATUS NTAPI hkZwQueryInformationThread( IN HANDLE ThreadHandle, IN THREADINFOCLASS ThreadInformationClass, PVOID ThreadInformation, _In_ ULONG ThreadInformationLength, _Out_opt_ PULONG ReturnLength )
{
	if ( ThreadQuerySetWin32StartAddress == ThreadInformationClass )
	{
		if ( DWORD( ThreadInformation ) >= Globals::Module::OwnModule && DWORD( ThreadInformation ) <= Globals::Module::OwnModule + Globals::Module::OwnModuleSize )
		{
#ifdef _DEBUG
			Tools::SaveToLog( "[QIT] Query on my module from: %p\n", _ReturnAddress() );
#endif
			ThreadInformation = NULL;
			return STATUS_UNSUCCESSFUL;
		}
	}
	return Old_ZwQueryInformationThread( ThreadHandle, ThreadInformationClass, ThreadInformation, ThreadInformationLength, ReturnLength );
}

_ZwReadVirtualMemory Old_ZwReadVirtualMemory = NULL;

NTSTATUS NTAPI hkZwReadVirtualMemory( IN HANDLE ProcessHandle, IN PVOID BaseAddress, OUT PVOID Buffer, IN ULONG NumberOfBytesToRead, OUT PULONG NumberOfBytesReaded OPTIONAL )
{
	if ( ProcessHandle == GetCurrentProcess() || GetProcessId( ProcessHandle ) == GetCurrentProcessId() )
	{
		if ( DWORD( BaseAddress ) >= Globals::Module::OwnModule && DWORD( BaseAddress ) <= Globals::Module::OwnModule + Globals::Module::OwnModuleSize )
		{
#ifdef _DEBUG
			Tools::SaveToLog( "[RVM] Query on my module from: %p\n", _ReturnAddress() );
#endif
			return STATUS_UNSUCCESSFUL;
		}
	}
	return Old_ZwReadVirtualMemory( ProcessHandle, BaseAddress, Buffer, NumberOfBytesToRead, NumberOfBytesReaded );
}

using NtWow64QueryVirtualMemory64_ = NTSTATUS( NTAPI* )( HANDLE, PVOID64, DWORD, PVOID, ULONG64, PULONG64 );
NtWow64QueryVirtualMemory64_ oNtWow64QueryVirtualMemory64 = NULL;

NTSTATUS NTAPI hkNtWow64QueryVirtualMemory64( HANDLE ProcessHandle, PVOID64 BaseAddress, DWORD MemoryInformationClass, PVOID Buffer, ULONG64 Length, PULONG64 ResultLength )
{
	const auto res = oNtWow64QueryVirtualMemory64( ProcessHandle, BaseAddress, MemoryInformationClass, Buffer, Length, ResultLength );

	if ( !NT_SUCCESS( res ) )
		return res;

	if ( ProcessHandle == GetCurrentProcess() || GetProcessId( ProcessHandle ) == GetCurrentProcessId() )
	{
		if ( MemoryInformationClass == MemoryBasicInformation )
		{
			MEMORY_BASIC_INFORMATION* mbiOut = ( MEMORY_BASIC_INFORMATION* )Buffer;

			if ( LODWORD( BaseAddress ) >= Globals::Module::OwnModule && LODWORD( BaseAddress ) <= Globals::Module::OwnModule + Globals::Module::OwnModuleSize ||
				HIDWORD( BaseAddress ) >= Globals::Module::OwnModule && HIDWORD( BaseAddress ) <= Globals::Module::OwnModule + Globals::Module::OwnModuleSize )
			{
#ifdef _DEBUG
				Tools::SaveToLog( "[VQEX_Wow64] Query on my module from: %p\n", _ReturnAddress() );
#endif
				mbiOut->AllocationBase = nullptr;
				mbiOut->AllocationProtect = PAGE_NOACCESS;
				mbiOut->RegionSize = Globals::Module::OwnModuleSize;
				mbiOut->State = MEM_FREE;
				mbiOut->Protect = PAGE_NOACCESS;
				mbiOut->Type = 0;
				return STATUS_INVALID_PARAMETER;
			}
		}

		if ( MemoryInformationClass == MemorySectionName )
		{
			if ( LODWORD( BaseAddress ) >= Globals::Module::OwnModule && LODWORD( BaseAddress ) <= Globals::Module::OwnModule + Globals::Module::OwnModuleSize ||
				HIDWORD( BaseAddress ) >= Globals::Module::OwnModule && HIDWORD( BaseAddress ) <= Globals::Module::OwnModule + Globals::Module::OwnModuleSize )
			{
				PUNICODE_STRING SectionName = ( PUNICODE_STRING )Buffer;
				if ( SectionName )
				{
					memset( SectionName->Buffer, 0, SectionName->MaximumLength );
				}
				return STATUS_UNSUCCESSFUL;
			}
		}
	}

	return res;
}

using NtWow64ReadVirtualMemory64_ = NTSTATUS( NTAPI* )( HANDLE, PVOID64, PVOID, ULONG64, PULONG64 );
NtWow64ReadVirtualMemory64_ oNtWow64ReadVirtualMemory64 = NULL;

NTSTATUS NTAPI hkNtWow64ReadVirtualMemory64( HANDLE ProcessHandle, PVOID64 BaseAddress, PVOID Buffer, ULONG64 Size, PULONG64 NumberOfBytesRead )
{
	if ( ProcessHandle == GetCurrentProcess() || GetProcessId( ProcessHandle ) == GetCurrentProcessId() )
	{
		if ( LODWORD( BaseAddress ) >= Globals::Module::OwnModule && LODWORD( BaseAddress ) <= Globals::Module::OwnModule + Globals::Module::OwnModuleSize ||
			HIDWORD( BaseAddress ) >= Globals::Module::OwnModule && HIDWORD( BaseAddress ) <= Globals::Module::OwnModule + Globals::Module::OwnModuleSize )
		{
#ifdef _DEBUG
			Tools::SaveToLog( "[RVM_Wow64] Query on my module from: %p\n", _ReturnAddress() );
#endif
			return STATUS_UNSUCCESSFUL;
		}
	}
	return oNtWow64ReadVirtualMemory64( ProcessHandle, BaseAddress, Buffer, Size, NumberOfBytesRead );
}

struct DBG_CONTEXT
{
	uintptr_t eflags;
	uintptr_t edi;
	uintptr_t esi;
	uintptr_t ebp;
	uintptr_t esp;
	uintptr_t ebx;
	uintptr_t edx;
	uintptr_t ecx;
	uintptr_t eax;
	uintptr_t eip;
};

bool flag = false;
DWORD desired = NULL, desired2 = NULL;

void __stdcall my_routine( DBG_CONTEXT* ctx )
{
	if ( !flag )
	{
		flag = true;

		/*uint8_t buffer[ sizeof( SYMBOL_INFO ) + MAX_SYM_NAME ] = { 0 };
		const auto symbol_info = ( PSYMBOL_INFO )buffer;
		symbol_info->SizeOfStruct = sizeof( SYMBOL_INFO );
		symbol_info->MaxNameLen = MAX_SYM_NAME;
		DWORD64 displacement;

		const auto result = SymFromAddr( GetCurrentProcess(), DWORD64( ctx->ecx ), &displacement, symbol_info );
		if ( result && DWORD( symbol_info->Address ) == desired )
		{*/
		const auto function = ctx->ecx - 12;
		if ( function == desired && ctx->esp != NULL )
		{
			HANDLE ProcessHandle = *( HANDLE* )( ctx->esp + 4 );
			DWORD BaseAddress = *( DWORD* )( ctx->esp + 8 );
			MEMORY_BASIC_INFORMATION* Out = *( MEMORY_BASIC_INFORMATION** )( ctx->esp + 16 );

			if ( ProcessHandle == GetCurrentProcess() && ( BaseAddress >= Globals::Module::OwnModule && BaseAddress <= Globals::Module::OwnModule + Globals::Module::OwnModuleSize ) )
			{
				//Tools::SaveToLog( "Query on my module from: 0x%X\n", *( DWORD* )( ctx->ebp + 0x4 ) );
				Out->AllocationBase = nullptr;
				Out->AllocationProtect = PAGE_NOACCESS;
				Out->RegionSize = Globals::Module::OwnModuleSize;
				Out->State = MEM_FREE;
				Out->Protect = PAGE_NOACCESS;
				Out->Type = 0;
				ctx->eax = 0xC000000D;
			}
		}

		/*if ( function == desired2 )
		{
			HANDLE ProcessHandle = *( HANDLE* )( ctx->esp + 4 );
			PVOID* BaseAddress = *( PVOID** )( ctx->esp + 8 );
			PSIZE_T RegionSize = *( PSIZE_T* )( ctx->esp + 16 );
			ULONG AllocationType = *( ULONG* )( ctx->esp + 20 );
			ULONG Protect = *( ULONG* )( ctx->esp + 24 );
			PVOID RetAddr = *( PVOID* )( ctx->ebp + 0x4 );

			if ( ProcessHandle == GetCurrentProcess() && ( AllocationType & MEM_RESERVE || AllocationType & MEM_COMMIT ) && ( Protect & PAGE_EXECUTE_READWRITE || Protect & PAGE_READWRITE ) && *RegionSize > 0x1000 )
			{
				PVOID base = nullptr;
				RtlPcToFileHeader( RetAddr, &base );

				if( !base )
					Tools::SaveToLog( "Memory will be allocated at: 0x%p ( 0x%X ) | RtnAddr: 0x%p\n", *BaseAddress, *RegionSize, RetAddr );
			}
		}*/

		flag = false;
	}
}

__declspec( naked ) void hk_instrumentation()
{
	__asm
	{
		pushad
		pushfd

		push esp
		call my_routine

		popfd
		popad
		jmp ecx
	}
}

namespace IAT
{
	void** find( const char* function, HMODULE module )
	{
		if ( !module )
			module = GetModuleHandle( 0 );

		PIMAGE_DOS_HEADER img_dos_headers = ( PIMAGE_DOS_HEADER )module;
		PIMAGE_NT_HEADERS img_nt_headers = ( PIMAGE_NT_HEADERS )( PBYTE( module ) + img_dos_headers->e_lfanew );

		const auto iat_va = img_nt_headers->OptionalHeader.DataDirectory[ IMAGE_DIRECTORY_ENTRY_IMPORT ].VirtualAddress;
		if ( iat_va <= 0 )
		{
#ifdef _DEBUG
			Tools::SaveToLog( "[IAT] VA: 0x%X - fail.\n", iat_va );
#endif
			return 0;
		}

		PIMAGE_IMPORT_DESCRIPTOR img_import_desc = ( PIMAGE_IMPORT_DESCRIPTOR )( PBYTE( module ) + iat_va );
		if ( IsBadReadPtr( img_import_desc, sizeof( PVOID ) ) )
		{
#ifdef _DEBUG
			Tools::SaveToLog( "[IAT] BadReadPtr at 0x%X - fail.\n", img_import_desc );
#endif
			return 0;
		}

		for ( IMAGE_IMPORT_DESCRIPTOR* iid = img_import_desc; iid->Name != 0; iid++ )
		{
			for ( int func_idx = 0; *( func_idx + ( void** )( iid->FirstThunk + ( size_t )module ) ) != nullptr; func_idx++ )
			{
				char* mod_func_name = ( char* )( *( func_idx + ( size_t* )( iid->OriginalFirstThunk + ( size_t )module ) ) + ( size_t )module + 2 );
				const intptr_t nmod_func_name = ( intptr_t )mod_func_name;
				if ( nmod_func_name >= 0 )
				{
					if ( !::strcmp( function, mod_func_name ) )
					{
#ifdef _DEBUG
						Tools::SaveToLog( "[IAT] Found procedure: %s\n", mod_func_name );
#endif
						return func_idx + ( void** )( iid->FirstThunk + ( size_t )module );
					}
				}
			}
		}

		return 0;
	}

	uintptr_t detour_iat_ptr( const char* function, void* newfunction, HMODULE module = 0 )
	{
		auto&& func_ptr = find( function, module );
		if ( *func_ptr == newfunction || *func_ptr == nullptr )
			return 0;

		DWORD old_rights, new_rights = PAGE_READWRITE;
		VirtualProtect( func_ptr, sizeof( uintptr_t ), new_rights, &old_rights );
		uintptr_t ret = ( uintptr_t )*func_ptr;
		*func_ptr = newfunction;
#ifdef _DEBUG
		Tools::SaveToLog( "[IAT] Set old procedure from %p to %p\n", ret, newfunction );
#endif
		VirtualProtect( func_ptr, sizeof( uintptr_t ), old_rights, &new_rights );
		return ret;
	}
};

void __stdcall FindXignNtdll( PVOID )
{
	SYSTEM_INFO si{ };
	GetNativeSystemInfo( &si );
	bool found = false;

	while ( !found )
	{
		MEMORY_BASIC_INFORMATION mi{ };
		for (
			DWORD address = NULL;
			VirtualQuery( PVOID( address ), &mi, sizeof mi ) > 0;
			address += si.dwPageSize )
		{
			if ( address >= DWORD( si.lpMaximumApplicationAddress ) )
				break;

			if ( ( mi.State & MEM_COMMIT ) && ( ( mi.Protect & PAGE_READONLY ) || ( mi.Protect & PAGE_READWRITE ) || ( mi.Protect & PAGE_EXECUTE_READWRITE ) || ( mi.Protect & PAGE_EXECUTE_READ ) ) && !( mi.Protect & PAGE_NOACCESS ) && !( mi.Protect & PAGE_GUARD ) && !( mi.State & MEM_RELEASE ) )
			{
				PVOID base = nullptr;
				RtlPcToFileHeader( PVOID( mi.BaseAddress ), &base );

				if ( !base )
				{
					PIMAGE_DOS_HEADER dh = PIMAGE_DOS_HEADER( PBYTE( mi.BaseAddress ) );
					if ( dh->e_magic != IMAGE_DOS_SIGNATURE )
						continue;

					PIMAGE_NT_HEADERS32 nt = PIMAGE_NT_HEADERS32( PBYTE( mi.BaseAddress ) + dh->e_lfanew );
					if ( nt->Signature != IMAGE_NT_SIGNATURE )
						continue;

					if ( nt->FileHeader.Machine != IMAGE_FILE_MACHINE_I386 )
						continue;

					BOOL wow64 = FALSE;
					if ( IsWow64Process( ( HANDLE )-1, &wow64 ) && wow64 )
					{
						auto _Expt1 = Tools::FindExportAddress( PVOID( mi.BaseAddress ), XS( "NtWow64QueryVirtualMemory64" ) );
						if ( _Expt1 )
						{
#ifdef _DEBUG
							Tools::SaveToLog( "(( NtWow64QueryVirtualMemory64: 0x%X ))\n", _Expt1 );
#endif
							oNtWow64QueryVirtualMemory64 = ( NtWow64QueryVirtualMemory64_ )
								CreateHook2( PBYTE( _Expt1 ), PBYTE( &hkNtWow64QueryVirtualMemory64 ) );
						}

						auto _Expt2 = Tools::FindExportAddress( PVOID( mi.BaseAddress ), XS( "NtWow64ReadVirtualMemory64" ) );
						if ( _Expt2 )
						{
#ifdef _DEBUG
							Tools::SaveToLog( "(( NtWow64ReadVirtualMemory64: 0x%X ))\n", _Expt2 );
#endif
							oNtWow64ReadVirtualMemory64 = ( NtWow64ReadVirtualMemory64_ )
								CreateHook2( PBYTE( _Expt2 ), PBYTE( &hkNtWow64ReadVirtualMemory64 ) );
						}
					}

					auto Expt1 = Tools::FindExportAddress( PVOID( mi.BaseAddress ), XS( "NtQueryVirtualMemory" ) );
					if ( Expt1 )
					{
#ifdef _DEBUG
						Tools::SaveToLog( "(( NtQueryVirtualMemory: 0x%X ))\n", Expt1 );
#endif
						Old_NtQueryVirtualMemory = ( NtQueryVirtualMemory_ )
							CreateHook2( PBYTE( Expt1 ), PBYTE( &hkNtQueryVirtualMemory ) );
					}

					auto Expt2 = Tools::FindExportAddress( PVOID( mi.BaseAddress ), XS( "NtQueryInformationThread" ) );
					if ( Expt2 )
					{
#ifdef _DEBUG
						Tools::SaveToLog( "(( NtQueryInformationThread: 0x%X ))\n", Expt1 );
#endif
						Old_ZwQueryInformationThread = ( _ZwQueryInformationThread )
							CreateHook2( PBYTE( Expt2 ), PBYTE( &hkZwQueryInformationThread ) );
					}

					auto Expt3 = Tools::FindExportAddress( PVOID( mi.BaseAddress ), XS( "NtProtectVirtualMemory" ) );
					if ( Expt3 )
					{
#ifdef _DEBUG
						Tools::SaveToLog( "(( NtProtectVirtualMemory: 0x%X ))\n", Expt3 );
#endif
						Old_ZwProtectVirtualMemory = ( _ZwProtectVirtualMemory )
							CreateHook2( PBYTE( Expt3 ), PBYTE( &hkZwProtectVirtualMemory ) );
					}

					auto Expt4 = Tools::FindExportAddress( PVOID( mi.BaseAddress ), XS( "NtReadVirtualMemory" ) );
					if ( Expt4 )
					{
#ifdef _DEBUG
						Tools::SaveToLog( "(( NtReadVirtualMemory: 0x%X ))\n", Expt4 );
#endif
						Old_ZwReadVirtualMemory = ( _ZwReadVirtualMemory )
							CreateHook2( PBYTE( Expt4 ), PBYTE( &hkZwReadVirtualMemory ) );
					}

					if ( Expt1 && Expt2 && Expt3 && Expt4 )
					{
#ifdef _DEBUG
						Tools::SaveToLog( "(( ALL HOOKS SETUP ))\n", Expt4 );
#endif
						found = true;
						break;
					}
				}
			}
		}
		Sleep( 2500 );
	}
}

#ifdef _DEBUG
LONG WINAPI PageGuardTest( EXCEPTION_POINTERS* ex )
{
	if ( ex->ExceptionRecord->ExceptionCode == STATUS_GUARD_PAGE_VIOLATION )
	{
		for ( DWORD i = 0; i < ex->ContextRecord->Eax; ++i )
		{
			PBYTE Search = PBYTE( ex->ContextRecord->Esi );

			if ( Search[ i ] == 0x4D && Search[ i + 1 ] == 0x5A )
			{
				Tools::SaveToLog( "Found dos signature at: %p\n", ex->ContextRecord->Edi );
			}
		}
		return EXCEPTION_CONTINUE_EXECUTION;
	}

	return EXCEPTION_CONTINUE_SEARCH;
}
#endif

//_VirtualQueryEx Old_VirtualQueryEx = NULL;

using ONtQueryVirtualMemory = NTSTATUS( __stdcall* ) ( HANDLE ProcessHandle, PVOID Address, MEMORY_INFORMATION_CLASS VirtualMemoryInformationClass, PVOID VirtualMemoryInformation, SIZE_T Length, PSIZE_T ResultLength );
ONtQueryVirtualMemory FnNtQueryVirtualMemory = NULL;

PBYTE hkVQEX = nullptr;

SIZE_T WINAPI hkVirtualQueryEx( HANDLE hProcess, DWORD lpAddress, PMEMORY_BASIC_INFORMATION lpBuffer, SIZE_T dwLength )
{
	if ( hProcess == GetCurrentProcess() && lpAddress >= Globals::Module::OwnModule && lpAddress <= Globals::Module::OwnModule + Globals::Module::OwnModuleSize )
	{
#ifdef _DEBUG
		Tools::SaveToLog( "[VQ] Query on my module from: %p\n", _ReturnAddress() );
#endif
		lpBuffer->AllocationBase = 0;
		lpBuffer->AllocationProtect = PAGE_NOACCESS;
		lpBuffer->RegionSize = Globals::Module::OwnModuleSize;
		lpBuffer->Protect = PAGE_NOACCESS;
		lpBuffer->Type = 0;
		lpBuffer->State = MEM_FREE;
		SetLastError( ERROR_INVALID_PARAMETER );
		return 0;
	}

	SIZE_T ResultLength = NULL;
	FnNtQueryVirtualMemory( hProcess, ( PVOID )lpAddress, MemoryBasicInformation, lpBuffer, dwLength, &ResultLength );
	return ResultLength;
}

LONG WINAPI ExpHook( PEXCEPTION_POINTERS ex )
{
	if ( ex->ExceptionRecord->ExceptionCode == EXCEPTION_BREAKPOINT )
	{
		if ( hkVQEX && ex->ExceptionRecord->ExceptionAddress == hkVQEX )
		{
			ex->ContextRecord->Eip = ( DWORD )&hkVirtualQueryEx;
			return EXCEPTION_CONTINUE_EXECUTION;
		}
	}
	return EXCEPTION_CONTINUE_SEARCH;
}

void SetXignBreakpoint()
{
	HANDLE hThreadSnap = INVALID_HANDLE_VALUE;
	THREADENTRY32 te32;
	hThreadSnap = CreateToolhelp32Snapshot( TH32CS_SNAPTHREAD, 0 );

	if ( hThreadSnap )
	{
		te32.dwSize = sizeof( THREADENTRY32 );

		if ( !Thread32First( hThreadSnap, &te32 ) )
		{
			CloseHandle( hThreadSnap );
			return;
		}

		do
		{
			if ( te32.th32OwnerProcessID == GetCurrentProcessId() && te32.th32ThreadID != GetCurrentThreadId() ) //Ignore threads from other processes AND the own thread of course
			{
				HANDLE hThread = OpenThread( THREAD_GET_CONTEXT | THREAD_SET_CONTEXT | THREAD_SUSPEND_RESUME, 0, te32.th32ThreadID );
				if ( hThread )
				{
					CONTEXT context;
					context.ContextFlags = CONTEXT_DEBUG_REGISTERS;
					SuspendThread( hThread ); //Suspend the thread so we can safely set a breakpoint

					if ( GetThreadContext( hThread, &context ) )
					{
						context.Dr0 = ( DWORD )hkVQEX; //Dr0 - Dr3 contain the address you want to break at
						context.Dr1 = 0; //You dont have to set them all
						context.Dr2 = 0;
						context.Dr3 = 0;

						//Those flags activate the breakpoints set in Dr0 - Dr3
						//You can either set break on: EXECUTE, WRITE or ACCESS
						//The Flags I'm using represent the break on execute
						context.Dr7 = ( 1 << 0 ) | ( 1 << 2 ) | ( 1 << 4 );

						SetThreadContext( hThread, &context );
					}

					ResumeThread( hThread );
					CloseHandle( hThread );
				}
			}
		} while ( Thread32Next( hThreadSnap, &te32 ) );
		CloseHandle( hThreadSnap );
	}
}

__declspec( noinline ) void SetupInstrumentationHook()
{
	CreateRemoteThread( ( HANDLE )-1, nullptr, NULL, PTHREAD_START_ROUTINE( FindXignNtdll ), nullptr, NULL, nullptr );

	/*AddVectoredExceptionHandler( 1, ExpHook );

	FnNtQueryVirtualMemory = ( ONtQueryVirtualMemory )
		PBYTE( GetProcAddress( LoadLibraryA( XS( "ntdll" ) ), XS( "NtQueryVirtualMemory" ) ) );

	hkVQEX = PBYTE( GetProcAddress( LoadLibraryA( XS( "kernelbase.dll" ) ), XS( "VirtualQueryEx" ) ) );
	SetXignBreakpoint();*/

	//Old_VirtualQueryEx = ( _VirtualQueryEx )
	//	CreateHook2( PBYTE( GetProcAddress( LoadLibraryA( XS( "kernelbase.dll" ) ), XS( "VirtualQueryEx" ) ) ), PBYTE( &hkVirtualQueryEx ) );

	//Old_NtQueryVirtualMemory2 = ( NtQueryVirtualMemory2_ )
	//	IAT::detour_iat_ptr( XS( "NtQueryVirtualMemory" ), PVOID( hkNtQueryVirtualMemory2 ), LoadLibraryA( XS( "kernelbase.dll" ) ) );

	//AddVectoredExceptionHandler( 1, PageGuardTest );
	//SymSetOptions( SYMOPT_UNDNAME );
	//SymInitialize( GetCurrentProcess(), nullptr, TRUE );

	//oVirtualAlloc = ( VirtualAlloc_ )
	//	CreateHook( PBYTE( GetProcAddress( LoadLibraryA( "kernelbase.dll" ), "VirtualAlloc" ) ), PBYTE( &hkVA ) );

	//	Iat_hook::detour_iat_ptr( "VirtualAlloc", ( void* )hkVA, LoadLibraryA( "kernelbase.dll" ) );

	//oVirtualAllocEx = ( VirtualAllocEx_ )
	//	CreateHook( PBYTE( GetProcAddress( LoadLibraryA( "kernelbase.dll" ), "VirtualAllocEx" ) ), PBYTE( &hkVAEX ) );

	//	Iat_hook::detour_iat_ptr( "VirtualAllocEx", ( void* )hkVAEX, LoadLibraryA( "kernelbase.dll" ) );

	//desired = DWORD( GetProcAddress( LoadLibraryA( "ntdll" ), "NtQueryVirtualMemory" ) );
	//desired2 = DWORD( GetProcAddress( LoadLibraryA( "ntdll" ), "NtAllocateVirtualMemory" ) );

	//DWORD peb = __readfsdword( 0x30 );
	//*( PVOID* )( peb + 0x488 ) = ( PVOID )&hk_instrumentation;
}