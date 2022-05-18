#pragma once

#ifndef _FINAL
#define _FINAL
#endif

#define WINVER			0x0601
#define _WIN32_WINNT	0x0601
#define NOMINMAX

#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN

#include <winsock2.h>
#include <windows.h>
#include <process.h>
#include <psapi.h>

#include <cstdint>
#include <cstdio>
#include <string>
#include <sstream>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <algorithm>
#include <iostream>
#include <intrin.h>
#include <vector>
#include <ctime>
#include <VersionHelpers.h>
#include <shellapi.h>
#include <filesystem>
#include <shlwapi.h>
#include <wininet.h>

#include <d3d9.h>
#include <d3dx9.h>
#include <ShlObj.h>
#include <DbgHelp.h>

#pragma comment( lib, "d3d9")
#pragma comment( lib, "d3dx9")
#pragma comment( lib, "Winmm")
#pragma comment( lib, "Shell32")
#pragma comment( lib, "Shlwapi")
#pragma comment( lib, "dbghelp")

extern IDirect3DDevice9* g_pD3DDevice;

static double latency_val = 0.0;

//3th party includes
#include "xorstr.hpp"

#ifndef _USE_DETOURS
#include "MinHook.h"
#pragma comment( lib, "libMinHook.x86.lib")
#else
#include "detours.h"
#pragma comment( lib, "detours.lib")
#endif

#include "Protection.hpp"

//local project includes
#include "WinSDK.hpp"
#include "Tools.hpp"
#include "HookManager.hpp"
#include "Globals.hpp"
#include "Math.hpp"

#include "ImGUI/imgui.h"
#include "ImGUI/imgui_impl_dx9.h"
#include "ImGUI/imgui_impl_win32.h"
#include "ImGUI/spectrum.h"
#include "Logo.hpp"

#include "ConfigSys.hpp"
#include "InputSys.hpp"
#include "Configs.hpp"
#include "Rendering.hpp"
#include "GUI.hpp"

#include "MsgIDs.hpp"
#include "Classes.hpp"
#include "Engine.hpp"
#include "Rendering.hpp"

#include "ESP.hpp"
#include "Radar.hpp"
#include "ZM.hpp"
#include "Aimbot.hpp"
#include "Trigger.hpp"
#include "Memory.hpp"
#include "ACHook.hpp"
#include "BreakpointManager.hpp"
//#include "BreakpointManager.hpp"

static std::vector< std::pair< D3DXVECTOR3, D3DXVECTOR3 > > saved_hitboxes;
//static bool can_modify_val = false;
