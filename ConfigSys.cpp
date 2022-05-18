#include "Includes.hpp"

#include "json/json.h"

#include <shlobj_core.h>
#include <filesystem>
#include <fstream>

static const char* aimbot_groups[] = { "Pistol", "Shotgun", "SMG", "Rifle", "Sniper", "Heavy" };

CConfigSystem config_system;

void GetVal( Json::Value& _cfg, int* setting )
{
	if ( _cfg.isNull() )
		return;

	*setting = _cfg.asInt();
}

void GetVal( Json::Value& _cfg, bool* setting )
{
	if ( _cfg.isNull() )
		return;

	*setting = _cfg.asBool();
}

void GetVal( Json::Value& _cfg, float* setting )
{
	if ( _cfg.isNull() )
		return;

	*setting = _cfg.asFloat();
}

void GetVal( Json::Value& _cfg, float* setting, bool is_color )
{
	if ( _cfg.isNull() )
		return;

	if ( !is_color )
		*setting = _cfg.asFloat();
	else
	{
		GetVal( _cfg[ XS( "r" ) ], &setting[ 0 ], false );
		GetVal( _cfg[ XS( "g" ) ], &setting[ 1 ], false );
		GetVal( _cfg[ XS( "b" ) ], &setting[ 2 ], false );
		GetVal( _cfg[ XS( "a" ) ], &setting[ 3 ], false );
	}
}


void GetVal( Json::Value& _cfg, ImColor* setting )
{
	if ( _cfg.isNull() )
		return;

	GetVal( _cfg[ XS( "r" ) ], &setting->Value.x, false );
	GetVal( _cfg[ XS( "g" ) ], &setting->Value.y, false );
	GetVal( _cfg[ XS( "b" ) ], &setting->Value.z, false );
	GetVal( _cfg[ XS( "a" ) ], &setting->Value.w, false );
}

void GetVal( Json::Value& _cfg, char** setting )
{
	if ( _cfg.isNull() )
		return;

	*setting = const_cast< char *>( _cfg.asCString() );
}

void GetVal( Json::Value& _cfg, char* setting )
{
	if ( _cfg.isNull() )
		return;

	strcpy( setting, _cfg.asCString() );
}

template <typename Ord, Ord( *lookupFunction )( std::string )>
void GetOrdinal( Json::Value& _cfg, Ord* setting )
{
	if ( _cfg.isNull() )
		return;

	Ord value;
	if ( _cfg.isString() )
		value = lookupFunction( _cfg.asString() );
	else
		value = ( Ord )_cfg.asInt();

	*setting = value;
}

void LoadColor( Json::Value& _cfg, ImColor color )
{
	_cfg[ XS( "r" ) ] = color.Value.x;
	_cfg[ XS( "g" ) ] = color.Value.y;
	_cfg[ XS( "b" ) ] = color.Value.z;
	_cfg[ XS( "a" ) ] = color.Value.w;
}

void LoadColor( Json::Value& _cfg, float* color )
{
	_cfg[ XS( "r" ) ] = color[ 0 ];
	_cfg[ XS( "g" ) ] = color[ 1 ];
	_cfg[ XS( "b" ) ] = color[ 2 ];
	_cfg[ XS( "a" ) ] = color[ 3 ];
}

