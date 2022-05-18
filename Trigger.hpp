#pragma once

class CTriggerbot
{
private:
	int GetBestTarget(CPlayer* m_pLocalPlayer);

public:
	//void Run(CPlayer* m_pLocalPlayer, int W, int H, bool AutoShoot, int Key, FireType FType);
	void Run(CPlayer* m_pLocalPlayer, CIntersectInfo* Info, CPlayerClient* m_pPlayerClient, bool AutoShoot, int Key, unsigned int Delay);
};

extern CTriggerbot trigger;