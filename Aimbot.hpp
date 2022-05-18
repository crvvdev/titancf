#pragma once

#define INVALID_AIM_TARGET		-1

class CAimbot
{
protected:
	CWeapon* m_pCurrentWeapon;
	CPlayer* m_pLocalPlayer;
	CPlayerClient* m_pPlayerClient;

	int m_iCurrentTarget;
	int m_iCurWeaponType;

	D3DXVECTOR3 m_CurAimedBone;

	float m_iAimFOV;
	bool m_bValidZombie;
	HOBJECT m_bZombieObj;

private:
	int GetBestTarget( D3DXVECTOR3* vPosOut );
	int FindBestTarget( D3DXVECTOR3* vPosOut, bool PrioritizeHealth );

	bool IsInFOV( float angle, const D3DXVECTOR3& camera, const D3DXVECTOR3& target );
	float GetFOV( const D3DXVECTOR3& viewangles, const D3DXVECTOR3& vTargetPos, const D3DXVECTOR3& vCameraPos );
	bool GetClosestBone( HOBJECT obj, D3DXVECTOR3* BoneOut );

public:
	CAimbot()
	{
		m_bZombieObj = NULL;
		m_bValidZombie = false;
		m_pCurrentWeapon = nullptr;
		m_pLocalPlayer = nullptr;
		m_pPlayerClient = nullptr;
		m_iCurrentTarget = INVALID_AIM_TARGET;

		m_iCurWeaponType = 0;
		m_iAimFOV = 10.0f;
	}

	~CAimbot() = default;

	void Update();
	void UpdateZombies();

	bool HandleIntersect( CIntersectQuery* query, CIntersectInfo* info );
	void HandleTrigger( CIntersectQuery* query, CIntersectInfo* info );

	auto& GetCurAimedBone()
	{
		return m_CurAimedBone;
	}

	inline void RenderScene()
	{
		if ( !g_pLTClientShell )
			return;

		if ( g_pLTClientShell->bInGame <= 0 )
			return;

		if ( !config.Aimbot[ m_iCurWeaponType ].Enabled )
			return;

		if ( !config.Aimbot[ m_iCurWeaponType ].DrawFOV )
			return;

		int width = 0, height = 0;
		if ( engine.GetScreenSizes( &width, &height ) )
		{
			const float ViewFov = 80;
			const float Scale = tan( config.Aimbot[ m_iCurWeaponType ].FieldOfView / 180 * M_PI ) / tan( ( ViewFov / 2 ) / 180 * M_PI );
			const auto radius = ( Scale * width / 2 );

			render.RenderCircle( width / 2, height / 2, radius, 20, D3DCOLOR_RGBA( 255, 0, 255, 255 ), 1.5f );
		}
#ifdef _DEBUG
		char szPrint[ 512 ]{ };
		sprintf_s( szPrint, sizeof szPrint, "CurAimBone: %f %f %f", m_CurAimedBone.x, m_CurAimedBone.y, m_CurAimedBone.z );

		render.RenderText( szPrint, ImVec2( 15.f, 80.f ), 18.f, D3DCOLOR_XRGB( 255, 255, 255 ) );

		D3DXVECTOR3 Pos{ };
		if ( Tools::WorldToScreen( m_CurAimedBone, &Pos ) )
			render.RenderCircle( Pos.x, Pos.y, 20.f, 30, D3DCOLOR_XRGB( 115, 0, 255 ), 1.5f );
#endif
	}


	static bool __cdecl FilterObject( HOBJECT hObj, PVOID )
	{
		for ( int i = 0; i < MAX_PLAYERS_IN_ROOM; ++i )
		{
			CPlayer* fx = ( CPlayer* )g_pLTClientShell->GetPlayerByID( i );

			if ( fx && hObj == fx->Object )
				return true;
		}
		return false;
	}
};

extern CAimbot aimbot;