void CConfigSystem::SaveConfig( std::string path )
{
	if ( path.empty() )
		path = GetConfigFolder() + XS( "default.json" );
	else
		path = GetConfigFolder() + path + XS( ".json" );

	Json::Value settings;
	Json::StreamWriterBuilder builder;
	builder[ XS( "commentStyle" ) ] = XS( "None" );
	builder[ XS( "indentation" ) ] = XS( "   " );

	try
	{
		///ESP
		LoadColor( settings[ XS( "ESP" ) ][ XS( "enemy_color" ) ], config.ESP.EnemyCol );
		LoadColor( settings[ XS( "ESP" ) ][ XS( "enemy_vis_color" ) ], config.ESP.EnemyVisCol );
		LoadColor( settings[ XS( "ESP" ) ][ XS( "glow_color" ) ], config.ESP.GlowEnemyCol );
		LoadColor( settings[ XS( "ESP" ) ][ XS( "glow_vis_color" ) ], config.ESP.GlowEnemyVisCol );
		settings[ XS( "ESP" ) ][ XS( "glow_force" ) ] = config.ESP.GlowThickness;

		settings[ XS( "Visuals" ) ][ XS( "ESP" ) ][ XS( "radar" ) ] = config.ESP.Radar;
		settings[ XS( "Visuals" ) ][ XS( "ESP" ) ][ XS( "names" ) ] = config.ESP.Names;
		settings[ XS( "Visuals" ) ][ XS( "ESP" ) ][ XS( "skeleton" ) ] = config.ESP.Skeleton;
		settings[ XS( "Visuals" ) ][ XS( "ESP" ) ][ XS( "boxes" ) ] = config.ESP.Boxes;
		settings[ XS( "Visuals" ) ][ XS( "ESP" ) ][ XS( "health" ) ] = config.ESP.Health;
		settings[ XS( "Visuals" ) ][ XS( "ESP" ) ][ XS( "weapons" ) ] = config.ESP.Weapons;
		settings[ XS( "Visuals" ) ][ XS( "ESP" ) ][ XS( "c4" ) ] = config.ESP.ShowC4;
		settings[ XS( "Visuals" ) ][ XS( "ESP" ) ][ XS( "glow" ) ] = config.ESP.Glow;
		settings[ XS( "Visuals" ) ][ XS( "ESP" ) ][ XS( "only_vis" ) ] = config.ESP.OnlyVisible;
		settings[ XS( "Visuals" ) ][ XS( "ESP" ) ][ XS( "zm" ) ] = config.ESP.UseOnZM;

		///Chams
		LoadColor( settings[ XS( "ESP" ) ][ XS( "chams_color" ) ], config.ESP.Chams.EnemyCol );
		LoadColor( settings[ XS( "ESP" ) ][ XS( "chams_vis_color" ) ], config.ESP.Chams.EnemyVisCol );

		settings[ XS( "Visuals" ) ][ XS( "Chams" ) ][ XS( "wallhack" ) ] = config.ESP.Chams.Wallhack;
		settings[ XS( "Visuals" ) ][ XS( "Chams" ) ][ XS( "chams" ) ] = config.ESP.Chams.Chams;
		settings[ XS( "Visuals" ) ][ XS( "Chams" ) ][ XS( "only_vis" ) ] = config.ESP.Chams.OnlyVisible;
		settings[ XS( "Visuals" ) ][ XS( "Chams" ) ][ XS( "fullbright" ) ] = config.ESP.Chams.Fullbright;
		settings[ XS( "Visuals" ) ][ XS( "Chams" ) ][ XS( "ghostchams" ) ] = config.ESP.Chams.GhostChams;

		///Aimbot
		for ( int i = 0; i < IM_ARRAYSIZE( aimbot_groups ); i++ )
		{
			settings[ XS( "Aimbot" ) ][ aimbot_groups[ i ] ][ XS( "enabled" ) ] = config.Aimbot[ i ].Enabled;
			settings[ XS( "Aimbot" ) ][ aimbot_groups[ i ] ][ XS( "key" ) ] = config.Aimbot[ i ].Key;
			settings[ XS( "Aimbot" ) ][ aimbot_groups[ i ] ][ XS( "silentaim" ) ] = config.Aimbot[ i ].SilentAim;
			settings[ XS( "Aimbot" ) ][ aimbot_groups[ i ] ][ XS( "vischeck" ) ] = config.Aimbot[ i ].VisibleCheck;
			settings[ XS( "Aimbot" ) ][ aimbot_groups[ i ] ][ XS( "teamkill" ) ] = config.Aimbot[ i ].TeamKill;
			settings[ XS( "Aimbot" ) ][ aimbot_groups[ i ] ][ XS( "autoaim" ) ] = config.Aimbot[ i ].AutoAim;
			settings[ XS( "Aimbot" ) ][ aimbot_groups[ i ] ][ XS( "autoshoot" ) ] = config.Aimbot[ i ].AutoShoot;

			settings[ XS( "Aimbot" ) ][ aimbot_groups[ i ] ][ XS( "smooth" ) ] = config.Aimbot[ i ].Smoothness;
			settings[ XS( "Aimbot" ) ][ aimbot_groups[ i ] ][ XS( "fov" ) ] = config.Aimbot[ i ].FieldOfView;
			settings[ XS( "Aimbot" ) ][ aimbot_groups[ i ] ][ XS( "fovtype" ) ] = config.Aimbot[ i ].FOVType;
			settings[ XS( "Aimbot" ) ][ aimbot_groups[ i ] ][ XS( "hitbox" ) ] = config.Aimbot[ i ].Hitbox;
			settings[ XS( "Aimbot" ) ][ aimbot_groups[ i ] ][ XS( "zm_prioritize_hp" ) ] = config.Aimbot[ i ].PrioritizeHealth;
			settings[ XS( "Aimbot" ) ][ aimbot_groups[ i ] ][ XS( "bone_scan" ) ] = config.Aimbot[ i ].BoneScan;

			settings[ XS( "Triggerbot" ) ][ aimbot_groups[ i ] ][ XS( "enabled" ) ] = config.Aimbot[ i ].TriggerEnabled;
			settings[ XS( "Triggerbot" ) ][ aimbot_groups[ i ] ][ XS( "key" ) ] = config.Aimbot[ i ].TriggerKey;
			settings[ XS( "Triggerbot" ) ][ aimbot_groups[ i ] ][ XS( "autoshoot" ) ] = config.Aimbot[ i ].TriggerAutoShoot;
			settings[ XS( "Triggerbot" ) ][ aimbot_groups[ i ] ][ XS( "delay" ) ] = config.Aimbot[ i ].TriggerDelay;
		}

		///Misc
		settings[ XS( "Misc" ) ][ XS( "nofog" ) ] = config.Misc.RemoveFog;
		settings[ XS( "Misc" ) ][ XS( "norecoil" ) ] = config.Misc.LessRecoil;
		settings[ XS( "Misc" ) ][ XS( "nospread" ) ] = config.Misc.LessSpread;
		settings[ XS( "Misc" ) ][ XS( "noreload" ) ] = config.Misc.FastReload;
		settings[ XS( "Misc" ) ][ XS( "nochangedelay" ) ] = config.Misc.FastWeaponChange;
		settings[ XS( "Misc" ) ][ XS( "nosmoke" ) ] = config.Misc.NoSmoke;
		settings[ XS( "Misc" ) ][ XS( "noflash" ) ] = config.Misc.NoFlash;
		settings[ XS( "Misc" ) ][ XS( "crosshair" ) ] = config.Misc.SniperCHair;
		settings[ XS( "Misc" ) ][ XS( "bhop" ) ] = config.Misc.Bunnyhop;
		settings[ XS( "Misc" ) ][ XS( "max_range" ) ] = config.Misc.MaxWeaponRange;
		settings[ XS( "Misc" ) ][ XS( "spy" ) ] = config.Misc.SpyMode;
		settings[ XS( "Misc" ) ][ XS( "fast_knife" ) ] = config.Misc.FastKnife;
		settings[ XS( "Misc" ) ][ XS( "fast_fire" ) ] = config.Misc.FastFire;
		settings[ XS( "Misc" ) ][ XS( "insta_defuse" ) ] = config.Misc.InstaDefuse;

		settings[ XS( "Misc" ) ][ XS( "nofalldmg" ) ] = config.Misc.NoFallDamage;
		settings[ XS( "Misc" ) ][ XS( "nobugdmg" ) ] = config.Misc.NoBugDamage;
		settings[ XS( "Misc" ) ][ XS( "crouch_speed" ) ] = config.Misc.FastWalk;
		settings[ XS( "Misc" ) ][ XS( "watermark" ) ] = config.Misc.Watermark;
		settings[ XS( "Misc" ) ][ XS( "afkbot" ) ] = config.Misc.AFKBot;

		settings[ XS( "Misc" ) ][ XS( "fov" ) ] = config.Misc.WeaponFOV;

		settings[ XS( "Misc" ) ][ XS( "spy_key" ) ] = config.Misc.SpyModeKey;

		std::unique_ptr<Json::StreamWriter> writer( builder.newStreamWriter() );
		std::ofstream outputFileStream( path );
		writer->write( settings, &outputFileStream );
	}
	catch ( Json::Exception& ex )
	{
#ifdef _DEBUG
		printf( "Failed to save settings, exception: %s\n", ex.what() );
#else
		UNREFERENCED_PARAMETER( ex );
#endif
	}
}

