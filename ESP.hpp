#pragma once

#include <mutex>

struct box_data
{
	int x, y, w, h, centerX, centerY;
};

class CESP
{
private:
	box_data box;

public:
	void Render3DBox( CCharacterFX* enemy, DWORD col );
	void Draw2DRadar( CCharacterFX* enemy );

	void RenderScene();
	bool calculate_dynamic_box( LTObject* entity, box_data& box );
};

extern CESP esp;