#pragma once

//#undef FLAG_VISIBLE
//#define FLAG_VISIBLE 0x1
//#undef FLAG_RAYHIT
//#define FLAG_RAYHIT 0x1000
//#undef FLAG_SOLID
//#define FLAG_SOLID 0x2000
//#undef FLAG_GRAVITY
//#define FLAG_GRAVITY 0x60000

class CEngine
{
public:
	CEngine() = default;
	~CEngine() = default;

	static bool __cdecl TraceFilter(HOBJECT Object, void* Data);

	bool IsVisible(HOBJECT Obj, D3DXVECTOR3& vTo);
	bool IsVisibleNoMask(D3DXVECTOR3& vTo);
	bool IsEntireVisible(HOBJECT Obj);
	bool IsBodyVisible(HOBJECT Obj);
	bool IsHeadVisible(HOBJECT Obj);
	//bool GetFirstVisBone(HOBJECT Obj, D3DXVECTOR3& vOut);

	void RunConsoleCmd(const CHAR* sVal);
	bool __cdecl GetBonePosition(HOBJECT Obj, const char* szBoneName, D3DXVECTOR3* vPos);
	
	bool ChatSendMessage(const char* msg);

	std::int16_t GetCurrentWeaponID();

	CModelNode* GetNodeByIndex(int i);
	CWeapon* GetWeaponByIndex(int i);
	CBasicPlayerInfo* GetPlayerInfo(int i);
	CTestValues* GetTestValues();
	CRoomInfo* GetRoomInfo();

	bool GetScreenSizes(int *w, int *h);
	
	bool IsMutantRoom();
	bool IsZMRoom();
	bool IsGDMRoom();
	bool IsInGame();
	bool IsSameTeam(CPlayer* you, CPlayer* enemy);
	bool IsPlayerMutant(CPlayer* other);

	void UseCursor(bool bUseCursor);

	std::uint8_t &GetS2SCounter();
};

extern CEngine engine;