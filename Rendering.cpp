#include "Includes.hpp"
#include "OpenSans.hpp"

CRenderer render;

ImFont* g_pESPFont = nullptr;
ImFont* g_pDefaultFont = nullptr;

std::mutex render_mutex;

void CRenderer::Initialize()
{
	ImGuiIO& io = ImGui::GetIO();

	char szFonts[ MAX_PATH ];
	if ( SUCCEEDED( SHGetFolderPathA( NULL, CSIDL_FONTS, NULL, 0, szFonts ) ) )
	{
		std::string Tahoma = szFonts;
		Tahoma += "\\consola.ttf";

		g_pESPFont = io.Fonts->AddFontFromFileTTF( Tahoma.c_str(), 18.0f, nullptr );
	}
	else
		g_pESPFont = ImGui::GetDefaultFont();

	g_pDefaultFont = io.FontDefault;
	
	/*ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(
		OpenSans_data,
		OpenSans_size,
		21.f );*/
}

void CRenderer::BeginScene()
{
	ImGuiIO& io = ImGui::GetIO();

	ImGui::PushStyleVar( ImGuiStyleVar_WindowBorderSize, 0.0f );
	ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, { 0.0f, 0.0f } );
	ImGui::PushStyleColor( ImGuiCol_WindowBg, { 0.0f, 0.0f, 0.0f, 0.0f } );
	ImGui::Begin( XS( "##Backbuffer" ), nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoInputs );

	ImGui::SetWindowPos( ImVec2( 0, 0 ), ImGuiCond_Always );
	ImGui::SetWindowSize( ImVec2( io.DisplaySize.x, io.DisplaySize.y ), ImGuiCond_Always );
}

void CRenderer::RenderScene()
{
	/*static bool theme = false;
	if (config.misc[18] && !theme)
	{
		theme = true;
		gui.LightTheme();
	}
	else if (!config.misc[18] && theme)
	{
		theme = false;
		gui.DarkTheme();
	}*/

	if ( config.Misc.SniperCHair )
	{
		if ( g_pLTClientShell != nullptr )
		{
			CPlayer* pLocal = g_pLTClientShell->GetLocalPlayer();

			if ( pLocal && pLocal->IsValidClient() )
			{
				auto CurWeapon = pLocal->CharacterFX->CurrentWeapon;

				if ( CurWeapon && CurWeapon->GetWeaponClass() == Sniper )
				{
					int CenterX = 0, CenterY = 0;
					if ( engine.GetScreenSizes( &CenterX, &CenterY ) )
					{
						auto CenterX2 = CenterX / 2;
						auto CenterY2 = CenterY / 2;

						const int size = 5;
						render.RenderBox( CenterX2 - size, CenterY2, CenterX2 + size, CenterY2, D3DCOLOR_RGBA( 0, 255, 0, 255 ), 1.2f );
						render.RenderBox( CenterX2, CenterY2 - size, CenterX2, CenterY2 + size, D3DCOLOR_RGBA( 0, 255, 0, 255 ), 1.2f );
					}
				}
			}
		}
	}

#ifdef _DEBUG
	if ( g_pLTClientShell->bInGame && g_pLTClientShell->PlayerClient )
	{
		auto Loc = g_pLTClientShell->GetLocalPlayer();
		if ( Loc != nullptr && Loc->IsValidClient() )
		{
			char szDest[ 512 ];
			sprintf_s( szDest, "LocalPlayer: %p\nPlayerClient: %p\nCharacterFX: %p", Loc, g_pLTClientShell->PlayerClient, Loc->CharacterFX );
			render.RenderText( szDest, ImVec2( 15.f, 25.f ), 16.f, D3DCOLOR_XRGB( 255, 255, 255 ) );
		}
	}

	const auto info = engine.GetRoomInfo();
	if ( info )
	{
		char szFormat[ 512 ];
		sprintf_s( szFormat, "RoomPtr: 0x%p - Mode: %d\n", info, ( int )info->GameMode );

		render.RenderText( szFormat, ImVec2( 15.f, 5.f ), 16.0f, D3DCOLOR_RGBA( 255, 255, 255, 255 ), false );
	}
#endif

	//Watermark
	if ( config.Misc.Watermark )
	{
		char szWatermark[ 256 ];
		sprintf_s( szWatermark, 256, XS( " | CrossFire MULTI | %s" ), Tools::DateToString().c_str() );

		render.RenderText( szWatermark, ImVec2( 10, 10 ), 15.f, D3DCOLOR_XRGB( 255, 255, 255 ), false, g_pESPFont );
	}

	/*	for (size_t i = 0; i < rets.size(); ++i)
		{
			char szFormat[4096]{ };
			sprintf_s(szFormat, sizeof szFormat, "Addr: 0x%X (RVA: 0x%X) - Hits: %d", rets[i].Address, rets[i].Address - Globals::cshell_dll, rets[i].Hits);

			render.RenderText(szFormat, ImVec2(10.f, 40.f + (i * 16.f)), 14.5f, D3DCOLOR_RGBA(255, 255, 255, 255), false);
		} */

	aimbot.RenderScene();
	gui.RenderScene();
	esp.RenderScene();
}

