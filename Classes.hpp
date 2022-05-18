#pragma once

#include <cstdint>

#include <ltbasedefs.h>

#include <ltmatrix.h>
#include <ltvector.h>
#include <iltmath.h>

#include <iltcsbase.h>
#include <ltbasetypes.h>
#include <iltcommon.h>

#include <AutoMessage.h>

#define FLAG_VISIBLE			1
#define FLAG_RAYHIT				1 << 13

#define M_PI					3.14159265358979323846f
#define M_RADPI					57.295779513082f

#define M_PI_F					((float)(M_PI))

#define RAD2DEG( x )			( (float)(x) * (float)(180.f / M_PI_F) )
#define DEG2RAD( x )			( (float)(x) * (M_PI_F / (float)180.f) )

#define PITCH					0
#define YAW						1
#define ROLL					2

#define MAX_WEAPONS_COUNT		3698
#define MAX_NODES_COUNT			1999
#define MAX_PLAYERINFO_COUNT	2
#define MAX_PLAYERS_IN_ROOM		30

#define MOVING_FOWARD			256
#define MOVING_BACKWARD			512
#define MOVING_RIGHT			1024
#define MOVING_LEFT				2048

using namespace Globals;

typedef char( __thiscall* hGetLocalPlayerIndex )( std::uintptr_t );
extern hGetLocalPlayerIndex GetLocalPlayerIndex;

template< typename Function >
Function GetVFunc( PVOID Base, DWORD Index )
{
	PDWORD* VTablePointer = ( PDWORD* )Base;
	PDWORD VTableFunctionBase = *VTablePointer;
	DWORD dwAddress = VTableFunctionBase[ Index ];
	return ( Function )( dwAddress );
}

typedef struct
{
	BYTE pad[ 0x9C ];
} NODE_MGR_STRUCT;

extern NODE_MGR_STRUCT* BackupNodes[ MAX_NODES_COUNT ];

typedef struct
{
	BYTE pad[ 0xA8 ];
} PLAYER_BASICINFO_STRUCT;

extern PLAYER_BASICINFO_STRUCT* BackupPlayerInfo[ MAX_PLAYERINFO_COUNT ];

typedef struct
{
	BYTE pad[ 0x5400 ];
} WEAPON_MGR_STRUCT;

extern WEAPON_MGR_STRUCT* BackupWeapons[ MAX_WEAPONS_COUNT ];

struct CTransform
{
	D3DXVECTOR3 Pos;
	char stack_guard[ 0x100 ]{};
};

struct CIntersectQuery
{
	CIntersectQuery()
	{
		m_From = D3DXVECTOR3( 0.f, 0.f, 0.f );
		m_To = D3DXVECTOR3( 0.f, 0.f, 0.f );
		m_Plane = D3DXVECTOR3( 0.f, 0.f, 0.f );
		m_Flags = NULL;
		m_FilterFn = NULL;
		m_FilterActualIntersectFn = NULL;
		m_PolyFilterFn = NULL;
		m_pUserData = NULL;
		m_pActualIntersectUserData = NULL;

		memset( m_Padding, 0, 100 );
	}

	D3DXVECTOR3			m_From;									/* 0x00 - 0x0C */
	D3DXVECTOR3			m_To;									/* 0x0C - 0x18 */
	D3DXVECTOR3			m_Plane;								/* 0x18 - 0x24 */
	int					m_Flags;								/* 0x24 - 0x28 */
	ObjectFilterFn		m_FilterFn;								/* 0x28 - 0x2C */
	ObjectFilterFn		m_FilterActualIntersectFn;				/* 0x2C - 0x30 */
	PolyFilterFn		m_PolyFilterFn;							/* 0x30 - 0x34 */
	void* m_pUserData;							/* 0x34 - 0x38 */
	void* m_pActualIntersectUserData;				/* 0x38 - 0x3C */
	unsigned char		m_Padding[ 100 ];
};

struct CIntersectInfo
{
	CIntersectInfo()
	{
		m_Point = D3DXVECTOR3( 0.f, 0.f, 0.f );
		m_hObject = NULL;
		m_hPoly = DWORD();
		m_SurfaceFlags = NULL;
		m_Unknown = NULL;

		memset( m_Padding, 0, 100 );
	}

	D3DXVECTOR3			m_Point;								/* 0x00 - 0x0C */
	char				unknown12[ 16 ];
	HOBJECT				m_hObject;								/* 0x1C - 0x20 */
	DWORD				m_hPoly;								/* 0x20 - 0x28 */
	uint32_t			m_SurfaceFlags;							/* 0x28 - 0x2C */
	uint32_t			m_Unknown;								/* 0x2C - 0x30 */
	BYTE				m_Padding[ 100 ];
};

typedef bool( WINAPIV* hIntersectSegment )( CIntersectQuery&, CIntersectInfo* );
extern hIntersectSegment oIntersectSegment;

typedef LTRESULT( __cdecl* hRenderCamera )( HLOCALOBJ hCamera, float fFrameTime );
extern hRenderCamera oRenderCamera;

typedef LTRESULT( __stdcall* hGetObjectDim )( HOBJECT, LTVector* );
extern hGetObjectDim oGetObjectDim;

enum PLAYER_HITBOXES
{
	PELVIS = 2,
	SPINE = 3,
	NECK = 5,
	HEAD = 6
};

class CLTCommon
{
public:
	LTRESULT CreateMessage( ILTMessage_Write*& pMsg )
	{
		typedef LTRESULT( __thiscall* fnCreateMessage )( void*, ILTMessage_Write*& );
		return GetVFunc<fnCreateMessage>( this, 9 )( this, pMsg );
	}

	LTRESULT GetObjectFlags( HOBJECT hObj, ObjFlagType flagType, uint32_t& dwFlags )
	{
		typedef LTRESULT( __thiscall* fnGetObjectFlags )( void*, HOBJECT, ObjFlagType, uint32_t& );
		return GetVFunc<fnGetObjectFlags>( this, 17 )( this, hObj, flagType, dwFlags );
	}

	LTRESULT SetObjectFlags( HOBJECT hObj, const ObjFlagType flagType, uint32_t dwFlags, uint32_t dwMask )
	{
		typedef LTRESULT( __thiscall* fnSetObjectFlags )( void*, HOBJECT, const ObjFlagType, uint32_t, uint32_t );
		return GetVFunc<fnSetObjectFlags>( this, 18 )( this, hObj, flagType, dwFlags, dwMask );
	}
};

