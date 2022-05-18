#pragma once

struct AIObjectData
{
	CCharacterFX* pEntity;
	bool bIsValid;
};

class CAIObjectManager
{
private:
	std::vector< AIObjectData > vAIObjectList{ };
	box_data box{ };

public:
	auto& GetObjectList()
	{
		return vAIObjectList;
	}

	void Update();
};

extern void __stdcall AddObject( CCharacterFX* eax );
extern void __stdcall ClearObjectList();

extern CAIObjectManager AIObjects;