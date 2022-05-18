#include "Includes.hpp"

namespace Math
{
	void VectorAngles(const float* forward, float* angles)
	{
		float tmp, yaw, pitch;

		if (forward[2] == 0 && forward[0] == 0)
		{
			yaw = 0;

			if (forward[2] > 0)
				pitch = 90;
			else
				pitch = 270;
		}
		else
		{
			yaw = (atan2(forward[2], -forward[0]) * 180 / M_PI) - 90;

			if (yaw < 0)
				yaw += 360;

			tmp = sqrt(forward[0] * forward[0] + forward[2] * forward[2]);
			pitch = (atan2(forward[1], tmp) * 180 / M_PI);

			if (pitch < 0)
				pitch += 360;
		}

		angles[0] = -pitch;
		angles[1] = yaw;
		angles[2] = 0;
	}

	void VectorAngles(D3DXVECTOR3 vTargetPos, D3DXVECTOR3 vCameraPos, D3DXVECTOR3& vAngles)
	{
		D3DXVECTOR3 vDelta = vTargetPos - vCameraPos;

		VectorAngles((float*)&vDelta, (float*)&vAngles);

		AngleNormalize(vAngles);
	}

	void SmoothAngles(D3DXVECTOR3 MyViewAngles, D3DXVECTOR3 AimAngles, D3DXVECTOR3 &OutAngles, float Smoothing)
	{
		OutAngles = AimAngles - MyViewAngles;

		AngleNormalize(OutAngles);

		OutAngles.x = OutAngles.x / Smoothing + MyViewAngles.x;
		OutAngles.y = OutAngles.y / Smoothing + MyViewAngles.y;

		AngleNormalize(OutAngles);
	}

	void AngleVectors2(const D3DXVECTOR3& angles, D3DXVECTOR3& forward)
	{
		float	sp, sy, cp, cy;

		SinCos(DEG2RAD(angles[1]), &sy, &cy);
		SinCos(DEG2RAD(angles[0]), &sp, &cp);

		forward.x = cp * cy;
		forward.y = cp * sy;
		forward.z = -sp;
	}

	void AngleVectors(const  float* angles, float* forward)
	{
		/*float	sp, sy, cp, cy;

		SinCos(DEG2RAD(angles[1]), &sy, &cy);
		SinCos(DEG2RAD(angles[0]), &sp, &cp);

		forward[0] = cp * cy;
		forward[1] = cp * sy;
		forward[2] = -sp;*/
		float			angle;
		static float	sr, sp, sy, cr, cp, cy, t;

		angle = angles[YAW];
		sy = sin(angle);
		cy = cos(angle);

		angle = angles[PITCH];
		sp = sin(angle);
		cp = cos(angle);

		angle = angles[ROLL];
		sr = sin(angle);
		cr = cos(angle);

		if (forward)
		{
			forward[0] = cp * cy;
			forward[1] = cp * sy;
			forward[2] = -sp;
		}
	}
};