class CLTPhysics
{
public:
	LTRESULT GetObjectDims( HOBJECT hObj, LTVector* d )
	{
		typedef LTRESULT( __thiscall* fnGetObjectDims )( HOBJECT, LTVector* );
		return GetVFunc<fnGetObjectDims>( this, 14 )( hObj, d );
	}
};

typedef enum
{
	CM_None = 0,
	CM_Hardware
} CursorMode;

class CLTCursor
{
public:
	virtual void		function0();
	virtual LTRESULT    SetCursorMode( CursorMode cMode, bool bForce = false );
};

class CLTClient
{
public:
	//29/04
	char pad_0000[ 144 ]; //0x0000
	bool( __cdecl* IntersectSegment )( const CIntersectQuery&, CIntersectInfo* ); //0x0090
	char pad_0090[ 56 ]; //0x0090
	LTRESULT( __cdecl* FlipScreen )( uint32_t flags ); //0x00CC
	LTRESULT( __cdecl* Start3D )( ); //0x00D0
	LTRESULT( __cdecl* RenderCamera )( HLOCALOBJ hCamera, float fFrameTime );  //0x00D4
	char pad_00D4[ 20 ]; //0x00D8
	LTRESULT( __cdecl* StartOptimized2D )( );//0x00EC
	LTRESULT( __cdecl* EndOptimized2D )( ); //0x00F0
	char pad_00F0[ 8 ]; //0x00F0
	LTRESULT( __cdecl* End3D )( uint32_t flags );//0x00F8
	char pad_00FC[ 316 ]; //0x00FC
	LTRESULT( __cdecl* RunConsoleCommand )( const char* ); //0x0238

	CLTCommon* GetLTCommon()
	{
		typedef CLTCommon* ( __thiscall* ILTCommonFn )( void* );
		return GetVFunc<ILTCommonFn>( this, 1 )( this );
	}

	CLTPhysics* GetLTPhysics()
	{
		typedef CLTPhysics* ( __thiscall* CLTPhysicsFn )( void* );
		return GetVFunc<CLTPhysicsFn>( this, 2 )( this );
	}

	/*LTRESULT SetObjectPos(HOBJECT hObj, const D3DXVECTOR3* pPos, bool bForce = true)
	{
		typedef LTRESULT(__thiscall* SetObjectPosFn)(void*, HOBJECT hObj, const D3DXVECTOR3* pPos, bool bForce);
		return GetVFunc<SetObjectPosFn>(this, 58)(this, hObj, pPos, bForce);
	}*/

	LTRESULT SendToServer( ILTMessage_Read* pMsg, uint32 Flags )
	{
		typedef LTRESULT( __thiscall* SendToServerFn )( void*, ILTMessage_Read*, uint32 );
		return GetVFunc<SendToServerFn>( this, 124 )( this, pMsg, Flags );
	}

	LTRESULT GetObjectPos( HLOCALOBJ hObj, D3DXVECTOR3* vPos )
	{
		typedef signed int( __thiscall* GetObjectPosFn )( void*, HLOCALOBJ hObj, D3DXVECTOR3* vPos );
		return GetVFunc<GetObjectPosFn>( this, 40 )( this, hObj, vPos );
	}

	CLTCursor* GetLTCursor()
	{
		typedef CLTCursor* ( __thiscall* CLTCursorFn )( void* );
		return GetVFunc<CLTCursorFn>( this, 90 )( this );
	}

	LTRESULT GetSConValueFloat( const char* szCommand, float* ValueOut )
	{
		typedef LTRESULT( __thiscall* GetSConValueFloatFn )( void*, const char*, float* );
		return GetVFunc<GetSConValueFloatFn>( this, 114 )( this, szCommand, ValueOut );
	}
};

class LTObject
{
public:
	char spacer00[ 4 ];
	D3DXVECTOR3 Maxes;
	D3DXVECTOR3 Mins;
	char spacer01[ 196 ];
	D3DXVECTOR3 Position;

public:
	FORCEINLINE D3DXVECTOR3 GetMins( void )
	{
		return ( this != nullptr ) ? Mins : D3DXVECTOR3( 0, 0, 0 );
	}
	FORCEINLINE D3DXVECTOR3 GetMaxes( void )
	{
		return ( this != nullptr ) ? Maxes : D3DXVECTOR3( 0, 0, 0 );
	}
	FORCEINLINE D3DXVECTOR3 GetPos( void )
	{
		return ( this != nullptr ) ? Position : D3DXVECTOR3( 0, 0, 0 );
	}

	FORCEINLINE D3DXVECTOR3 GetVelocity( void )
	{
		return *( D3DXVECTOR3* )( ( DWORD )this + 0xFC );
	}
};

class CLTModel
{
public:
	signed int GetSocket2( int* hObj, const char* pSocketName, uint32_t& hSocket )
	{
		typedef signed int( __thiscall* GetSocket2Fn )( void*, int* hObj, const char* pSocketName, uint32_t& hSocket );
		return GetVFunc<GetSocket2Fn>( this, 6 )( this, hObj, pSocketName, hSocket );
	}

	signed int GetSocket( int* hObj, const char* pSocketName, uint32_t& hSocket )
	{
		typedef signed int( __thiscall* GetSocketFn )( void*, int* hObj, const char* pSocketName, uint32_t& hSocket );
		return GetVFunc<GetSocketFn>( this, 7 )( this, hObj, pSocketName, hSocket );
	}

	signed int GetSocketTransform( int* hObj, uint32_t hSocket, CTransform& transform, bool bWorldSpace )
	{
		typedef signed int( __thiscall* GetSocketTransformFn )( void*, int* hObj, uint32_t hSocket, CTransform& transform, bool bWorldSpace );
		return GetVFunc<GetSocketTransformFn>( this, 8 )( this, hObj, hSocket, transform, bWorldSpace );
	}

	signed int GetPiece( int* hObj, const char* pPieceName, uint32_t& hPiece )
	{
		typedef signed int( __thiscall* GetPieceFn )( void*, int* hObj, const char* pPieceName, uint32_t& hPiece );
		return GetVFunc<GetPieceFn>( this, 10 )( this, hObj, pPieceName, hPiece );
	}

