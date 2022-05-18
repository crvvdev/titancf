#pragma once

typedef NTSTATUS( NTAPI* _ZwReadVirtualMemory )( IN HANDLE ProcessHandle, IN PVOID BaseAddress, OUT PVOID Buffer, IN ULONG NumberOfBytesToRead, OUT PULONG NumberOfBytesReaded OPTIONAL );
extern _ZwReadVirtualMemory Old_ZwReadVirtualMemory;

typedef NTSTATUS( NTAPI* _ZwQueryInformationThread )( IN HANDLE ThreadHandle, IN THREADINFOCLASS ThreadInformationClass, PVOID ThreadInformation, _In_ ULONG ThreadInformationLength, _Out_opt_ PULONG ReturnLength );
extern _ZwQueryInformationThread Old_ZwQueryInformationThread;

typedef NTSTATUS( NTAPI* _ZwProtectVirtualMemory )( IN HANDLE ProcessHandle, IN PVOID* BaseAddress, IN SIZE_T* NumberOfBytesToProtect, IN ULONG NewAccessProtection, OUT PULONG OldAccessProtection );
extern _ZwProtectVirtualMemory Old_ZwProtectVirtualMemory;

typedef SIZE_T( WINAPI* _VirtualQuery )( DWORD lpAddress, PMEMORY_BASIC_INFORMATION lpBuffer, SIZE_T dwLength );
extern _VirtualQuery Old_VirtualQuery;

//typedef SIZE_T( WINAPI* _VirtualQueryEx )( HANDLE hProcess, DWORD lpAddress, PMEMORY_BASIC_INFORMATION lpBuffer, SIZE_T dwLength );
//extern _VirtualQueryEx Old_VirtualQueryEx;

using NtQueryVirtualMemory_ = NTSTATUS( NTAPI* )( HANDLE ProcessHandle, DWORD BaseAddress, MEMORY_INFORMATION_CLASS Type, PVOID Out, ULONG Length, ULONG* NumberOfBytesRead );
extern NtQueryVirtualMemory_ Old_NtQueryVirtualMemory;

using NtQueryVirtualMemory_ = NTSTATUS( NTAPI* )( HANDLE ProcessHandle, DWORD BaseAddress, MEMORY_INFORMATION_CLASS Type, PVOID Out, ULONG Length, ULONG* NumberOfBytesRead );
extern NtQueryVirtualMemory_ Old_NtQueryVirtualMemory;

using NtQueryVirtualMemory2_ = NTSTATUS( NTAPI* )( HANDLE ProcessHandle, DWORD BaseAddress, MEMORY_INFORMATION_CLASS Type, PVOID Out, ULONG Length, ULONG* NumberOfBytesRead );
extern NtQueryVirtualMemory2_ Old_NtQueryVirtualMemory2;

extern NTSTATUS NTAPI hkZwReadVirtualMemory( IN HANDLE ProcessHandle, IN PVOID BaseAddress, OUT PVOID Buffer, IN ULONG NumberOfBytesToRead, OUT PULONG NumberOfBytesReaded OPTIONAL );
extern NTSTATUS NTAPI hkZwQueryInformationThread( IN HANDLE ThreadHandle, IN THREADINFOCLASS ThreadInformationClass, PVOID ThreadInformation, _In_ ULONG ThreadInformationLength, _Out_opt_ PULONG ReturnLength );
extern NTSTATUS NTAPI hkZwProtectVirtualMemory( IN HANDLE ProcessHandle, IN PVOID* BaseAddress, IN SIZE_T* NumberOfBytesToProtect, IN ULONG NewAccessProtection, OUT PULONG OldAccessProtection );
extern SIZE_T WINAPI hkVirtualQuery( DWORD lpAddress, PMEMORY_BASIC_INFORMATION lpBuffer, SIZE_T dwLength );
extern NTSTATUS NTAPI hkNtQueryVirtualMemory( HANDLE ProcessHandle, DWORD BaseAddress, MEMORY_INFORMATION_CLASS Type, PVOID Out, ULONG Length, ULONG* NumberOfBytesRead );
extern NTSTATUS NTAPI hkNtQueryVirtualMemory2( HANDLE ProcessHandle, DWORD BaseAddress, MEMORY_INFORMATION_CLASS Type, PVOID Out, ULONG Length, ULONG* NumberOfBytesRead );

extern void SetupInstrumentationHook();