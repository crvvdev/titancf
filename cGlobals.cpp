#include "Includes.hpp"

namespace Globals
{
	namespace Pointer
	{
		DWORD dwLTClientShell = 0;
		DWORD dwLTModelClient = 0;
		DWORD dwLTClient = 0;
	};

	namespace Offset
	{
		DWORD uPlayerSize = 0;
		DWORD uPlayerOffset = 0;
	};

	namespace Address
	{
		DWORD FnIntersectSegment = 0;
		DWORD dwSetGlowStrenght = 0;
		DWORD FnSetGlowActive = 0;
		DWORD FnSetGlowOutline = 0;
		DWORD FnUpdateGlowOutine = 0;
		DWORD FnGetLocalPlayerIndex = 0;
		DWORD FnGetWeaponByID = 0;
		DWORD FnGetBPIByID = 0;
		DWORD FnGetNodeByID = 0;
	};

	namespace Module
	{
		DWORD CrossFire = 0, CrossFireSize = 0;
		DWORD CShell = 0, CShellSize = 0;
		DWORD OwnModule = 0, OwnModuleSize = 0;
	};

	bool g_bInterfaceInitialized = false;
	bool g_bSpyModeActive = false;
	bool g_bIsSafeToHook = false;
}

hGetLocalPlayerIndex	GetLocalPlayerIndex = NULL;
hIntersectSegment		oIntersectSegment = NULL;
hRenderCamera			oRenderCamera = NULL;
hGetObjectDim			oGetObjectDim = NULL;
UpdateOutline_			FnUpdateOutline = NULL;
SetObjectOutline_		FnSetOutline = NULL;
GlowSetActive_			FnGlowSetActive = NULL;
GetWeaponByIndex_		oGetWeaponByIndex = NULL;
GetBasicPlayerInfo_		oGetBasicPlayerInfo = NULL;

IDirect3DDevice9* g_pD3DDevice = nullptr;
CLTClientShell* g_pLTClientShell = nullptr;
CLTModel* g_pLTModel = nullptr;
CLTClient* g_pLTClient = nullptr;
CLTCommon* g_pLTCommon = nullptr;