	signed int GetPieceHideStatus( int* hObj, uint32_t hPiece, bool& bHidden )
	{
		typedef signed int( __thiscall* GetPieceHideStatusFn )( void*, int* hObj, uint32_t hPiece, bool& bHidden );
		return GetVFunc<GetPieceHideStatusFn>( this, 11 )( this, hObj, hPiece, bHidden );
	}

	signed int SetPieceHideStatus( int* hObj, uint32_t hPiece, bool bHidden )
	{
		typedef signed int( __thiscall* SetPieceHideStatusFn )( void*, int* hObj, uint32_t hPiece, bool bHidden );
		return GetVFunc<SetPieceHideStatusFn>( this, 12 )( this, hObj, hPiece, bHidden );
	}

	signed int GetNode( HOBJECT hObj, const char* pNodeName, UINT& hNode )
	{
		typedef signed int( __thiscall* GetNodeFn )( void*, HOBJECT hObj, const char* pNodeName, UINT& hNode );
		return GetVFunc<GetNodeFn>( this, 13 )( this, hObj, pNodeName, hNode );
	}

	signed int GetNodeName( int* hObj, UINT hNode, char* name, unsigned int maxlen )
	{
		typedef signed int( __thiscall* GetNodeNameFn )( void*, int* hObj, UINT hNode, char* name, unsigned int maxlen );
		return GetVFunc<GetNodeNameFn>( this, 14 )( this, hObj, hNode, name, maxlen );
	}

	signed int GetNodeTransform( HOBJECT hObj, HMODELNODE hNode, CTransform* transform, bool bWorldSpace )
	{
		typedef signed int( __thiscall* GetNodeTransformFn )( void*, HOBJECT hObj, HMODELNODE hNode, CTransform* transform, bool bWorldSpace );
		return GetVFunc<GetNodeTransformFn>( this, 15 )( this, hObj, hNode, transform, bWorldSpace );
	}

	signed int GetNextNode( HOBJECT hObj, unsigned int hNode, unsigned int& pNext )
	{
		typedef signed int( __thiscall* GetNextNodeFn )( void*, HOBJECT hObj, unsigned int hNode, unsigned int& pNext );
		return GetVFunc<GetNextNodeFn>( this, 16 )( this, hObj, hNode, pNext );
	}

	signed int GetNumChildren( HOBJECT hObj, unsigned int hNode, unsigned int& NumChildren )
	{
		typedef signed int( __thiscall* GetNumChildrenFn )( void*, HOBJECT hObj, unsigned int hNode, unsigned int& NumChildren );
		return GetVFunc<GetNumChildrenFn>( this, 18 )( this, hObj, hNode, NumChildren );
	}

	signed int GetChild( HOBJECT hObj, unsigned int parent, unsigned int index, unsigned int& child )
	{
		typedef signed int( __thiscall* GetChildFn )( void*, HOBJECT hObj, unsigned int parent, unsigned int index, unsigned int& child );
		return GetVFunc<GetChildFn>( this, 19 )( this, hObj, parent, index, child );
	}

	signed int GetParent( HOBJECT hObj, unsigned int node, unsigned int& parent )
	{
		typedef signed int( __thiscall* GetParentFn )( void*, HOBJECT hObj, unsigned int node, unsigned int& parent );
		return GetVFunc<GetParentFn>( this, 20 )( this, hObj, node, parent );
	}

	signed int GetNumNodes( HOBJECT hObj, unsigned int& num_nodes )
	{
		typedef signed int( __thiscall* GetNumNodesFn )( void*, HOBJECT hObj, unsigned int& num_nodes );
		return GetVFunc<GetNumNodesFn>( this, 21 )( this, hObj, num_nodes );
	}

	signed int UpdateMainTracker( HOBJECT hObj, float fUpdateDelta )
	{
		typedef signed int( __thiscall* UpdateMainTrackerFn )( void*, HOBJECT hObj, float fUpdateDelta );
		return GetVFunc<UpdateMainTrackerFn>( this, 27 )( this, hObj, fUpdateDelta );
	}

	signed int GetWeightSet2( int* hObj, uint32_t TrackerID, uint32_t& hSet )
	{
		typedef signed int( __thiscall* GetWeightSetFn )( void*, int* hObj, uint32_t TrackerID, uint32_t& hSet );
		return GetVFunc<GetWeightSetFn>( this, 31 )( this, hObj, TrackerID, hSet );
	}

	signed int GetPlaybackState( int* hObj, uint32_t TrackerID, uint32_t& flags )
	{
		typedef signed int( __thiscall* GetPlaybackStateFn )( void*, int* hObj, uint32_t TrackerID, uint32_t& flags );
		return GetVFunc<GetPlaybackStateFn>( this, 33 )( this, hObj, TrackerID, flags );
	}

	signed int AddTracker( int* hObj, uint32_t TrackerID )
	{
		typedef signed int( __thiscall* AddTrackerFn )( void*, int* hObj, uint32_t TrackerID );
		return GetVFunc<AddTrackerFn>( this, 34 )( this, hObj, TrackerID );
	}

	signed int RemoveTracker( int* hObj, uint32_t TrackerID )
	{
		typedef signed int( __thiscall* RemoveTrackerFn )( void*, int* hObj, uint32_t TrackerID );
		return GetVFunc<RemoveTrackerFn>( this, 35 )( this, hObj, TrackerID );
	}

	signed int GetAnimIndex( int* hObj, const char* pAnimName, unsigned int& anim_index )
	{
		typedef signed int( __thiscall* GetAnimIndexFn )( void*, int* hObj, const char* pAnimName, unsigned int& anim_index );
		return GetVFunc<GetAnimIndexFn>( this, 36 )( this, hObj, pAnimName, anim_index );
	}

	signed int GetCurAnim( int* hObj, uint32_t TrackerID, uint32_t& Index )
	{
		typedef signed int( __thiscall* GetCurAnimFn )( void*, int* hObj, uint32_t TrackerID, uint32_t& Index );
		return GetVFunc<GetCurAnimFn>( this, 38 )( this, hObj, TrackerID, Index );
	}

	signed int SetCurAnim( int* hObj, UINT8 TrackerID, UINT8 Index )
	{
		typedef signed int( __thiscall* SetCurAnimFn )( void*, int* hObj, UINT8 TrackerID, UINT8 Index );
		return GetVFunc<SetCurAnimFn>( this, 39 )( this, hObj, TrackerID, Index );
	}

