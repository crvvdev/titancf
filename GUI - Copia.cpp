#define IMGUI_DEFINE_MATH_OPERATORS

#include "Includes.hpp"
#include <filesystem>

CGUI gui;
CONFIGS config;

namespace ImGui
{
	static auto vector_getter = []( void* vec, int idx, const char** out_text )
	{
		auto& vector = *static_cast< std::vector<std::string>* >( vec );
		if ( idx < 0 || idx >= static_cast< int >( vector.size() ) ) { return false; }
		*out_text = vector.at( idx ).c_str();
		return true;
	};

	IMGUI_API bool ComboBoxArray( const char* label, int* currIndex, std::vector<std::string>& values )
	{
		if ( values.empty() ) { return false; }
		return Combo( label, currIndex, vector_getter,
			static_cast< void* >( &values ), values.size() );
	}

	IMGUI_API void MultiCombo( const char* name, std::vector<std::string>& items, bool* selected )
	{
		static std::string _preview = "";

		if ( ImGui::BeginCombo( name, _preview.c_str() ) )
		{
			_preview = "";

			std::vector < std::string > vec;

			for ( size_t i = 0; i < items.size(); i++ )
			{
				ImGui::Selectable( items.at( i ).c_str(), &selected[ i ], ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups );

				if ( selected[ i ] )
					vec.emplace_back( items[ i ] );
			}

			for ( size_t i = 0; i < vec.size(); i++ )
			{
				if ( vec.size() == 1 )
					_preview += vec.at( i );
				else if ( !( i == vec.size() - 1 ) )
					_preview += vec.at( i ) + ", ";
				else
					_preview += vec.at( i );
			}

			ImGui::EndCombo();
		}
	}

	bool Hotkey( const char* label, int* k, const ImVec2& size_arg )
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if ( window->SkipItems )
			return false;

		ImGuiContext& g = *GImGui;
		ImGuiIO& io = g.IO;
		const ImGuiStyle& style = g.Style;

		const ImGuiID id = window->GetID( label );
		const ImVec2 label_size = ImGui::CalcTextSize( label, NULL, true );
		ImVec2 size = ImGui::CalcItemSize( size_arg, ImGui::CalcItemWidth(), label_size.y + style.FramePadding.y * 2.0f );
		const ImRect frame_bb( window->DC.CursorPos + ImVec2( label_size.x + style.ItemInnerSpacing.x, 0.0f ), window->DC.CursorPos + size );
		const ImRect total_bb( window->DC.CursorPos, frame_bb.Max );

		ImGui::ItemSize( total_bb, style.FramePadding.y );
		if ( !ImGui::ItemAdd( total_bb, id ) )
			return false;

		const bool focus_requested = ImGui::FocusableItemRegister( window, g.ActiveId == id );
		//const bool focus_requested_by_code = focus_requested && (window->FocusIdxAllCounter == window->FocusIdxAllRequestCurrent);
		//const bool focus_requested_by_code = focus_requested && (g.FocusRequestCurrCounterAll == g.FocusRequestCurrCounterTab);
		//const bool focus_requested_by_tab = focus_requested && !focus_requested_by_code;

		const bool hovered = ImGui::ItemHoverable( frame_bb, id );

		if ( hovered ) {
			ImGui::SetHoveredID( id );
			g.MouseCursor = ImGuiMouseCursor_TextInput;
		}

		const bool user_clicked = hovered && io.MouseClicked[ 0 ];

		if ( focus_requested || user_clicked ) {
			if ( g.ActiveId != id ) {
				// Start edition
				memset( io.MouseDown, 0, sizeof( io.MouseDown ) );
				memset( io.KeysDown, 0, sizeof( io.KeysDown ) );
				*k = 0;
			}
			ImGui::SetActiveID( id, window );
			ImGui::FocusWindow( window );
		}
		else if ( io.MouseClicked[ 0 ] ) {
			// Release focus when we click outside
			if ( g.ActiveId == id )
				ImGui::ClearActiveID();
		}

		bool value_changed = false;
		int key = *k;

