#pragma once

#pragma once

enum
{
	SHAPE_TRIANGLE_UPSIDEDOWN,
	SHAPE_TRIANGLE,
	SHAPE_CIRCLE,
	SHAPE_SQUARE
};

class CRadar2D
{
	bool WindowEnabled; 

public:
	CRadar2D()
	{
		WindowEnabled = false;
	}

	void RadarRange(float* x, float* y, float range);
	void CalcRadarPoint(D3DXVECTOR3 vOrigin, int& screenx, int& screeny);

	void OnRenderPlayer();
	void OnRender();

	bool &EnableWindow()
	{
		return WindowEnabled;
	}
};

extern CRadar2D radar;