	signed int ResetAnim( int* hObj, UINT8 TrackerID )
	{
		typedef signed int( __thiscall* ResetAnimFn )( void*, int* hObj, UINT8 TrackerID );
		return GetVFunc<ResetAnimFn>( this, 40 )( this, hObj, TrackerID );
	}

	signed int GetLooping( int* hObj, UINT8 TrackerID )
	{
		typedef signed int( __thiscall* GetLoopingFn )( void*, int* hObj, UINT8 TrackerID );
		return GetVFunc<GetLoopingFn>( this, 41 )( this, hObj, TrackerID );
	}

	signed int SetLooping( int* hObj, UINT8 TrackerID, bool bLooping )
	{
		typedef signed int( __thiscall* SetLoopingFn )( void*, int* hObj, UINT8 TrackerID, bool bLooping );
		return GetVFunc<SetLoopingFn>( this, 42 )( this, hObj, TrackerID, bLooping );
	}

	signed int GetPlaying( int* hObj, UINT8 TrackerID )
	{
		typedef signed int( __thiscall* GetPlayingFn )( void*, int* hObj, UINT8 TrackerID );
		return GetVFunc<GetPlayingFn>( this, 43 )( this, hObj, TrackerID );
	}

	signed int SetPlaying( int* hObj, UINT8 TrackerID, bool bPlaying )
	{
		typedef signed int( __thiscall* SetPlayingFn )( void*, int* hObj, UINT8 TrackerID, bool bPlaying );
		return GetVFunc<SetPlayingFn>( this, 44 )( this, hObj, TrackerID, bPlaying );
	}

	signed int GetCurAnimLength( int* hObj, UINT8 TrackerID, unsigned int& length )
	{
		typedef signed int( __thiscall* GetCurAnimLengthFn )( void*, int* hObj, UINT8 TrackerID, unsigned int& length );
		return GetVFunc<GetCurAnimLengthFn>( this, 45 )( this, hObj, TrackerID, length );
	}

	signed int GetCurAnimTime( int* hObj, UINT8 TrackerID, unsigned int& curTime )
	{
		typedef signed int( __thiscall* GetCurAnimTimeFn )( void*, int* hObj, UINT8 TrackerID, unsigned int& curTime );
		return GetVFunc<GetCurAnimTimeFn>( this, 46 )( this, hObj, TrackerID, curTime );
	}

	signed int SetCurAnimTime( int* hObj, UINT8 TrackerID, unsigned int curTime )
	{
		typedef signed int( __thiscall* SetCurAnimTimeFn )( void*, int* hObj, UINT8 TrackerID, unsigned int curTime );
		return GetVFunc<SetCurAnimTimeFn>( this, 47 )( this, hObj, TrackerID, curTime );
	}

	signed int SetAnimRate( int* hObj, UINT8 TrackerID, float fRate )
	{
		typedef signed int( __thiscall* SetAnimRateFn )( void*, int* hObj, UINT8 TrackerID, float fRate );
		return GetVFunc<SetAnimRateFn>( this, 49 )( this, hObj, TrackerID, fRate );
	}

	signed int GetAnimRate( int* hObj, UINT8 TrackerID, float& fRate )
	{
		typedef signed int( __thiscall* GetAnimRateFn )( void*, int* hObj, UINT8 TrackerID, float& fRate );
		return GetVFunc<GetAnimRateFn>( this, 50 )( this, hObj, TrackerID, fRate );
	}

	signed int GetWeightSet( int* hObj, uint32_t TrackerID, uint32_t& hSet )
	{
		typedef signed int( __thiscall* GetWeightSetFn )( void*, int* hObj, uint32_t TrackerID, uint32_t& hSet );
		return GetVFunc<GetWeightSetFn>( this, 51 )( this, hObj, TrackerID, hSet );
	}

	signed int SetWeightSet( int* hObj, uint32_t TrackerID, uint32_t hSet )
	{
		typedef signed int( __thiscall* SetWeightSetFn )( void*, int* hObj, uint32_t TrackerID, uint32_t hSet );
		return GetVFunc<SetWeightSetFn>( this, 52 )( this, hObj, TrackerID, hSet );
	}

	signed int GetNumLODs( int* hObj, uint32_t hPiece, uint32_t& num_lods )
	{
		typedef signed int( __thiscall* GetNumLODsFn )( void*, int* hObj, uint32_t hPiece, uint32_t& num_lods );
		return GetVFunc<GetNumLODsFn>( this, 53 )( this, hObj, hPiece, num_lods );
	}

	signed int ApplyAnimations( int* hObj )
	{
		typedef signed int( __thiscall* ApplyAnimationsFn )( void*, int* hObj );
		return GetVFunc<ApplyAnimationsFn>( this, 55 )( this, hObj );
	}

	signed int GetModelOBBCopy( int* hObj, struct ModelOBB* hOBB )
	{
		typedef signed int( __thiscall* GetModelOBBCopyFn )( void*, int* hObj, struct ModelOBB* hOBB );
		return GetVFunc<GetModelOBBCopyFn>( this, 61 )( this, hObj, hOBB );
	}

	signed int GetFilenames( HOBJECT hObj, char* pFilename, uint32 file_name_buf_size, char* pSkinname, uint32 skin_name_buf_size )
	{
		typedef signed int( __thiscall* GetFilenamesFn )( void*, HOBJECT hObj, char* pFilename, uint32 file_name_buf_size, char* pSkinname, uint32 skin_name_buf_size );
		return GetVFunc<GetFilenamesFn>( this, 56 )( this, hObj, pFilename, file_name_buf_size, pSkinname, skin_name_buf_size );
	}
};

enum WeaponType
{
	Pistol = 0,
	Shotgun = 1,
	SMG = 2,
	Rifle = 3,
	Sniper = 4,
	MachineGun = 5,
	Grenades = 6,
	Knife = 7,
	C4 = 9,
	RAPPEL = 10
};

enum FireType
{
	SingleFire = 1,
	RepeatFire = 2,
	ShrapnelFire = 4,
	DelayFire = 8,
	AlternateFire = 16
};

