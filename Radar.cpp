#include "Includes.hpp"

#include <set>
#include "ImGUI/imgui.h"
#include "ImGUI/imgui_internal.h"

CRadar2D radar;

void CRadar2D::RadarRange( float* x, float* y, float range )
{
	if ( fabs( ( *x ) ) > range || fabs( ( *y ) ) > range )
	{
		if ( ( *y ) > ( *x ) )
		{
			if ( ( *y ) > -( *x ) )
			{
				( *x ) = range * ( *x ) / ( *y );
				( *y ) = range;
			}
			else
			{
				( *y ) = -range * ( *y ) / ( *x );
				( *x ) = -range;
			}
		}
		else
		{
			if ( ( *y ) > -( *x ) )
			{
				( *y ) = range * ( *y ) / ( *x );
				( *x ) = range;
			}
			else
			{
				( *x ) = -range * ( *x ) / ( *y );
				( *y ) = -range;
			}
		}
	}
}

//void CRadar2D::CalcRadarPoint(D3DXVECTOR3 vOrigin, int& screenx, int& screeny)
//{
//	float dx = vOrigin.x - g_pLTClientShell->Camera->CameraPos.x;
//	float dy = vOrigin.y - g_pLTClientShell->Camera->CameraPos.y;
//
//	float fYaw = float(RAD2DEG(g_pLTClientShell->PlayerClient->Yaw) * (M_PI / 180.0));
//
//	float fsin_yaw = sin(fYaw);
//	float fminus_cos_yaw = -cos(fYaw);
//
//	float x = dy * fminus_cos_yaw + dx * fsin_yaw;
//	float y = dx * fminus_cos_yaw - dy * fsin_yaw;
//
//	float range = (float)3500;
//
//	RadarRange(&x, &y, range);
//
//	ImVec2 DrawPos = ImGui::GetCursorScreenPos();
//	ImVec2 DrawSize = ImGui::GetContentRegionAvail();
//
//	int rad_x = (int)DrawPos.x;
//	int rad_y = (int)DrawPos.y;
//
//	float r_siz_x = DrawSize.x;
//	float r_siz_y = DrawSize.y;
//
//	int x_max = (int)r_siz_x + rad_x - 5;
//	int y_max = (int)r_siz_y + rad_y - 5;
//
//	screenx = rad_x + ((int)r_siz_x / 2 + int(x / range * r_siz_x));
//	screeny = rad_y + ((int)r_siz_y / 2 + int(y / range * r_siz_y));
//
//	if (screenx > x_max)
//		screenx = x_max;
//
//	if (screenx < rad_x)
//		screenx = rad_x;
//
//	if (screeny > y_max)
//		screeny = y_max;
//
//	if (screeny < rad_y)
//		screeny = rad_y;
//}

void CRadar2D::CalcRadarPoint( D3DXVECTOR3 vOrigin, int& screenx, int& screeny )
{
	float delta_x = g_pLTClientShell->Camera->CameraPos.x - vOrigin.x;
	float delta_y = g_pLTClientShell->Camera->CameraPos.y - vOrigin.y;

	float cos_yaw = cos( g_pLTClientShell->PlayerClient->GetYaw() );
	float sin_yaw = sin( g_pLTClientShell->PlayerClient->GetYaw() );

	float x = ( delta_y * cos_yaw - delta_x * sin_yaw );
	float y = ( delta_x * cos_yaw + delta_y * sin_yaw );

	float range = ( float )3500;

	RadarRange( &x, &y, range );

	ImVec2 DrawPos = ImGui::GetCursorScreenPos();
	ImVec2 DrawSize = ImGui::GetContentRegionAvail();

	int rad_x = ( int )DrawPos.x;
	int rad_y = ( int )DrawPos.y;

	float r_siz_x = DrawSize.x;
	float r_siz_y = DrawSize.y;

	int x_max = ( int )r_siz_x + rad_x - 5;
	int y_max = ( int )r_siz_y + rad_y - 5;

	screenx = rad_x + ( ( int )r_siz_x / 2 + int( x * r_siz_x ) );
	screeny = rad_y + ( ( int )r_siz_y / 2 + int( y * r_siz_y ) );

	if ( screenx > x_max )
		screenx = x_max;

	if ( screenx < rad_x )
		screenx = rad_x;

	if ( screeny > y_max )
		screeny = y_max;

	if ( screeny < rad_y )
		screeny = rad_y;
}

void CRadar2D::OnRenderPlayer()
{
	float Alpha = 255.f;//(float)Settings::Radar::rad_Alpha / 255.f;

	CPlayer* pLocal = g_pLTClientShell->GetLocalPlayer();

	if ( !pLocal )
		return;

	for ( int i = 0; i < MAX_PLAYERS_IN_ROOM; i++ )
	{
		CPlayer* pPlayer = g_pLTClientShell->GetPlayerByID( i );

		if ( pPlayer && pPlayer->IsValidClient() )
		{
			if ( pPlayer->bClientID == pLocal->bClientID )
				continue;

			if ( pPlayer->bTeam == pLocal->bTeam )
				continue;

			int screenx = 0;
			int screeny = 0;

			float Color_R = 1.f;
			float Color_G = 0.f;
			float Color_B = 0.f;

			D3DXVECTOR3 vOrigin{ };
			g_pLTClient->GetObjectPos( pPlayer->Object, &vOrigin );

			CalcRadarPoint( vOrigin, screenx, screeny );

			ImDrawList* Draw = ImGui::GetWindowDrawList();

			Draw->AddRectFilled( ImVec2( ( float )screenx, ( float )screeny ),
				ImVec2( ( float )screenx + 5, ( float )screeny + 5 ),
				ImColor( Color_R, Color_G, Color_B, Alpha ) );
		}
	}
}

void CRadar2D::OnRender()
{
	if ( !g_pLTClientShell )
		return;

	if ( g_pLTClientShell->bInGame <= 0 )
		return;

	if ( !this->WindowEnabled )
		return;

	float prevAlpha = ImGui::GetStyle().Alpha;

	float Alpha = 225.f;//(float)Settings::Radar::rad_Alpha / 255.f;

	ImGui::GetStyle().Alpha = Alpha;

	ImGui::SetNextWindowSize( ImVec2( 200, 150 ), ImGuiCond_Once );
	if ( ImGui::Begin( XS( "Radar 2D" ), &this->WindowEnabled, ImGuiWindowFlags_NoCollapse ) )
	{
		ImDrawList* Draw = ImGui::GetWindowDrawList();

		ImVec2 DrawPos = ImGui::GetCursorScreenPos();
		ImVec2 DrawSize = ImGui::GetContentRegionAvail();

		Draw->AddLine(
			ImVec2( DrawPos.x + DrawSize.x / 2.f, DrawPos.y ),
			ImVec2( DrawPos.x + DrawSize.x / 2.f, DrawPos.y + DrawSize.y ),
			ImColor( 1.f, 1.f, 1.f, Alpha ) );

		Draw->AddLine(
			ImVec2( DrawPos.x, DrawPos.y + DrawSize.y / 2.f ),
			ImVec2( DrawPos.x + DrawSize.x, DrawPos.y + DrawSize.y / 2.f ),
			ImColor( 1.f, 1.f, 1.f, Alpha ) );

		OnRenderPlayer();

		ImGui::End();
	}

	ImGui::GetStyle().Alpha = prevAlpha;
}