void CRenderer::EndScene()
{
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	window->DrawList->PushClipRectFullScreen();

	ImGui::End();
	ImGui::PopStyleColor();
	ImGui::PopStyleVar( 2 );
}

ImVec2 CRenderer::GetFontRect( const std::string& text, float size, ImFont* g_pFont )
{
	return g_pFont->CalcTextSizeA( size, FLT_MAX, 0.0f, text.c_str() );
}

float CRenderer::RenderText( const std::string& text, const ImVec2& position, float size, uint32_t color, bool center, ImFont* g_pFont )
{
	ImGuiWindow* window = ImGui::GetCurrentWindow();

	float a = static_cast< float >( ( color >> 24 ) & 0xff );
	float r = static_cast< float >( ( color >> 16 ) & 0xff );
	float g = static_cast< float >( ( color >> 8 ) & 0xff );
	float b = static_cast< float >( ( color ) & 0xff );

	std::stringstream stream( text );
	std::string line;

	float y = 0.0f;
	int i = 0;

	while ( std::getline( stream, line ) )
	{
		ImVec2 textSize = g_pFont->CalcTextSizeA( size, FLT_MAX, 0.0f, line.c_str() );
		//if (!g_pFont->ContainerAtlas) return 0.f;
		//draw_list->PushTextureID(g_pFont->ContainerAtlas->TexID);

		if ( center )
		{
			window->DrawList->AddText( g_pFont, size, { ( position.x - textSize.x / 2.0f ) + 1.0f, ( position.y + textSize.y * i ) + 1.0f }, ImGui::GetColorU32( { 0.0f, 0.0f, 0.0f, a / 255.0f } ), line.c_str() );
			window->DrawList->AddText( g_pFont, size, { ( position.x - textSize.x / 2.0f ) - 1.0f, ( position.y + textSize.y * i ) - 1.0f }, ImGui::GetColorU32( { 0.0f, 0.0f, 0.0f, a / 255.0f } ), line.c_str() );
			window->DrawList->AddText( g_pFont, size, { ( position.x - textSize.x / 2.0f ) + 1.0f, ( position.y + textSize.y * i ) - 1.0f }, ImGui::GetColorU32( { 0.0f, 0.0f, 0.0f, a / 255.0f } ), line.c_str() );
			window->DrawList->AddText( g_pFont, size, { ( position.x - textSize.x / 2.0f ) - 1.0f, ( position.y + textSize.y * i ) + 1.0f }, ImGui::GetColorU32( { 0.0f, 0.0f, 0.0f, a / 255.0f } ), line.c_str() );

			window->DrawList->AddText( g_pFont, size, { position.x - textSize.x / 2.0f, position.y + textSize.y * i }, ImGui::GetColorU32( { r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f } ), line.c_str() );
		}
		else
		{
			window->DrawList->AddText( g_pFont, size, { ( position.x ) + 1.0f, ( position.y + textSize.y * i ) + 1.0f }, ImGui::GetColorU32( { 0.0f, 0.0f, 0.0f, a / 255.0f } ), line.c_str() );
			window->DrawList->AddText( g_pFont, size, { ( position.x ) - 1.0f, ( position.y + textSize.y * i ) - 1.0f }, ImGui::GetColorU32( { 0.0f, 0.0f, 0.0f, a / 255.0f } ), line.c_str() );
			window->DrawList->AddText( g_pFont, size, { ( position.x ) + 1.0f, ( position.y + textSize.y * i ) - 1.0f }, ImGui::GetColorU32( { 0.0f, 0.0f, 0.0f, a / 255.0f } ), line.c_str() );
			window->DrawList->AddText( g_pFont, size, { ( position.x ) - 1.0f, ( position.y + textSize.y * i ) + 1.0f }, ImGui::GetColorU32( { 0.0f, 0.0f, 0.0f, a / 255.0f } ), line.c_str() );

			window->DrawList->AddText( g_pFont, size, { position.x, position.y + textSize.y * i }, ImGui::GetColorU32( { r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f } ), line.c_str() );
		}

		//draw_list->PopTextureID();
		y = position.y + textSize.y * ( i + 1 );
		i++;
	}

	return y;
}