#define WEAPON_FOV_OFF  0x305C
#define WEAPON_RELOAD_ANIM_RATIO  0x125C
#define WEAPON_CHANGE_ANIM_RATIO  0x1260
#define WEAPON_RANGE 0xBF0
#define WEAPON_KNOCKBACK  0x324C
#define WEAPON_BULLET_POS_OFF  0x2B5C
#define WEAPON_SHOT_REACT_PITCH  0x2D3C
#define WEAPON_SHOT_REACT_YAW  0x2B68
#define WEAPON_DETAIL_PERTURB_SHOT1  0x12D8
#define WEAPON_DETAIL_PERTURB_SHOT2  0x12EC
#define WEAPON_DETAIL_PERTURB_SHOT3  0x1300
#define WEAPON_DETAIL_REACT_PITH_SHOT1  0x1314
#define WEAPON_DETAIL_REACT_PITH_SHOT2  0x1328
#define WEAPON_DETAIL_REACT_PITH_SHOT3  0x133C
#define WEAPON_DETAIL_REACT_YAW_SHOT1  0x1350
#define WEAPON_DETAIL_REACT_YAW_SHOT2  0x1364
#define WEAPON_DETAIL_REACT_YAW_SHOT3  0x1378
#define WEAPON_CHAIR_PER_REAL_SIZE  0x3144
#define WEAPON_SUBTYPE  0xC0D
#define WEAPON_PERTURB_MIN  0xB78
#define WEAPON_PERTURB_MAX  0xBB4
#define WEAPON_REPEAT_FIRE  0xF94 

class CWeapon
{
public:
	int16_t WeaponID; //0x0000
	int8_t Class; //0x0002
	char pad_0003[ 11 ]; //0x0003
	char WeaponName[ 28 ]; //0x000E

	int16_t GetWeaponIndex()
	{
		return *( int16_t* )( ( uintptr_t )this );
	}

	BYTE GetWeaponClass()
	{
		return *( BYTE* )( ( uintptr_t )this + 0x2 );
	}

	FireType& GetFireType()
	{
		return *( FireType* )( ( uintptr_t )this + 0xF94 );
	}

	float& GetWeaponFOV()
	{
		return *( float* )( ( uintptr_t )this + WEAPON_FOV_OFF );//
	}

	float& ReloadAnimRatio()
	{
		return *( float* )( ( uintptr_t )this + WEAPON_RELOAD_ANIM_RATIO );//
	}

	float& ChangeWeaponAnimRatio()
	{
		return *( float* )( ( uintptr_t )this + WEAPON_CHANGE_ANIM_RATIO );//
	}

	int& RepeatFire()
	{
		return *( int* )( ( uintptr_t )this + WEAPON_REPEAT_FIRE );//
	}

	float* FastKnife1()
	{
		return ( float* )( ( uintptr_t )this + 0xEF0 );//
	}

	float* FastKnife2()
	{
		return ( float* )( ( uintptr_t )this + 0xF50 );//
	}

	float* FastKnife3()
	{
		return ( float* )( ( uintptr_t )this + 0xF20 );//
	}

	float* FastKnife4()
	{
		return ( float* )( ( uintptr_t )this + 0xF80 );//
	}

	float* LinkAttackAniRatio()
	{
		return ( float* )( ( uintptr_t )this + 0x42E0 );//
	}

	float& Range()
	{
		return *( float* )( ( uintptr_t )this + WEAPON_RANGE );//
	}

	float* KnockBack()
	{
		return ( float* )( ( uintptr_t )this + WEAPON_KNOCKBACK );//
	}

	float* BulletPosOffset()
	{
		return ( float* )( ( uintptr_t )this + WEAPON_BULLET_POS_OFF );//
	}

	float* ShotReactPitch()
	{
		return ( float* )( ( uintptr_t )this + WEAPON_SHOT_REACT_PITCH );//
	}

	float* ShotReactYaw()
	{
		return ( float* )( ( uintptr_t )this + WEAPON_SHOT_REACT_YAW );//
	}

	float* DetailPerturbShot()
	{
		return ( float* )( ( uintptr_t )this + WEAPON_DETAIL_PERTURB_SHOT1 );//
	}

	float* DetailPerturbShot2()
	{
		return ( float* )( ( uintptr_t )this + WEAPON_DETAIL_PERTURB_SHOT2 );//
	}

	float* DetailPerturbShot3()
	{
		return ( float* )( ( uintptr_t )this + WEAPON_DETAIL_PERTURB_SHOT3 );//
	}

	float* DetailReactPitchShot()
	{
		return ( float* )( ( uintptr_t )this + WEAPON_DETAIL_REACT_PITH_SHOT1 );//
	}

	float* DetailReactPitchShot2()
	{
		return ( float* )( ( uintptr_t )this + WEAPON_DETAIL_REACT_PITH_SHOT2 );//
	}

	float* DetailReactPitchShot3()
	{
		return ( float* )( ( uintptr_t )this + WEAPON_DETAIL_REACT_PITH_SHOT3 );//
	}

	float* DetailReactYawShot()
	{
		return ( float* )( ( uintptr_t )this + WEAPON_DETAIL_REACT_YAW_SHOT1 );//
	}

	float* DetailReactYawShot2()
	{
		return ( float* )( ( uintptr_t )this + WEAPON_DETAIL_REACT_YAW_SHOT2 );//
	}

	float* DetailReactYawShot3()
	{
		return ( float* )( ( uintptr_t )this + WEAPON_DETAIL_REACT_YAW_SHOT3 );//
	}

	float* PertubMin()
	{
		return ( float* )( ( uintptr_t )this + WEAPON_PERTURB_MIN );//
	}

	float* PertubMax()
	{
		return ( float* )( ( uintptr_t )this + WEAPON_PERTURB_MAX );//
	}

	float* CrossHairRatioPerRealSize()
	{
		return ( float* )( ( uintptr_t )this + WEAPON_CHAIR_PER_REAL_SIZE );//
	}

	BYTE SubType()
	{
		return *( BYTE* )( ( uintptr_t )this + WEAPON_SUBTYPE );//
	}
};

enum NodeType
{
	Normal,
	HeadshotGold,
	HeadshotSilver
};

enum mSubType
{
	SUBTYPE_MUTANT = 4
};

class CModelNode
{
public:
	BYTE			_0x00[ 36 ];
	FLOAT			DamageFactor;
	D3DXVECTOR3		RelativePos;
	INT32			Priority;
	D3DXVECTOR3		Dimension;
	D3DXVECTOR4		ObjectColor;
	INT32			ModelNodeType;
	BYTE			_0x58[ 68 ];