		if ( g.ActiveId == id ) {
			for ( auto i = 0; i < 5; i++ ) {
				if ( io.MouseDown[ i ] ) {
					switch ( i ) {
					case 0:
						key = VK_LBUTTON;
						break;
					case 1:
						key = VK_RBUTTON;
						break;
					case 2:
						key = VK_MBUTTON;
						break;
					case 3:
						key = VK_XBUTTON1;
						break;
					case 4:
						key = VK_XBUTTON2;
						break;
					}
					value_changed = true;
					ImGui::ClearActiveID();
				}
			}
			if ( !value_changed ) {
				for ( auto i = VK_BACK; i <= VK_RMENU; i++ ) {
					if ( io.KeysDown[ i ] ) {
						key = i;
						value_changed = true;
						ImGui::ClearActiveID();
					}
				}
			}

			if ( IsKeyPressedMap( ImGuiKey_Escape ) ) {
				*k = 0;
				ImGui::ClearActiveID();
			}
			else {
				*k = key;
			}
		}

		// Render
		// Select which buffer we are going to display. When ImGuiInputTextFlags_NoLiveEdit is Set 'buf' might still be the old value. We Set buf to NULL to prevent accidental usage from now on.

		char buf_display[ 64 ];//= "None";
		strcpy_s( buf_display, XS( "Nenhuma Tecla" ) );

		ImGui::RenderFrame( frame_bb.Min, frame_bb.Max, ImGui::GetColorU32( ImVec4( 0.07f, 0.07f, 0.07f, 0.91f ) ), true, style.FrameRounding );

		if ( *k != 0 && g.ActiveId != id )
		{
			if ( *k <= VK_RBUTTON )
			{
				sprintf_s( buf_display, 64, XS( "Mouse %d" ), *k );
			}
			else if ( *k > VK_CANCEL && *k <= VK_XBUTTON2 )
			{
				sprintf_s( buf_display, 64, XS( "Mouse %d" ), *k - 1 );
			}
			else
			{
				UINT scanCode = MapVirtualKeyA( *k, 0 ) << 16;

				if ( !GetKeyNameTextA( scanCode, buf_display, 128 ) )
					sprintf_s( buf_display, 64, XS( "Virtual Key %d" ), *k );
			}
		}
		else if ( g.ActiveId == id ) {
			strcpy_s( buf_display, XS( "<Pressione uma Tecla>" ) );
		}

		const ImRect clip_rect( frame_bb.Min.x, frame_bb.Min.y, frame_bb.Min.x + size.x, frame_bb.Min.y + size.y ); // Not using frame_bb.Max because we have adjusted size
		ImVec2 render_pos = frame_bb.Min + style.FramePadding;

		ImGui::RenderTextClipped( frame_bb.Min + style.FramePadding, frame_bb.Max - style.FramePadding,
			buf_display, NULL, NULL, style.ButtonTextAlign, &clip_rect );

		if ( label_size.x > 0 )
			ImGui::RenderText( ImVec2( total_bb.Min.x, frame_bb.Min.y + style.FramePadding.y ), label );

		return value_changed;
	}

	static void HelpMarker( const char* desc )
	{
		ImGui::TextDisabled( XS( "(?)" ) );
		if ( ImGui::IsItemHovered() )
		{
			ImGui::BeginTooltip();
			ImGui::PushTextWrapPos( ImGui::GetFontSize() * 35.0f );
			ImGui::TextUnformatted( desc );
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}
	}
};

void CGUI::Initialize( IDirect3DDevice9* device, HWND hwnd )
{
	ImGui::CreateContext();

	ImGui_ImplWin32_Init( hwnd );
	ImGui_ImplDX9_Init( device );

	ImGui::Spectrum::StyleColorsSpectrum();
	DarkTheme();

	ImGuiIO& io = ImGui::GetIO();
	io.IniFilename = nullptr;

	auto& style = ImGui::GetStyle();
	style.WindowTitleAlign = ImVec2( 0.5f, 0.5f );

	_device = device;
	_ready = true;
}

float clip( float n, float lower, float upper )
{
	n = ( n > lower ) * n + !( n > lower ) * lower;
	return ( n < upper ) * n + !( n < upper ) * upper;
}

