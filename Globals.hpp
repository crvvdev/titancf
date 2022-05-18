#pragma once

#include <cstdint>

//typedef struct
//{
//	DWORD Address;
//	int Hits;
//
//	bool operator==(const DWORD& addr)
//	{
//		return (addr == this->Address);
//	}
//
//} RET_ADDR_DATA;
//
//extern std::vector< RET_ADDR_DATA > rets;

namespace Globals
{
	namespace Pointer
	{
		extern DWORD dwLTClientShell;
		extern DWORD dwLTModelClient;
		extern DWORD dwLTClient;
	};

	namespace Offset
	{
		extern DWORD uPlayerSize;
		extern DWORD uPlayerOffset;
	};

	namespace Address
	{
		extern DWORD FnIntersectSegment;
		extern DWORD dwSetGlowStrenght;
		extern DWORD FnSetGlowActive;
		extern DWORD FnSetGlowOutline;
		extern DWORD FnUpdateGlowOutine;
		extern DWORD FnGetLocalPlayerIndex;
		extern DWORD FnGetWeaponByID;
		extern DWORD FnGetBPIByID;
		extern DWORD FnGetNodeByID;
	};

	namespace Module
	{
		extern DWORD CrossFire, CrossFireSize;
		extern DWORD CShell, CShellSize;
		extern DWORD OwnModule, OwnModuleSize;
	};

	extern bool g_bInterfaceInitialized;
	extern bool g_bSpyModeActive;
	extern bool g_bIsSafeToHook;
}