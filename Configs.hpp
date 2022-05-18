#pragma once

struct AimbotSettings
{
	bool Enabled;
	int Key;
	bool AutoAim;
	bool AutoShoot;
	float Smoothness;
	float Sense;
	float FieldOfView;
	int FOVType;
	int Hitbox;
	bool SilentAim;
	bool VisibleCheck;
	bool PrioritizeHealth;
	bool TeamKill;
	bool ZM;
	bool DrawFOV;
	bool BoneScan;
	bool MouseEvent;

	bool TriggerEnabled;
	int TriggerKey;
	bool TriggerAutoShoot;
	int TriggerDelay;
};

struct CONFIGS
{
	CONFIGS()
	{
		memset( &ESP, 0, sizeof( ESP ) );
		memset( &ESP.Chams, 0, sizeof( ESP.Chams ) );
		memset( &Misc, 0, sizeof( Misc ) );

		for ( auto i = 0; i < 10; i++ )
		{
			memset( &Aimbot[ i ], 0, sizeof( AimbotSettings ) );
			Aimbot[ i ].Key = VK_LBUTTON;
		}

		//Intensity
		ESP.GlowThickness = 1.8f;

		//Red
		ESP.Chams.EnemyCol[ 0 ] = 1.f;

		//Blue
		ESP.Chams.EnemyVisCol[ 2 ] = 1.f;

		//Enemy
		ESP.EnemyCol[ 0 ] = 1.0f;

		//Enemy Vis
		ESP.EnemyVisCol[ 0 ] = 1.0f;
		ESP.EnemyVisCol[ 1 ] = 1.0f;

		//Glow Enemy
		ESP.GlowEnemyCol[ 0 ] = 1.0f;

		//Glow Enemy Vis
		ESP.GlowEnemyVisCol[ 0 ] = 1.0f;
		ESP.GlowEnemyVisCol[ 1 ] = 1.0f;

		//Spy Mode Key
		Misc.SpyModeKey = VK_F10;
		Misc.LagRoomKey = VK_F11;
		Misc.CrashRoomKey = VK_F12;
	}

	AimbotSettings Aimbot[ 10 ];

	struct
	{
		bool Names;
		bool Skeleton;
		int Boxes;
		int Health;
		bool Weapons;
		bool ShowC4;
		bool OnlyVisible;
		bool UseOnZM;
		bool Radar;

		bool Glow;
		float GlowThickness;

		float EnemyCol[ 3 ];
		float EnemyVisCol[ 3 ];

		float GlowEnemyCol[ 3 ];
		float GlowEnemyVisCol[ 3 ];

		struct
		{
			bool Wallhack;
			bool Chams;
			bool Fullbright;
			bool GhostChams;
			bool OnlyVisible;

			float EnemyCol[ 3 ];
			float EnemyVisCol[ 3 ];
		} Chams;
	} ESP;

	struct
	{
		bool RemoveFog;
		bool LessRecoil;
		bool LessSpread;
		bool FastReload;
		bool FastWeaponChange;
		bool MaxWeaponRange;
		bool InstaDefuse;
		bool FastKnife;
		float FastKnifeSpeed;

		bool NoSmoke;
		bool NoFlash;
		bool SniperCHair;
		bool Bunnyhop;
		bool SpyMode;
		bool FastFire;

		float WeaponFOV;
		bool NoBugDamage;
		bool NoFallDamage;
		bool NoNadeDamage;

		bool FastWalk;
		bool Watermark;
		bool RadioSpam;
		bool AFKBot;

		int SpyModeKey;
		int LagRoomKey;
		int CrashRoomKey;
	} Misc;

	/*bool misc[ 100 ];
	bool esp[ 100 ];
	bool chams[ 100 ];
	bool esp_filters[ 10 ][ 10 ];

	float esp_colors[ 15 ][ 4 ];
	float chams_colors[ 15 ][ 4 ];
	float testing;

	int esp_misc[ 10 ];
	int other_misc[ 100 ];
	float misc_misc[ 10 ];

	int custom_keys[ 10 ];*/
};

extern CONFIGS config;