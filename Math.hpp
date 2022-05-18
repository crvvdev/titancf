#pragma once

namespace Math
{
	inline float GetDistance(const D3DXVECTOR3& vFrom, const D3DXVECTOR3& vOther)
	{
		D3DXVECTOR3 delta{ };

		delta.x = vFrom.x - vOther.x;
		delta.y = vFrom.y - vOther.y;
		delta.z = vFrom.z - vOther.z;

		return sqrt(delta.x * delta.x + delta.y * delta.y + delta.z * delta.z);
	}

	inline void AngleNormalize(D3DXVECTOR3& angle)
	{
		while (angle.x > 89.0f)
			angle.x -= 180.f;

		while (angle.x < -89.0f)
			angle.x += 180.f;

		while (angle.y > 180.f)
			angle.y -= 360.f;

		while (angle.y < -180.f)
			angle.y += 360.f;

		angle.z = 0;

		/*if ( angle.y > 180.0f )
			angle.y = 180.0f;
		else if ( angle.y < -180.0f )
			angle.y = -180.0f;

		if ( angle.x > 89.0f )
			angle.x = 89.0f;
		else if ( angle.x < -89.0f )
			angle.x = -89.0f;

		angle.z = 0;*/
	}

	inline float VectorNormalize( D3DXVECTOR3& v )
	{
		float l = std::sqrt( ( v.x * v.x ) + ( v.y * v.y ) + ( v.z * v.z ) );
		float m = 1.f / ( l + std::numeric_limits< float >::epsilon() );

		*v *= m;

		return l;
	}

	/*inline float AngleNormalize(float angle)
	{
		while (angle < -180) angle += 360;
		while (angle > 180) angle -= 360;

		return angle;
	}*/

	inline void SinCos(float radians, float *sine, float *cosine)
	{
		_asm
		{
			fld		DWORD PTR[radians]
			fsincos

			mov edx, DWORD PTR[cosine]
			mov eax, DWORD PTR[sine]

			fstp DWORD PTR[edx]
			fstp DWORD PTR[eax]
		}
	}

	void VectorAngles(const float* forward, float* angles);
	void AngleVectors(const  float* angles, float* forward);
	void AngleVectors2(const D3DXVECTOR3& angles, D3DXVECTOR3& forward);
	void VectorAngles(D3DXVECTOR3 vTargetPos, D3DXVECTOR3 vCameraPos, D3DXVECTOR3& vAngles);
	void SmoothAngles(D3DXVECTOR3 MyViewAngles, D3DXVECTOR3 AimAngles, D3DXVECTOR3 &OutAngles, float Smoothing);
};