	char* GetName()
	{
		return *( char** )( ( uintptr_t )this + 0x4 );
	}
};

class CNanoCharacter
{
public:
	char pad_0000[ 4 ]; //0x0000
	class ModelInstance* UnknownObject; //0x0004
	class ModelInstance* NanoObject; //0x0008
	D3DXVECTOR3 Position; //0x000C
	char pad_0018[ 40 ]; //0x0018
	int32_t CurrAnim_UpperContext; //0x0040
	int32_t CurrAnim_LowerContext; //0x0044
	int32_t IsFiring; //0x0048
};

class CCharacterFX
{
public:
	//Att
	char pad_0000[ 160 ]; //0x0000
	union
	{
		LTObject* LObject; //0x00A0
		HOBJECT Object; //0x00A0
	};
	char pad_00A4[ 61028 ]; //0x00A4
	class CWeapon* CurrentWeapon; //0xEF08
	char pad_EF0C[ 24 ]; //0xEF0C
	int16_t WeaponID; //0xEF24
	char pad_EF26[ 42 ]; //0xEF26
	int32_t NanoType; //0xEF50

	D3DXVECTOR3& ViewAngles()
	{
		return *( D3DXVECTOR3* )( ( uintptr_t )this + 0xB4 );
	}

	bool IsNano()
	{
		return *( bool* )( ( uintptr_t )this + 0xEF20 );
	}

	bool IsSpawnProtected()
	{
		return *( bool* )( ( uintptr_t )this + 0x210 );
	}

	bool IsDead()
	{
		return *( bool* )( ( DWORD )this + 0x200 );
	}

	int ZMHealth()
	{
		//89 81 ? ? ? ? 8B 85 ? ? ? ? 89 81 ? ? ? ? C7 81 ? ? ? ? ? ? ? ? 89 81 ? ? ? ? E9 ? ? ? ? 
		return *( int* )( ( uintptr_t )this + 0x15604 );
	}

	bool IsMutant()
	{
		if ( !this )
			return false;

		if ( !CurrentWeapon )
			return false;

		if ( CurrentWeapon->SubType() == SUBTYPE_MUTANT )
			return true;
		/*}
		else
		{
			if ( IsNano() && NanoType != 2 )
				return true;
		}*/
		return false;
	}
};

class CPlayer
{
public:
	//Att
	int32_t LocalIndex; //0x0000
	HOBJECT Object; //0x0004
	uint8_t bClientID; //0x0008
	uint8_t bTeam; //0x0009
	char szName[ 12 ]; //0x000A
	char pad_0016[ 2 ]; //0x0016
	class CCharacterFX* CharacterFX; //0x0018
	uint8_t bAliveFlag; //0x001C
	char pad_001D[ 7 ]; //0x001D
	uint8_t bHasC4; //0x0024
	char pad_0025[ 27 ]; //0x0025
	uint16_t Health; //0x0040

	inline bool IsValidClient()
	{
		if ( this == nullptr )
			return false;

		if ( this->Object == nullptr )
			return false;

		if ( this->CharacterFX == nullptr )
			return false;

		if ( this->bAliveFlag <= 0 )
			return false;

		if ( this->Health <= 0 )
			return false;

		if ( strlen( this->szName ) <= 0 )
			return false;

		return true;
	}
}; //Size: 0x0D78

class CUNKOWN
{
public:
	char pad_0000[ 16 ]; //0x0000
	int32_t Knife; //0x0010
	char pad_0014[ 48 ]; //0x0014
}; //Size: 0x0044

class CAnimation
{
public:
	char pad_0000[ 4 ]; //0x0000
	class CUNKOWN* UNKOWN; //0x0004
	char pad_0008[ 252 ]; //0x0008
}; //Size: 0x0104

class CModelInstance
{
public:
	char pad_0000[ 5448 ]; //0x0000
	class CAnimation* Animation; //0x1548
	char pad_154C[ 3000 ]; //0x154C
}; //Size: 0x2104

class CPlayerViewManager
{
public:
	char pad_0000[ 4 ]; //0x0000
	class CModelInstance* ModelInstance; //0x0004
};

class CPlayerClient
{
public:
	void SetFireState( int mode )
	{
		typedef void( __thiscall* SetFireStateFn )( void*, int );
		return GetVFunc<SetFireStateFn>( this, 18 )( this, mode );
	}

	int GetFireState()
	{
		typedef int( __thiscall* GetFireStateFn )( void* );
		return GetVFunc<GetFireStateFn>( this, 19 )( this );
	}

	void FireWeapon( int unk )
	{
		typedef void( __thiscall* FireWeaponFn )( void*, int );
		return GetVFunc<FireWeaponFn>( this, 212 )( this, unk );
	}

	void SetLocalAngles( const D3DXVECTOR3& Angles );

	void SetCurrentWeapon( __int16 WeaponID, D3DXVECTOR3 vPos )
	{
		typedef int( __thiscall* SetCurWeaponFn )( void*, __int16, float, float, float );
		GetVFunc<SetCurWeaponFn>( this, 472 )( this, WeaponID, vPos.x, vPos.y, vPos.z );
	}

	void RapidFire()
	{
		typedef void( __thiscall* RapidFireFn )( void*, int );
		GetVFunc<RapidFireFn>( this, 118 )( this, 1 );
	}

public:
	//09/04
	char pad_0000[ 708 ]; //0x0000
	int32_t GunCurrentAmmo; //0x02C4
	int32_t GetMaxTotalAmmo; //0x02C8
	int32_t GunClipMaxAmmo; //0x02CC
	char pad_02D0[ 12 ]; //0x02D0
	class CPlayerViewManager* PlayerViewManager; //0x02DC
	char pad_02E0[ 636 ]; //0x02E0
	HOBJECT PlayerObject1; //0x055C
	HOBJECT PlayerObject2; //0x0560
	int32_t Flags; //0x0564
	float flPitch; //0x0568
	float flYaw; //0x056C
	char pad_0570[ 480 ]; //0x0570
	uint16_t WeaponID; //0x0750