#pragma warning( push )
#pragma warning( disable : 4456)
void CGUI::Render()
{
	auto clip = []( float n, float lower, float upper )
	{
		n = ( n > lower ) * n + !( n > lower ) * lower;
		return ( n < upper ) * n + !( n < upper ) * upper;
	};

	ImGui::GetIO().MouseDrawCursor = _visible;
	static constexpr auto frequency = 1 / 0.55f;
	static float flAlpha = 0.0f;

	if ( !_visible )
	{
		flAlpha = 0.0f;
		return;
	}

	flAlpha = clip( flAlpha + frequency * ImGui::GetIO().DeltaTime, 0.f, 1.f );
	ImGui::GetStyle().Alpha = flAlpha;

	engine.UseCursor( _visible );

	ImGui::SetNextWindowSize( ImVec2{ 935, 475 }, ImGuiCond_Always );

	ImGui::Begin( XS( "www.forum.loxproduction.xyz | CrossFire BR" ), &_visible, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize );
	{
		static int tabb = 0;
		//defining and making the tabs

		auto cur_w = ImGui::GetWindowWidth();

		{
			ImGui::SameLine();
			if ( ImGui::Button( XS( "Visuais" ), ImVec2( cur_w / 4 - 15, 25 ) ) )
			{
				tabb = 0;
			}
			ImGui::SameLine();

			if ( ImGui::Button( XS( "Crossbot" ), ImVec2( cur_w / 4 - 15, 25 ) ) )
			{
				tabb = 1;
			}
			ImGui::SameLine();
			if ( ImGui::Button( XS( "Outros" ), ImVec2( cur_w / 4 - 15, 25 ) ) )
			{
				tabb = 2;
			}
			ImGui::SameLine();
			if ( ImGui::Button( XS( "Configs" ), ImVec2( cur_w / 4 - 15, 25 ) ) )
			{
				tabb = 3;
			}
		}

		ImGui::Separator();
		ImGui::NewLine();

		if ( tabb == 0 )
		{
			static int internal_tab = 0;
			auto cur_w = ImGui::GetWindowWidth();

			ImGui::SameLine();
			if ( ImGui::Button( XS( "Visuals##1" ), ImVec2( cur_w / 2 - 15, 25 ) ) )
			{
				internal_tab = 0;
			}
			ImGui::SameLine();

			if ( ImGui::Button( XS( "Wallhack" ), ImVec2( cur_w / 2 - 15, 25 ) ) )
			{
				internal_tab = 1;
			}

			ImGui::Spacing();

			if ( internal_tab == 0 )
			{
				auto win_pos = ImGui::GetWindowPos();

				ImGui::SetNextWindowPos( ImVec2( win_pos.x + 15, win_pos.y + 92 ), ImGuiCond_Always );
				ImGui::BeginChild( XS( "##CHILD_VISUALS" ), ImVec2( 0, 0 ), false, ImGuiWindowFlags_NoTitleBar );
				{
					ImGui::Columns( 2, nullptr );

					static const char* ESPHealthOpts[] =
					{
						"Desativado",
						"Texto",
						"Barras"
					};

					static const char* ESPBoxOpts[] =
					{
						"Desativado",
						"2D",
						"3D",
						"Edge"
					};

					ImGui::Checkbox( XS( "ESP Nomes" ), &config.ESP.Names );
					ImGui::Checkbox( XS( "ESP Esqueleto" ), &config.ESP.Skeleton );

					ImGui::Text( XS( "ESP Caixas" ) );
					ImGui::Combo( XS( "##BoxMode" ), &config.ESP.Boxes, ESPBoxOpts, IM_ARRAYSIZE( ESPBoxOpts ) );

					ImGui::Text( XS( "ESP Vida" ) );
					ImGui::Combo( XS( "##HealthMode" ), &config.ESP.Health, ESPHealthOpts, IM_ARRAYSIZE( ESPHealthOpts ) );

					ImGui::Checkbox( XS( "ESP Armas" ), &config.ESP.Weapons );
					ImGui::Checkbox( XS( "Mostrar C4" ), &config.ESP.ShowC4 );
					ImGui::Checkbox( XS( "Apenas Visivel" ), &config.ESP.OnlyVisible );
					ImGui::Checkbox( XS( "Ativar em Modo Zumbi" ), &config.ESP.UseOnZM );
					//	ImGui::Checkbox( XS( "Radar" ), &config.ESP.Radar );
					ImGui::Checkbox( XS( "Glow" ), &config.ESP.Glow ); ImGui::SameLine(); ImGui::HelpMarker( XS( u8"Ativa um brilho em volta dos jogadores." ) );

					ImGui::NextColumn();

					ImGui::PushItemWidth( cur_w / 2 - 15 );
					ImGui::Text( XS( "Cor do Inimigo" ) );
					ImGui::ColorEdit3( XS( "##enemy color" ), config.ESP.EnemyCol );
					ImGui::Spacing();
					ImGui::Text( XS( "Cor do Inimigo Quando Visivel" ) );
					ImGui::ColorEdit3( XS( "##enemy vis color" ), config.ESP.EnemyVisCol );
					ImGui::Spacing();
					ImGui::Text( XS( "Cor do Glow" ) );
					ImGui::ColorEdit3( XS( "##glow enemy color" ), config.ESP.GlowEnemyCol );
					ImGui::Spacing();
					ImGui::Text( XS( "Cor do Glow Quando Visivel" ) );
					ImGui::ColorEdit3( XS( "##glow enemy vis color" ), config.ESP.GlowEnemyVisCol );
					ImGui::Spacing();

					ImGui::Text( XS( "Intensidade do Glow" ) );
					ImGui::SliderFloat( XS( "##GLOWTHICK" ), &config.ESP.GlowThickness, 0.0f, 3.0f );

					/*if (ImGui::Button(XS("Radar 2D"), ImVec2(-1, -1)))
						radar.EnableWindow() = !radar.EnableWindow();*/

					ImGui::PopItemWidth();

					ImGui::EndChild();
				}
			}

			if ( internal_tab == 1 )
			{
				auto win_pos = ImGui::GetWindowPos();

				ImGui::SetNextWindowPos( ImVec2( win_pos.x + 15, win_pos.y + 92 ), ImGuiCond_Always );

				ImGui::BeginChild( XS( "##CHILD_CHAMS" ), ImVec2( 0, 0 ), false, ImGuiWindowFlags_NoTitleBar );
				{
					ImGui::Columns( 2, nullptr );

					ImGui::Checkbox( XS( "Wallhack" ), &config.ESP.Chams.Wallhack );
					ImGui::Checkbox( XS( "Chams" ), &config.ESP.Chams.Chams );
					ImGui::Checkbox( XS( "Apenas Bonecos Visiveis" ), &config.ESP.Chams.OnlyVisible );

					ImGui::NextColumn();

					ImGui::PushItemWidth( cur_w / 2 - 15 );
					ImGui::Text( XS( "Cor do Chams" ) );
					ImGui::ColorEdit3( XS( "##enemy_color" ), config.ESP.Chams.EnemyCol );
					ImGui::Spacing();
					ImGui::Text( XS( "Cor do Chams Quando Visivel" ) );
					ImGui::ColorEdit3( XS( "##enemy_vis_color" ), config.ESP.Chams.EnemyVisCol );
					ImGui::Spacing();

					ImGui::Checkbox( XS( "Brilho total" ), &config.ESP.Chams.Fullbright );
					ImGui::Checkbox( XS( "Chams Ghost" ), &config.ESP.Chams.GhostChams );

					ImGui::PopItemWidth();

					ImGui::EndChild();
				}
			}
		}

		if ( tabb == 1 )
		{
			static int internal_tab = 0;
			auto cur_w = ImGui::GetWindowWidth();

			ImGui::SameLine();
			if ( ImGui::Button( XS( "Pistola" ), ImVec2( cur_w / 6 - 15, 25 ) ) )
				internal_tab = 0;

			ImGui::SameLine();
			if ( ImGui::Button( XS( "Shotgun" ), ImVec2( cur_w / 6 - 15, 25 ) ) )
				internal_tab = 1;

			ImGui::SameLine();
			if ( ImGui::Button( XS( "SMG" ), ImVec2( cur_w / 6 - 15, 25 ) ) )
				internal_tab = 2;

			ImGui::SameLine();
			if ( ImGui::Button( XS( "Rifle" ), ImVec2( cur_w / 6 - 15, 25 ) ) )
				internal_tab = 3;

			ImGui::SameLine();
			if ( ImGui::Button( XS( "Sniper" ), ImVec2( cur_w / 6 - 15, 25 ) ) )
				internal_tab = 4;

			ImGui::SameLine();
			if ( ImGui::Button( XS( "MG" ), ImVec2( cur_w / 6 - 15, 25 ) ) )
				internal_tab = 5;

			auto win_pos = ImGui::GetWindowPos();
			ImGui::SetNextWindowPos( ImVec2( win_pos.x + 15, win_pos.y + 92 ), ImGuiCond_Always );

			ImGui::BeginChild( XS( "##CHILD_AIMBOT" ), ImVec2( 0, 0 ), false, ImGuiWindowFlags_NoTitleBar );
			{
				static const char* vAimbotGroups[] =
				{
					"Pistolas",
					"Shotguns" ,
					"SMG" ,
					"Rifles" ,
					"Snipers" ,
					"Machine Guns"
				};

				auto v2 = ImGui::CalcTextSize( vAimbotGroups[ internal_tab ] );

				ImGui::Separator();
				ImGui::SameLine( ( ImGui::GetWindowWidth() / 2 ) - ( v2.x / 2 ) );
				ImGui::Text( XS( "%s" ), vAimbotGroups[ internal_tab ] );

				ImGui::Columns( 2, nullptr );

				ImGui::Checkbox( XS( "Aimbot Habilitado" ), &config.Aimbot[ internal_tab ].Enabled );
				if ( !config.Aimbot[ internal_tab ].ZM ) ImGui::Checkbox( XS( "Silent-Aim" ), &config.Aimbot[ internal_tab ].SilentAim ); ImGui::SameLine(); ImGui::HelpMarker( XS( "Ativa o modo mira fantasma: A mira não ira puxar." ) );
				if ( !config.Aimbot[ internal_tab ].ZM ) ImGui::Checkbox( XS( "Mirar no Time" ), &config.Aimbot[ internal_tab ].TeamKill );
				ImGui::Checkbox( XS( "Ativar em Modo Zumbi" ), &config.Aimbot[ internal_tab ].ZM );

				if ( config.Aimbot[ internal_tab ].ZM ) { ImGui::SameLine(); ImGui::HelpMarker( XS( u8"Somente o modo 'Silent-Aim' funciona no Modo Zumbi." ) ); }

				if ( !config.Aimbot[ internal_tab ].ZM )
				{
					if ( !config.Aimbot[ internal_tab ].SilentAim )
					{
						ImGui::Checkbox( XS( "Check de Visibilidade" ), &config.Aimbot[ internal_tab ].VisibleCheck );
						ImGui::Checkbox( XS( "Mirar Automaticamente" ), &config.Aimbot[ internal_tab ].AutoAim );
					}
					else
						ImGui::Checkbox( XS( "Ignorar Visibilidade" ), &config.Aimbot[ internal_tab ].VisibleCheck );
				}

				ImGui::Checkbox( XS( "Desenhar Campo de Visao" ), &config.Aimbot[ internal_tab ].DrawFOV );
				ImGui::Checkbox( XS( "Bone-Scan" ), &config.Aimbot[ internal_tab ].BoneScan ); ImGui::SameLine(); ImGui::HelpMarker( XS( "Usa o bone visível mais proximo da sua mira (Bom para uso legit)." ) );

				if ( config.Aimbot[ internal_tab ].ZM )
					ImGui::Checkbox( XS( "Priorizar Zumbi com Pouca Vida" ), &config.Aimbot[ internal_tab ].PrioritizeHealth );
				//else
				//	ImGui::Checkbox( XS( "Usar mouse_event" ), &config.Aimbot[ internal_tab ].MouseEvent );

				ImGui::NextColumn();
				ImGui::PushItemWidth( cur_w / 2 - 15 );
				ImGui::Text( XS( "Tecla do Aimbot" ) );
				ImGui::Hotkey( XS( "Key##1" ), &config.Aimbot[ internal_tab ].Key, ImVec2( cur_w / 2 - 15, 25 ) );
				ImGui::Text( XS( "Suavidade" ) );
				ImGui::SliderFloat( XS( "##smoothness" ), &config.Aimbot[ internal_tab ].Smoothness, 0.0F, 100.0F );
				ImGui::Text( XS( "Campo de Visao" ) );
				ImGui::SliderFloat( XS( "##fov" ), &config.Aimbot[ internal_tab ].FieldOfView, 0.0f, 180.0f );

				ImGui::Text( XS( "Modo de Mira" ) );

				static const char* vFovMode[] =
				{
					"Por Campo de Visao",
					"Por Campo de Visao e Proximidade"
				};

				ImGui::Combo( XS( "##fovs" ), &config.Aimbot[ internal_tab ].FOVType, vFovMode, IM_ARRAYSIZE( vFovMode ) );

				static const char* vHitBoxes[] =
				{
					"Quadril",
					"Peito",
					"Pescoco",
					"Cabeca"
				};

				if ( !config.Aimbot[ internal_tab ].BoneScan )
				{
					ImGui::Text( XS( "Bone" ) );
					ImGui::Combo( XS( "##hitbox" ), &config.Aimbot[ internal_tab ].Hitbox, vHitBoxes, IM_ARRAYSIZE( vHitBoxes ) );
				}

				ImGui::Text( XS( "Triggerbot" ) ); ImGui::SameLine(); ImGui::HelpMarker( XS( u8"Se configurado, atira automaticamente caso um jogador esteja na sua mira." ) );
				ImGui::Checkbox( XS( "Trigger Habilitado" ), &config.Aimbot[ internal_tab ].TriggerEnabled );
				ImGui::Hotkey( XS( "Tecla##5" ), &config.Aimbot[ internal_tab ].TriggerKey, ImVec2( cur_w / 2 - 15, 25 ) );
				ImGui::Text( XS( "Atraso do Triggerbot" ) ); ImGui::SameLine(); ImGui::HelpMarker( XS( u8"Quanto maior, mais demorado vai ser para ele atirar." ) );
				ImGui::SliderInt( XS( "##TriggerDelay" ), &config.Aimbot[ internal_tab ].TriggerDelay, 0, 1000 );
				ImGui::Checkbox( XS( "Atirar Automaticamente##1" ), &config.Aimbot[ internal_tab ].TriggerAutoShoot );

				ImGui::PopItemWidth();

				ImGui::EndChild();
			}
		}

		if ( tabb == 2 )
		{
			auto win_pos = ImGui::GetWindowPos();
			ImGui::SetNextWindowPos( ImVec2( win_pos.x + 15, win_pos.y + 68 ), ImGuiCond_Always );

			ImGui::BeginChild( XS( "##CHILD_MISC" ), ImVec2( 0, 0 ), false, ImGuiWindowFlags_NoTitleBar );
			{
				ImGui::Columns( 2, nullptr );

				ImGui::Checkbox( XS( "Remover Fog" ), &config.Misc.RemoveFog ); ImGui::SameLine(); ImGui::HelpMarker( XS( "Remove a neblina de alguns mapas e melhora a visilibidade." ) );
				ImGui::Checkbox( XS( "Diminuir Recoil" ), &config.Misc.LessRecoil );
				ImGui::Checkbox( XS( "Diminuir Spread" ), &config.Misc.LessSpread );
				ImGui::Checkbox( XS( "Recarregar Rapidamente" ), &config.Misc.FastReload );
				ImGui::Checkbox( XS( "Troca de Arma Rapida" ), &config.Misc.FastWeaponChange );
				ImGui::Checkbox( XS( "Alcance Maximo da Arma" ), &config.Misc.MaxWeaponRange );
				ImGui::Checkbox( XS( "Faca x1000" ), &config.Misc.FastKnife );
				/*if ( config.Misc.FastKnife )
				{
					ImGui::Text( XS( "Velocidade:" ) );
					ImGui::SliderFloat( XS( "##FastKnifeSpeed" ), &config.Misc.FastKnifeSpeed, 1.f, 3.f );
				}*/

				//	ImGui::Checkbox( XS( "Sem Knockback" ), &config.misc[ 5 ] ); ImGui::SameLine(); ImGui::HelpMarker( XS( u8"Remove o efeito de ser jogado para tras quando se é um mutante." ) );
				ImGui::Checkbox( XS( "Sem Smoke" ), &config.Misc.NoSmoke );
				ImGui::Checkbox( XS( "Sem Flash" ), &config.Misc.NoFlash ); ImGui::SameLine(); ImGui::HelpMarker( XS( "Remove o Efeito que cega o seu boneco." ) );
				ImGui::Checkbox( XS( "Mira para Sniper" ), &config.Misc.SniperCHair ); ImGui::SameLine(); ImGui::HelpMarker( XS( "Ativa uma pequena mira 2D que ajuda a dar no scope." ) );
				ImGui::Checkbox( XS( "Bunnyhop" ), &config.Misc.Bunnyhop ); ImGui::SameLine(); ImGui::HelpMarker( XS( "Segure espaço e seu boneco pula sozinho." ) );
				ImGui::Checkbox( XS( "Spy Mode" ), &config.Misc.SpyMode ); ImGui::SameLine(); ImGui::HelpMarker( XS( "Ative somente dentro da sala, se abusar muito você toma erro de cliente." ) );
				ImGui::Checkbox( XS( "Atirar Rapido" ), &config.Misc.FastFire ); ImGui::SameLine(); ImGui::HelpMarker( XS( "Funciona apenas em Sniper e Pistola." ) );

				ImGui::NextColumn();

				ImGui::PushItemWidth( cur_w / 2 - 15 );

				ImGui::Text( XS( "FOV da Arma" ) );
				ImGui::SliderFloat( XS( "##fov" ), &config.Misc.WeaponFOV, 1, 120 );

				ImGui::Checkbox( XS( "Sem Dano de Queda" ), &config.Misc.NoFallDamage );
				ImGui::Checkbox( XS( "Sem Dano de Granada" ), &config.Misc.NoNadeDamage );
				ImGui::Checkbox( XS( "Defuse Instantaneo" ), &config.Misc.InstaDefuse );

				//		ImGui::Checkbox( XS( "Sem Dano de Bug" ), &config.Misc.NoBugDamage );

					/*	ImGui::Checkbox(XS("Aumetar Velocidade Agaichado"), &config.misc[18]);// ImGui::SameLine(); ImGui::HelpMarker(XS(u8"Seu boneco ira flutuar de acordo com a tecla configurada abaixo."));
						if (config.misc[18])
						{
							ImGui::Text(XS("Velocidade:"));
							ImGui::SliderFloat(XS("##CrouchSpeedSlider"), &config.misc_misc[2], 1.f, 2.f);
						} */

				ImGui::Checkbox( XS( "Caminhar Rapidamente" ), &config.Misc.FastWalk );
				ImGui::Checkbox( XS( "Marca d'Agua" ), &config.Misc.Watermark );
				ImGui::Checkbox( XS( "Spammer de Radio" ), &config.Misc.RadioSpam );
				ImGui::Checkbox( XS( "AFK Bot" ), &config.Misc.AFKBot ); ImGui::SameLine(); ImGui::HelpMarker( XS( "Deixe ativo e você não ira levar kick por ausência." ) );

				ImGui::Text( XS( "Tecla do Spy Mode" ) );
				ImGui::Hotkey( XS( "##Key1" ), &config.Misc.SpyModeKey, ImVec2( cur_w / 2 - 15, 25 ) );

				ImGui::Text( XS( "Tecla do Room Lag" ) );
				ImGui::Hotkey( XS( "##Key2" ), &config.Misc.LagRoomKey, ImVec2( cur_w / 2 - 15, 25 ) );

				ImGui::Text( XS( "Tecla do Crashar Sala" ) );
				ImGui::Hotkey( XS( "##Key3" ), &config.Misc.CrashRoomKey, ImVec2( cur_w / 2 - 15, 25 ) );

				ImGui::PopItemWidth();

				ImGui::EndChild();
			}
		}

		if ( tabb == 3 )
		{
			auto win_pos = ImGui::GetWindowPos();
			ImGui::SetNextWindowPos( ImVec2( win_pos.x + 15, win_pos.y + 68 ), ImGuiCond_Always );

			ImGui::BeginChild( XS( "##CHILD_CFG" ), ImVec2( 0, 0 ), false, ImGuiWindowFlags_NoTitleBar );
			{
				ImGui::Columns( 2, nullptr );

				auto cur_w = ImGui::GetWindowWidth();

				static std::string cfg_load_path, cfg_load_name;

				static int cur_item = 0;

				ImGui::ListBoxHeader( XS( "##cfgs" ), ImVec2( cur_w / 2 - 15, -1 ) );
				for ( const auto& files : std::filesystem::directory_iterator( config_system.GetConfigFolder() ) )
				{
					if ( files.path().extension().string().find( XS( "json" ) ) == std::string::npos )
						continue;

					const bool item_selected = !cfg_load_path.compare( files.path().string() );
					if ( ImGui::Selectable( files.path().filename().string().c_str(), item_selected ) )
					{
						cfg_load_path = files.path().string().c_str();
						cfg_load_name = files.path().filename().string().c_str();
					}
				}

				ImGui::ListBoxFooter();

				ImGui::NextColumn();

				static char cfg_a[ MAX_PATH ];
				ImGui::Text( XS( "Salvar com um nome personalizado:" ) );

				ImGui::PushItemWidth( cur_w / 2 - 15 );
				ImGui::InputText( XS( "##customcfgname" ), cfg_a, MAX_PATH );
				ImGui::PopItemWidth();

				if ( ImGui::Button( XS( "Carregar Config Selecionada" ), ImVec2( cur_w / 2 - 15, 25 ) ) )
				{
					if ( !cfg_load_path.empty() )
					{
						config_system.LoadConfig( cfg_load_path );

						this->DrawToScene( XS( "A Config foi carregada!" ), 5 );
#ifdef _DEBUG
						printf( "Config: %s was loaded!\n", cfg_load_path.c_str() );
#endif
					}
				}

				if ( ImGui::Button( XS( "Deletar Config Selecionada" ), ImVec2( cur_w / 2 - 15, 25 ) ) )
				{
					if ( !cfg_load_path.empty() )
					{
						size_t lastindex = cfg_load_path.find_last_of( '/' );
						if ( lastindex != std::string::npos )
							cfg_load_path = cfg_load_path.substr( 0, lastindex );

						DeleteFileA( cfg_load_path.c_str() );
						this->DrawToScene( XS( "A Config foi apagada!" ), 5 );
					}
				}

				if ( ImGui::Button( XS( "Salvar Config" ), ImVec2( cur_w / 2 - 15, 25 ) ) )
				{
					if ( strlen( cfg_a ) <= 0 && !cfg_load_name.empty() )
					{
						size_t lastindex = cfg_load_name.find_last_of( '.' );
						if ( lastindex != std::string::npos )
							cfg_load_name = cfg_load_name.substr( 0, lastindex );

						config_system.SaveConfig( cfg_load_name );

						this->DrawToScene( XS( "A Config foi salva!" ), 5 );
					}
					else
					{

						config_system.SaveConfig( cfg_a );

						this->DrawToScene( XS( "A Config foi salva!" ), 5 );
					}
				}

				ImGui::EndChild();

				char szFText[ 400 ];
				sprintf_s( szFText, sizeof szFText, XS( "Local das Configs: %s" ), config_system.GetConfigFolder().c_str() );
				ImGui::Text( szFText );
			}
		}

		ImGui::End();
	}
}
#pragma warning( pop ) 