std::string CConfigSystem::GetConfigFolder()
{
	ConfigPath.clear();
	CHAR szPath[ MAX_PATH ];

	HRESULT hr = SHGetFolderPathA( HWND_DESKTOP, CSIDL_COMMON_APPDATA, INVALID_HANDLE_VALUE, 0, szPath );
	if ( SUCCEEDED( hr ) )
	{
		ConfigPath = szPath;
		ConfigPath += XS( "\\benigo_cf" );

		if ( std::filesystem::exists( ConfigPath ) )
			return ConfigPath + XS( "\\" );
		else
		{
			std::filesystem::create_directory( ConfigPath );
			return ConfigPath + XS( "\\" );
		}
	}
	return "";
}

void CConfigSystem::LoadConfig( std::string path )
{
	if ( path.empty() )
	{
		path = GetConfigFolder() + XS( "default.json" );

		if ( std::filesystem::exists( path ) )
			return LoadConfig( path );
		else
			return SaveConfig( "" );
	}

	try
	{
		Json::Value settings;
		std::ifstream configDoc( path, std::ifstream::binary );
		configDoc >> settings;

		GetVal( settings[ XS( "ESP" ) ][ XS( "enemy_color" ) ], config.ESP.EnemyCol, true );
		GetVal( settings[ XS( "ESP" ) ][ XS( "enemy_vis_color" ) ], config.ESP.EnemyVisCol, true );
		GetVal( settings[ XS( "ESP" ) ][ XS( "glow_color" ) ], config.ESP.GlowEnemyCol, true );
		GetVal( settings[ XS( "ESP" ) ][ XS( "glow_vis_color" ) ], config.ESP.GlowEnemyVisCol, true );
		GetVal( settings[ XS( "ESP" ) ][ XS( "glow_force" ) ], &config.ESP.GlowThickness );

		GetVal( settings[ XS( "Visuals" ) ][ XS( "ESP" ) ][ XS( "radar" ) ], &config.ESP.Radar );
		GetVal( settings[ XS( "Visuals" ) ][ XS( "ESP" ) ][ XS( "names" ) ], &config.ESP.Names );
		GetVal( settings[ XS( "Visuals" ) ][ XS( "ESP" ) ][ XS( "skeleton" ) ], &config.ESP.Skeleton );
		GetVal( settings[ XS( "Visuals" ) ][ XS( "ESP" ) ][ XS( "boxes" ) ], &config.ESP.Boxes );
		GetVal( settings[ XS( "Visuals" ) ][ XS( "ESP" ) ][ XS( "health" ) ], &config.ESP.Health );
		GetVal( settings[ XS( "Visuals" ) ][ XS( "ESP" ) ][ XS( "weapons" ) ], &config.ESP.Weapons );
		GetVal( settings[ XS( "Visuals" ) ][ XS( "ESP" ) ][ XS( "c4" ) ], &config.ESP.ShowC4 );
		GetVal( settings[ XS( "Visuals" ) ][ XS( "ESP" ) ][ XS( "glow" ) ], &config.ESP.Glow );
		GetVal( settings[ XS( "Visuals" ) ][ XS( "ESP" ) ][ XS( "only_vis" ) ], &config.ESP.OnlyVisible );
		GetVal( settings[ XS( "Visuals" ) ][ XS( "ESP" ) ][ XS( "zm" ) ], &config.ESP.UseOnZM );

		///Chams
		GetVal( settings[ XS( "ESP" ) ][ XS( "chams_color" ) ], config.ESP.Chams.EnemyCol, true );
		GetVal( settings[ XS( "ESP" ) ][ XS( "chams_vis_color" ) ], config.ESP.Chams.EnemyVisCol, true );

		GetVal( settings[ XS( "Visuals" ) ][ XS( "Chams" ) ][ XS( "wallhack" ) ], &config.ESP.Chams.Wallhack );
		GetVal( settings[ XS( "Visuals" ) ][ XS( "Chams" ) ][ XS( "chams" ) ], &config.ESP.Chams.Chams );
		GetVal( settings[ XS( "Visuals" ) ][ XS( "Chams" ) ][ XS( "only_vis" ) ], &config.ESP.Chams.OnlyVisible );
		GetVal( settings[ XS( "Visuals" ) ][ XS( "Chams" ) ][ XS( "fullbright" ) ], &config.ESP.Chams.Fullbright );
		GetVal( settings[ XS( "Visuals" ) ][ XS( "Chams" ) ][ XS( "ghostchams" ) ], &config.ESP.Chams.GhostChams );


		///Aimbot
		for ( int i = 0; i < IM_ARRAYSIZE( aimbot_groups ); i++ )
		{
			GetVal( settings[ XS( "Aimbot" ) ][ aimbot_groups[ i ] ][ XS( "enabled" ) ], &config.Aimbot[ i ].Enabled );
			GetVal( settings[ XS( "Aimbot" ) ][ aimbot_groups[ i ] ][ XS( "key" ) ], &config.Aimbot[ i ].Key );
			GetVal( settings[ XS( "Aimbot" ) ][ aimbot_groups[ i ] ][ XS( "silentaim" ) ], &config.Aimbot[ i ].SilentAim );
			GetVal( settings[ XS( "Aimbot" ) ][ aimbot_groups[ i ] ][ XS( "vischeck" ) ], &config.Aimbot[ i ].VisibleCheck );
			GetVal( settings[ XS( "Aimbot" ) ][ aimbot_groups[ i ] ][ XS( "teamkill" ) ], &config.Aimbot[ i ].TeamKill );
			GetVal( settings[ XS( "Aimbot" ) ][ aimbot_groups[ i ] ][ XS( "autoaim" ) ], &config.Aimbot[ i ].AutoAim );
			GetVal( settings[ XS( "Aimbot" ) ][ aimbot_groups[ i ] ][ XS( "autoshoot" ) ], &config.Aimbot[ i ].AutoShoot );

			GetVal( settings[ XS( "Aimbot" ) ][ aimbot_groups[ i ] ][ XS( "smooth" ) ], &config.Aimbot[ i ].Smoothness, false );
			GetVal( settings[ XS( "Aimbot" ) ][ aimbot_groups[ i ] ][ XS( "fov" ) ], &config.Aimbot[ i ].FieldOfView );
			GetVal( settings[ XS( "Aimbot" ) ][ aimbot_groups[ i ] ][ XS( "fovtype" ) ], &config.Aimbot[ i ].FOVType );
			GetVal( settings[ XS( "Aimbot" ) ][ aimbot_groups[ i ] ][ XS( "hitbox" ) ], &config.Aimbot[ i ].Hitbox );

			GetVal( settings[ XS( "Aimbot" ) ][ aimbot_groups[ i ] ][ XS( "zm_prioritize_hp" ) ], &config.Aimbot[ i ].PrioritizeHealth );
			GetVal( settings[ XS( "Aimbot" ) ][ aimbot_groups[ i ] ][ XS( "bone_scan" ) ], &config.Aimbot[ i ].BoneScan );

			GetVal( settings[ XS( "Triggerbot" ) ][ aimbot_groups[ i ] ][ XS( "trigger_enabled" ) ], &config.Aimbot[ i ].TriggerEnabled );
			GetVal( settings[ XS( "Triggerbot" ) ][ aimbot_groups[ i ] ][ XS( "trigger_key" ) ], &config.Aimbot[ i ].TriggerKey );
			GetVal( settings[ XS( "Triggerbot" ) ][ aimbot_groups[ i ] ][ XS( "trigger_autoshoot" ) ], &config.Aimbot[ i ].TriggerAutoShoot );
			GetVal( settings[ XS( "Triggerbot" ) ][ aimbot_groups[ i ] ][ XS( "delay" ) ], &config.Aimbot[ i ].TriggerDelay );
		}

		///Misc
		GetVal( settings[ XS( "Misc" ) ][ XS( "nofog" ) ], &config.Misc.RemoveFog );
		GetVal( settings[ XS( "Misc" ) ][ XS( "norecoil" ) ], &config.Misc.LessRecoil );
		GetVal( settings[ XS( "Misc" ) ][ XS( "nospread" ) ], &config.Misc.LessSpread );
		GetVal( settings[ XS( "Misc" ) ][ XS( "noreload" ) ], &config.Misc.FastReload );
		GetVal( settings[ XS( "Misc" ) ][ XS( "nochangedelay" ) ], &config.Misc.FastWeaponChange );
		GetVal( settings[ XS( "Misc" ) ][ XS( "nosmoke" ) ], &config.Misc.NoSmoke );
		GetVal( settings[ XS( "Misc" ) ][ XS( "noflash" ) ], &config.Misc.NoFlash );
		GetVal( settings[ XS( "Misc" ) ][ XS( "crosshair" ) ], &config.Misc.SniperCHair );
		GetVal( settings[ XS( "Misc" ) ][ XS( "bhop" ) ], &config.Misc.Bunnyhop );
		GetVal( settings[ XS( "Misc" ) ][ XS( "max_range" ) ], &config.Misc.MaxWeaponRange );
		GetVal( settings[ XS( "Misc" ) ][ XS( "spy" ) ], &config.Misc.SpyMode );
		GetVal( settings[ XS( "Misc" ) ][ XS( "fast_knife" ) ], &config.Misc.FastKnife );
		GetVal( settings[ XS( "Misc" ) ][ XS( "fast_fire" ) ], &config.Misc.FastFire );
		GetVal( settings[ XS( "Misc" ) ][ XS( "insta_defuse" ) ], &config.Misc.InstaDefuse );

		GetVal( settings[ XS( "Misc" ) ][ XS( "nofalldmg" ) ], &config.Misc.NoFallDamage );
		GetVal( settings[ XS( "Misc" ) ][ XS( "nobugdmg" ) ], &config.Misc.NoBugDamage );
		GetVal( settings[ XS( "Misc" ) ][ XS( "fastwalk" ) ], &config.Misc.FastWalk );
		GetVal( settings[ XS( "Misc" ) ][ XS( "watermark" ) ], &config.Misc.Watermark );
		GetVal( settings[ XS( "Misc" ) ][ XS( "afkbot" ) ], &config.Misc.AFKBot );

		GetVal( settings[ XS( "Misc" ) ][ XS( "fov" ) ], &config.Misc.WeaponFOV );
		GetVal( settings[ XS( "Misc" ) ][ XS( "spy_key" ) ], &config.Misc.SpyModeKey );
	}
	catch ( Json::Exception& ex )
	{
#ifdef _DEBUG
		printf( "Failed to load settings, exception: %s\n", ex.what() );
#else
		UNREFERENCED_PARAMETER( ex );
#endif
	}
}