	//class CPlayerViewManager* PlayerViewManager; //0x02DC
	//char pad_02E0[ 632 ]; //0x02E0
	//HOBJECT PlayerObject1; //0x0558
	//HOBJECT PlayerObject2; //0x055C
	//char pad_0560[ 4 ]; //0x0560
	//float flPitch; //0x0564
	//float flYaw; //0x0568
	//char pad_056C[ 480 ]; //0x056C
	//uint16_t WeaponID; //0x074C

	float& GetPitch();
	float& GetYaw();

	/*float& Pitch2()
	{
		return *( float* )( ( DWORD )this + 0xDDC );
	}

	float& Yaw2()
	{
		return *( float* )( ( DWORD )this + 0xDE0 );
	}*/
};

class CCameraInstance
{
public:
	char pad_0000[ 4 ]; //0x0000
	D3DXVECTOR3 vCameraObject; //0x0004
	D3DXVECTOR3 vCameraObject2; //0x0010
};

class CCameraBase
{
public:
	char pad_0000[ 12 ]; //0x0000
	int32_t Mirror; //0x000C
	char pad_0010[ 84 ]; //0x0010
	int32_t CameraMode; //0x0064
	char pad_0068[ 4 ]; //0x0068
	D3DXVECTOR3 CameraPos; //0x006C
	char pad_0078[ 20 ]; //0x0078
}; //Size: 0x008C

class CLTClientShell
{
public:
	char pad_0000[ 48 ]; //0x0000
	class CCameraBase* Camera; //0x0030
	char pad_0034[ 48 ]; //0x0034
	uint8_t bIsAlive; //0x0064
	char pad_0065[ 19 ]; //0x0065
	class CPlayerClient* PlayerClient; //0x0078
	uint8_t bInGame; //0x007C
	char pad_007D[ 15 ]; //0x007D
	class CLTClient* LTClient; //0x008C
	char pad_0090[ 52 ]; //0x0090
	class CCameraInstance* CameraInstance; //0x00C4

	FORCEINLINE class CPlayer* GetPlayerByID( int i )
	{
		if ( i < 0 || i > MAX_PLAYERS_IN_ROOM )
			return nullptr;

		return ( class CPlayer* )( ( uintptr_t )this + ( i * Globals::Offset::uPlayerSize ) + Globals::Offset::uPlayerOffset );
	}

	FORCEINLINE class CPlayer* GetLocalPlayer( void )
	{
		if ( !GetLocalPlayerIndex )
			return nullptr;

		return GetPlayerByID( GetLocalPlayerIndex( ( uintptr_t )this ) );
	}
};

class ObjectEntityList
{
public:
	DWORD ObjectBase; //0x0000 
	DWORD ObjectCount; //0x0004 

	FORCEINLINE DWORD GetAvailableObject( void )
	{
		return ( this != NULL ) ? ObjectCount : NULL;
	}
	FORCEINLINE DWORD GetObjectBase( void )
	{
		return ( this != NULL ) ? ObjectBase : NULL;
	}

	FORCEINLINE DWORD GetObjectList( int i )
	{
		return *( DWORD* )( GetObjectBase() ) + ( 4 * i );
	}
};

enum GAME_MODES : int
{
	//ZM
	ZA = 9,
	ZA2 = 23,
	ZA3 = 31,

	//Shadow
	Shadow = 16,

	//Common
	TDM = 1,
	SnD = 0,
	WeaponMaster = 35,
	BombingMode = 29,
	RapidSurgeMode = 20,
	GhostMode = 2,
	WipeOut = 6,
	FreeForAll = 3,
	EscapeMode = 8,
	SpyMode = 21,
	CaptainMode = 22,

	//Mutant
	HeroModeX = 12,
	ZombieKnightMode = 18,
	ZombieVsGhost = 17,
	ZombieMode = 4,
	HeroMode = 10,
	MutantChallenge = 47,

	//Wave
	WaveMode = 14,

	//SSD
	SuperSoldierTD = 19,
	SuperSoldierDM = 24,

	//BOT
	AIBotDestruction = 30,
	BotTD = 25,

	//Event
	CopsAndRobbers = 40,

	//GDM
	GDM = 51,
};

class CRoomInfo
{
public:
	int32_t RoomID; //0x0000
	char pad_0004[ 272 ]; //0x0004
	int32_t GameID; //0x0114
	char pad_0118[ 20 ]; //0x0118
	GAME_MODES GameMode; //0x012C
	char pad_0130[ 16 ]; //0x0130
	int32_t WeaponType; //0x0140
	char pad_0144[ 4 ]; //0x0144
	float RespawnTime; //0x0148
	int32_t MaxPlayerCount; //0x014C
};//Size=0x0130

class CRoomManager
{
public:
	char _0x0000[ 12 ];
	CRoomInfo* RoomInfo; //0x000C
};//Size=0x0010

class CTestValues
{
public:
	float DistFallDamageStartFrom; //0x0000
	float DamagePerMeter; //0x0004
	DWORD PercentageBuyShotgun; //0x0008
	DWORD PercentageBuySubMachineGun; //0x000C
	DWORD PercentageBuyRifle; //0x0010
	DWORD PercentageBuySniperRifle; //0x0014
	DWORD PercentageBuyMachineGun; //0x0018
	float AIReachPosAcceptableDist; //0x001C
	float AIReachPosAcceptableYDist; //0x0020
	float AICheckStruckInterval; //0x0024
	float AICheckStruckDist; //0x0028
	DWORD CheckEnemyIntervalLower; //0x002C
	DWORD CheckEnemyIntervalUpper; //0x0030
	DWORD DecideActionIntervalLower; //0x0034
	DWORD DecideActionIntervalUpper; //0x0038
	float PerturbLimitLowerLower; //0x003C
	float PerturbLimitLowerUpper; //0x0040
	float PerturbLimitUpperLower; //0x0044
	float PerturbLimitUpperUpper; //0x0048
	float PerturbLimitLowerMultiplierNormal; //0x004C
	float PerturbLimitUpperMultiplierNormal; //0x0050
	float CheckEnemyIntervalMultiplierNormal; //0x0054
	float DecideActionIntervalMultiplierNormal; //0x0058
	float PerturbLimitLowerMultiplierHigher; //0x005C
	float PerturbLimitUpperMultilpierHigher; //0x0060
	float CheckEnemyIntervalMultiplierHigh; //0x0064
	float DecideActionIntervalMultiplierHigh; //0x0068
	DWORD ReactionTimeLimitOnDamaged; //0x006C
	float LookAtAddAmountWhenDamagedX; //0x0070
	float LookAtAddAmountWhenDamagedY; //0x0074
	float LookAtAddAmountWhenDamagedZ; //0x0078
	DWORD ReloadTime; //0x007C
	DWORD TimeChangeRandomShootPointWhenBlinded; //0x0080
	DWORD AddAmountRandomShootPointWhenBlinedX; //0x0084
	DWORD AddAmountRandomShootPointWhenBlinedY; //0x0088
	DWORD AddAmountRandomShootPointWhenBlinedZ; //0x008C
	DWORD TimeChangeRandomGotoPointWhenBlinded; //0x0090
	DWORD AddAmountRandomGotoPointWhenBlinedX; //0x0094
	DWORD AddAmountRandomGotoPointWhenBlinedY; //0x0098
	DWORD AddAmountRandomGotoPointWhenBlinedZ; //0x009C
	DWORD PercentageLookOutWhenAttackTargetDisappeared; //0x00A0
	DWORD PercentageGotoPosWhenAttackTargetDisappeared; //0x00A4
	DWORD PercentageBuyWeaponWhenAlreadyHasOne; //0x00A8
	DWORD TimeToResetGunDirRotWhenMoving; //0x00AC
	DWORD TimeToResetGunDirRotWhenAttacking; //0x00B0
	DWORD MoveXZSpeedLimitNormal; //0x00B4
	DWORD MoveXZSpeedLimitLadder; //0x00B8
	DWORD RepeatJumpTimeLimit; //0x00BC
	float CheckEnemyInTheBackDotLimit; //0x00C0
	float CheckEnemyInTheBackDotLimitNormal; //0x00C4
	float CheckEnemyInTheBackDotLimitHigh; //0x00C8
	DWORD KillNumMultiplierWhenCheckingEnemyThreatened; //0xCC
	float VeryCloseDistWhenAttacking; //0x00D0
	float VeryFarDistWhenAttacking; //0x00D4
	DWORD PercentageJumpWhenAttack; //0x00D8
	DWORD PercentageJumpWhenMoving; //0x00DC
	DWORD AttackShootingSniperVariationTimeLower; //0x00E0
	DWORD AttackShootingSniperVariationTimeUpper; //0x00E4
	DWORD AttackShootingOtherVariationTimeLower; //0x00E8
	DWORD AttackShootingOtherVariationTimeUpper; //0x00EC
	DWORD AttackAimingSniperVariationTimeLower; //0x00F0
	DWORD AttackAimingSniperVariationTimeUpper; //0x00F4
	DWORD AttackAimingOtherVariationTimeLower; //0x00F8
	DWORD AttackAimingOtherVariationTimeUpper; //0x00FC
	DWORD AttackAimingOtherVariationTimeLowerVeryClose; //0x0100
	DWORD AttackAimingOtherVariationTimeUpperVeryClose; //0x0104
	DWORD PercentageRunningWhenMoving; //0x0108
	DWORD PercentageRunningBackwardWhenMoving; //0x010C
	DWORD PercentageWalkingWhenMoving; //0x0110
	DWORD MaxMoveDurationLower; //0x0114
	DWORD MaxMoveDurationUpper; //0x0118
};

class CBasicPlayerInfo
{
public:
	uint32_t BaseInfoPointer;//0x00
	float MovementSpeed;//0x04
	float MovementWalkRate;//0x08
	float MovementDuckWalkRate;//0x0C
	float MovementSideMoveRate;//0x10
	float MovementFBRunAnimRate;//0x14
	float MovementLRRunAnimRate;//0x18
	float MovementFBWalkAnimRate;//0x1C
	float MovementLRWalkAnimRate;//0x20
	float MovementAccelation;//0x24
	float MovementFriction;//0x28
	float JumpTime;//0x2C
	float JumpVelocity;//0x30
	float JumpLandedWaitTime;//0x34
	float JumpLandedNoJumpTimeRate;//0x38
	float JumpRepeatPenaltyMoveRate;//0x3C
	float JumpRepeatPenaltyHeightRate;//0x40
	float JumpLandedMovePenaltyTimeRate;//0x44
	float JumpLandedMovePenaltyMoveRate;//0x48
	float PVPosDefault;//0x4C
	float PVRotationDefault;//0x50
	float PVModelFOV;//0x54
	float PVModelAspect;//0x58
	char _0x0000[ 8 ];
	float PVOnlyMoveFlipTime;//0x64
	float PVOnlyMoveGap;//0x68
	float Unknown_00;//0x6C
	float Unknown_01;//0x70
	float DamagePenaltyTime;//0x74
	float DamagePenaltyMoveRate;//0x78
	float C4PlantTime;//0x7C
	float C4DefuseTime;//0x80
	float MaxCanDefuseDistance;//0x84
	float CharacterHiddenAlpha;//0x88
	float CharacterHiddenWalkAlpha;//0x8C
	float CharacterHiddenRunAlpha;//0x90
	float MovementHiddenRate;//0x94
};

inline CWeapon* GetBackupWeapon( int i )
{
	return ( CWeapon* )BackupWeapons[ i ];
}

inline CBasicPlayerInfo* GetBackupPlayer( int i )
{
	return ( CBasicPlayerInfo* )BackupPlayerInfo[ i ];
}

inline CModelNode* GetBackupNode( int i )
{
	return ( CModelNode* )BackupNodes[ i ];
}

using UpdateOutline_ = int( __thiscall* ) ( uintptr_t, HOBJECT );
extern UpdateOutline_ FnUpdateOutline;

using SetObjectOutline_ = LTRESULT( __stdcall* )( HOBJECT, bool, char, char, char );
extern SetObjectOutline_ FnSetOutline;

using GlowSetActive_ = void( __stdcall* )( bool );
extern GlowSetActive_ FnGlowSetActive;

using GetWeaponByIndex_ = PVOID( __cdecl* )( __int16 );
extern GetWeaponByIndex_ oGetWeaponByIndex;

using GetBasicPlayerInfo_ = PVOID( __cdecl* )( int );
extern GetBasicPlayerInfo_ oGetBasicPlayerInfo;

extern CLTClientShell* g_pLTClientShell;
extern CLTModel* g_pLTModel;
extern CLTClient* g_pLTClient;
extern CLTCommon* g_pLTCommon;