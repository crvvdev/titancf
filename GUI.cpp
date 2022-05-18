#define IMGUI_DEFINE_MATH_OPERATORS

#include "Includes.hpp"
#include "OpenSans.hpp"
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

		ImGui::RenderTextClipped( frame_bb.Min + style.FramePadding, frame_bb.Max - style.FramePadding,
			buf_display, NULL, NULL, style.ButtonTextAlign, &clip_rect );

		if ( label_size.x > 0 )
			ImGui::RenderText( ImVec2( total_bb.Min.x, frame_bb.Min.y + style.FramePadding.y ), label );

		return value_changed;
	}

	bool ToggleButton( const char* label, bool* v, int type, const ImVec2& size_arg )
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if ( window->SkipItems )
			return false;

		int flags = 0;
		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID( label );
		const ImVec2 label_size = ImGui::CalcTextSize( label, NULL, true );

		ImVec2 pos = window->DC.CursorPos;
		ImVec2 size = ImGui::CalcItemSize( size_arg, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f );

		const ImRect bb( pos, pos + size );
		ImGui::ItemSize( bb, style.FramePadding.y );
		if ( !ImGui::ItemAdd( bb, id ) )
			return false;

		if ( window->DC.ItemFlags & ImGuiItemFlags_ButtonRepeat ) flags |= ImGuiButtonFlags_Repeat;
		bool hovered, held;
		bool pressed = ImGui::ButtonBehavior( bb, id, &hovered, &held, flags );

		// Render
		const ImU32 col = ImGui::GetColorU32( ( hovered && held || *v ) ? ImGuiCol_ButtonActive : ( hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button ) );
		ImGui::RenderFrame( bb.Min, bb.Max, col, true, style.FrameRounding );

		ImVec2 icon_size = ImVec2( 0, 0 );
		if ( type == 0 )
		{
			ImGui::RenderTextClipped( bb.Min + style.FramePadding, bb.Max - style.FramePadding, label, NULL, &label_size, style.ButtonTextAlign, &bb );
			//     ImGui::PushFont( icon );
			//     ImGui::RenderTextClipped( bb.Min + style.FramePadding, bb.Max - style.FramePadding, "A", NULL, &label_size, icon_size, &bb );
			//     ImGui::PopFont();
		}
		else if ( type == 1 )
		{
			ImGui::RenderTextClipped( bb.Min + style.FramePadding, bb.Max - style.FramePadding, label, NULL, &label_size, style.ButtonTextAlign, &bb );
			//   ImGui::PushFont( icon );
			//   ImGui::RenderTextClipped( bb.Min + style.FramePadding, bb.Max - style.FramePadding, "B", NULL, &label_size, icon_size, &bb );
		   //    ImGui::PopFont();
		}
		else if ( type == 2 )
		{
			ImGui::RenderTextClipped( bb.Min + style.FramePadding, bb.Max - style.FramePadding, label, NULL, &label_size, style.ButtonTextAlign, &bb );
			//    ImGui::PushFont( icon );
			//    ImGui::RenderTextClipped( bb.Min + style.FramePadding, bb.Max - style.FramePadding, "C", NULL, &label_size, icon_size, &bb );
			//    ImGui::PopFont();
		}
		else if ( type == 3 )
		{
			ImGui::RenderTextClipped( bb.Min + style.FramePadding, bb.Max - style.FramePadding, label, NULL, &label_size, style.ButtonTextAlign, &bb );
			//    ImGui::PushFont( icon );
			//    ImGui::RenderTextClipped( bb.Min + style.FramePadding, bb.Max - style.FramePadding, "D", NULL, &label_size, icon_size, &bb );
			 //   ImGui::PopFont();
		}
		else if ( type == 4 )
		{
			ImGui::RenderTextClipped( bb.Min + style.FramePadding, bb.Max - style.FramePadding, label, NULL, &label_size, style.ButtonTextAlign, &bb );
			//    ImGui::PushFont( icon );
			//    ImGui::RenderTextClipped( bb.Min + style.FramePadding, bb.Max - style.FramePadding, "E", NULL, &label_size, icon_size, &bb );
			 //   ImGui::PopFont();
		}
		else if ( type == 5 )
		{
			ImGui::RenderTextClipped( bb.Min + style.FramePadding, bb.Max - style.FramePadding, label, NULL, &label_size, style.ButtonTextAlign, &bb );
			//    ImGui::PushFont( icon );
			//    ImGui::RenderTextClipped( bb.Min + style.FramePadding, bb.Max - style.FramePadding, "F", NULL, &label_size, icon_size, &bb );
			 //   ImGui::PopFont();
		}
		else {
			if ( *v ) {
				ImGui::RenderTextClipped( bb.Min + style.FramePadding, bb.Max - style.FramePadding, label, NULL, &label_size, style.ButtonTextAlign, &bb );
				ImGui::GetWindowDrawList()->AddRectFilled( bb.Min, ImVec2( bb.Max.x, bb.Min.y + 2 ), ImColor( 255, 102, 102 ) );
			}
			else
			{
				ImGui::RenderTextClipped( bb.Min + style.FramePadding, bb.Max - style.FramePadding, label, NULL, &label_size, style.ButtonTextAlign, &bb );
				//     ImGui::GetWindowDrawList()->AddRectFilled( bb.Min, ImVec2( bb.Max.x, bb.Min.y + 2 ), ImColor( 105, 105, 105 ) );
			}

		}
		if ( pressed )
			*v = !*v;

		return pressed;
	}

	template<size_t N>
	void RenderTabInside( const char* ( &names )[ N ], int& activetab, float w, float h )
	{
		bool values[ N ] = { false };
		values[ activetab ] = true;

		if ( N > 1 )
			w /= static_cast< float >( N );

		for ( auto i = 0; i < N; ++i )
		{
			if ( ImGui::ToggleButton( names[ i ], &values[ i ], 6, ImVec2{ w, h } ) )
				activetab = i;

			ImGui::SameLine();
		}
	}

	template<size_t N>
	void RenderTab( const char* ( &names )[ N ], int& activetab, float w, float h )
	{
		ImGui::PushStyleVar( ImGuiStyleVar_FrameRounding, 3.f );
		bool values[ N ] = { false };
		values[ activetab ] = true;

		for ( auto i = 0; i < N; ++i )
		{
			ImGui::NewLine(); ImGui::NewLine();
			ImGui::SameLine( ( 190 / 2 ) - ( w / 2 ) );
			if ( ImGui::ToggleButton( names[ i ], &values[ i ], i, ImVec2{ w, h } ) )
				activetab = i;
		}

		ImGui::PopStyleVar();
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

auto RenderConfigTab = []( float& w ) -> void
{
	static const char* Tab[] =
	{
		"Configuracões"
	};

	static int tab = 0;
	ImGui::RenderTabInside( Tab, tab, w + 14.f, 40.f );
	ImGui::NewLine();
	ImGui::Separator();

	ImGui::BeginChild( XS( "##CHLD_MISC" ), ImVec2( -1, -1 ) );
	{
		static std::string cfg_load_path{ }, cfg_load_name{ };

		ImGui::ListBoxHeader( XS( "##cfgs" ), ImVec2( -1, 310 ) );

		for ( const auto& files : std::filesystem::directory_iterator( config_system.GetConfigFolder() ) )
		{
			if ( files.path().extension().string().find( XS( "json" ) ) == std::string::npos )
				continue;

			const bool item_selected = ( cfg_load_path == files.path().string().c_str() );
			if ( ImGui::Selectable( files.path().filename().string().c_str(), item_selected ) )
			{
				cfg_load_path = files.path().string().c_str();
				cfg_load_name = files.path().filename().string().c_str();
			}
		}

		ImGui::ListBoxFooter();

		static char szCfgCustomPath[ MAX_PATH ]{ };

		ImGui::Text( XS( "Nome da config a ser salva:" ) );
		ImGui::PushItemWidth( -1 );
		ImGui::InputText( XS( "##CFG_CUSTOM_NAME" ), szCfgCustomPath, MAX_PATH );
		ImGui::PopItemWidth();

		if ( ImGui::Button( XS( "Carregar Config Selecionada" ), ImVec2( -1, 25 ) ) )
		{
			if ( !cfg_load_path.empty() )
			{
				config_system.LoadConfig( cfg_load_path );

				gui.DrawToScene( XS( "A Config foi carregada!" ), 5 );
#ifdef _DEBUG
				printf( "Config: %s was loaded!\n", cfg_load_path.c_str() );
#endif
			}
		}

		if ( ImGui::Button( XS( "Deletar Config Selecionada" ), ImVec2( -1, 25 ) ) )
		{
			if ( !cfg_load_path.empty() )
			{
				size_t lastindex = cfg_load_path.find_last_of( '/' );
				if ( lastindex != std::string::npos )
					cfg_load_path = cfg_load_path.substr( 0, lastindex );

				DeleteFileA( cfg_load_path.c_str() );
				gui.DrawToScene( XS( "A Config foi apagada!" ), 5 );
			}
		}

		if ( ImGui::Button( XS( "Salvar Config" ), ImVec2( -1, 25 ) ) )
		{
			if ( strlen( szCfgCustomPath ) <= 0 && !cfg_load_name.empty() )
			{
				size_t lastindex = cfg_load_name.find_last_of( '.' );
				if ( lastindex != std::string::npos )
					cfg_load_name = cfg_load_name.substr( 0, lastindex );

				config_system.SaveConfig( cfg_load_name );

				gui.DrawToScene( XS( "A Config foi salva!" ), 5 );
			}
			else
			{
				config_system.SaveConfig( szCfgCustomPath );
				gui.DrawToScene( XS( "A Config foi salva!" ), 5 );
			}
		}

		std::stringstream ss;
		ss << XS( "As configs estão salvas em: " ) << config_system.GetConfigFolder();

		ImGui::Text( ss.str().c_str() );
		ImGui::EndChild();
	}
};

auto RenderAimbotTab = []( float& w ) -> void
{
	static const char* Tab[] =
	{
		"Aimbot",
		"Triggerbot"
	};

	static const char* vFovMode[] =
	{
		"Por Campo de Visão",
		"Por Campo de Visão e Proximidade"
	};

	static const char* vHitBoxes[] =
	{
		"Quadril",
		"Peito",
		"Pescoco",
		"Cabeca"
	};

	static int tab = 0;
	ImGui::RenderTabInside( Tab, tab, w + 14.f, 40.f );
	ImGui::NewLine();

	ImGui::BeginChild( XS( "##CHLD_AIMBOT" ), ImVec2( -1, -1 ) );
	{
		static const char* Tabs[] =
		{
			"Pistola",
			"Escopetas",
			"SMG",
			"Rifles",
			"Snipers",
			"Pesadas"
		};

		static int act_tab = 0;
		ImGui::RenderTabInside( Tabs, act_tab, w - 15.f, 25.f );
		ImGui::NewLine();
		ImGui::Separator();

		switch ( tab )
		{
		case 0:
			ImGui::Columns( 2, nullptr, false );

			ImGui::Checkbox( XS( "Habilitado" ), &config.Aimbot[ act_tab ].Enabled );
			if ( !config.Aimbot[ act_tab ].ZM )
			{
				ImGui::Checkbox( XS( "Silent-Aim" ), &config.Aimbot[ act_tab ].SilentAim ); ImGui::SameLine(); ImGui::HelpMarker( XS( "Ativa o modo mira fantasma: A mira não ira puxar." ) );
				ImGui::Checkbox( XS( "Mirar no Time" ), &config.Aimbot[ act_tab ].TeamKill );
			}

			ImGui::Checkbox( XS( "Ativar em Modo Zumbi" ), &config.Aimbot[ act_tab ].ZM );
			if ( config.Aimbot[ act_tab ].ZM ) { ImGui::SameLine(); ImGui::HelpMarker( XS( "O Aimbot por padrão funciona no modo 'Silent-Aim' nesse tipo de jogo." ) ); }

			if ( !config.Aimbot[ act_tab ].ZM )
			{
				if ( !config.Aimbot[ act_tab ].SilentAim )
				{
					ImGui::Checkbox( XS( "Check de Visibilidade" ), &config.Aimbot[ act_tab ].VisibleCheck );
					ImGui::Checkbox( XS( "Mirar Automaticamente" ), &config.Aimbot[ act_tab ].AutoAim );
				}
				else
					ImGui::Checkbox( XS( "Ignorar Visibilidade" ), &config.Aimbot[ act_tab ].VisibleCheck );
			}

			ImGui::Checkbox( XS( "Desenhar Campo de Visao" ), &config.Aimbot[ act_tab ].DrawFOV );
			ImGui::Checkbox( XS( "Bone-Scan" ), &config.Aimbot[ act_tab ].BoneScan ); ImGui::SameLine(); ImGui::HelpMarker( XS( "Usa o bone visivel mais proximo da sua mira (Bom para uso legit)." ) );

			if ( config.Aimbot[ act_tab ].ZM )
				ImGui::Checkbox( XS( "Priorizar Zumbi com Pouca Vida" ), &config.Aimbot[ act_tab ].PrioritizeHealth );

			ImGui::NextColumn();

			ImGui::PushItemWidth( -1 );
			ImGui::Text( XS( "Tecla" ) );
			ImGui::Hotkey( XS( "##AIM_KEY" ), &config.Aimbot[ act_tab ].Key, ImVec2( -1, 25 ) );
			ImGui::Text( XS( "Suavidade" ) );
			ImGui::SliderFloat( XS( "##smoothness" ), &config.Aimbot[ act_tab ].Smoothness, 0.0F, 100.0F );
			ImGui::Text( XS( "Campo de Visao" ) );
			ImGui::SliderFloat( XS( "##fov" ), &config.Aimbot[ act_tab ].FieldOfView, 0.0f, 180.0f );

			ImGui::Text( XS( "Modo de Mira" ) );
			ImGui::Combo( XS( "##fovs" ), &config.Aimbot[ act_tab ].FOVType, vFovMode, IM_ARRAYSIZE( vFovMode ) );

			if ( !config.Aimbot[ act_tab ].BoneScan )
			{
				ImGui::Text( XS( "Bone" ) );
				ImGui::Combo( XS( "##hitbox" ), &config.Aimbot[ act_tab ].Hitbox, vHitBoxes, IM_ARRAYSIZE( vHitBoxes ) );
			}
			ImGui::PopItemWidth();

			break;

		case 1:
			ImGui::Columns( 2, nullptr, false );

			ImGui::Text( XS( "Triggerbot" ) ); ImGui::SameLine(); ImGui::HelpMarker( XS( "Se configurado, atira automaticamente caso um jogador esteja na sua mira." ) );
			ImGui::Checkbox( XS( "Habilitado" ), &config.Aimbot[ act_tab ].TriggerEnabled );
			ImGui::Checkbox( XS( "Atirar Automaticamente##1" ), &config.Aimbot[ act_tab ].TriggerAutoShoot );

			ImGui::NextColumn();

			ImGui::PushItemWidth( -1 );
			ImGui::Text( XS( "Tecla" ) );
			ImGui::Hotkey( XS( "##TRIGGER_KEY" ), &config.Aimbot[ act_tab ].TriggerKey, ImVec2( -1, 25 ) );
			ImGui::Text( XS( "Atraso do Triggerbot" ) ); ImGui::SameLine(); ImGui::HelpMarker( XS( "Quanto maior, mais demorado vai ser para ele atirar." ) );
			ImGui::SliderInt( XS( "##TriggerDelay" ), &config.Aimbot[ act_tab ].TriggerDelay, 0, 1000 );

			ImGui::PopItemWidth();

			break;
		}

		ImGui::EndChild();
	}
};

auto RenderMiscTab = []( float& w ) -> void
{
	static const char* Tabs[] =
	{
		"Outros",
		"Teclas"
	};

	static int tab = 0;
	ImGui::RenderTabInside( Tabs, tab, w + 14.f, 40.f );
	ImGui::NewLine();
	ImGui::Separator();

	ImGui::BeginChild( XS( "##CHLD_MISC" ), ImVec2( -1, -1 ) );
	{
		switch ( tab )
		{
		case 0:
			ImGui::Columns( 2, nullptr );

			ImGui::Checkbox( XS( "Remover Fog" ), &config.Misc.RemoveFog ); ImGui::SameLine(); ImGui::HelpMarker( XS( "Remove a neblina de alguns mapas e melhora a visilibidade." ) );
			ImGui::Checkbox( XS( "Diminuir Recoil" ), &config.Misc.LessRecoil );
			ImGui::Checkbox( XS( "Diminuir Spread" ), &config.Misc.LessSpread );
			ImGui::Checkbox( XS( "Recarregar Rapidamente" ), &config.Misc.FastReload );
			ImGui::Checkbox( XS( "Troca de Arma Rapida" ), &config.Misc.FastWeaponChange );
			ImGui::Checkbox( XS( "Alcance Maximo da Arma" ), &config.Misc.MaxWeaponRange );
			ImGui::Checkbox( XS( "Faca x1000" ), &config.Misc.FastKnife );

			ImGui::Checkbox( XS( "Sem Smoke" ), &config.Misc.NoSmoke );
			ImGui::Checkbox( XS( "Sem Flash" ), &config.Misc.NoFlash ); ImGui::SameLine(); ImGui::HelpMarker( XS( "Remove o Efeito que cega o seu boneco." ) );
			ImGui::Checkbox( XS( "Mira para Sniper" ), &config.Misc.SniperCHair ); ImGui::SameLine(); ImGui::HelpMarker( XS( "Ativa uma pequena mira 2D que ajuda a dar no scope." ) );
			ImGui::Checkbox( XS( "Bunnyhop" ), &config.Misc.Bunnyhop ); ImGui::SameLine(); ImGui::HelpMarker( XS( "Segure espaço e seu boneco pula sozinho." ) );
			ImGui::Checkbox( XS( "Spy Mode" ), &config.Misc.SpyMode ); ImGui::SameLine(); ImGui::HelpMarker( XS( "Ative somente dentro da sala, se abusar muito você toma erro de cliente." ) );
			ImGui::Checkbox( XS( "Atirar Rapido" ), &config.Misc.FastFire ); ImGui::SameLine(); ImGui::HelpMarker( XS( "Funciona apenas em Sniper e Pistola." ) );

			ImGui::NextColumn();

			ImGui::Text( XS( "FOV da Arma" ) );
			ImGui::PushItemWidth( -1 );
			ImGui::SliderFloat( XS( "##fov" ), &config.Misc.WeaponFOV, 1, 120 );
			ImGui::PopItemWidth();

			ImGui::Checkbox( XS( "Sem Dano de Queda" ), &config.Misc.NoFallDamage );
			ImGui::Checkbox( XS( "Sem Dano de Granada" ), &config.Misc.NoNadeDamage );
			ImGui::Checkbox( XS( "Defuse Instantâneo" ), &config.Misc.InstaDefuse );
			ImGui::Checkbox( XS( "Caminhar Rapidamente" ), &config.Misc.FastWalk );
			ImGui::Checkbox( XS( "Marca d'Agua" ), &config.Misc.Watermark );
			ImGui::Checkbox( XS( "Spammer de Radio" ), &config.Misc.RadioSpam );
			ImGui::Checkbox( XS( "AFK Bot" ), &config.Misc.AFKBot ); ImGui::SameLine(); ImGui::HelpMarker( XS( "Deixe ativo e você não ira levar kick por ausência." ) );
			break;
		case 1:

			ImGui::Text( XS( "Tecla do Spy Mode" ) );
			ImGui::Hotkey( XS( "##Key1" ), &config.Misc.SpyModeKey, ImVec2( -1, 25 ) );

			ImGui::Text( XS( "Tecla do Room Lag" ) );
			ImGui::Hotkey( XS( "##Key2" ), &config.Misc.LagRoomKey, ImVec2( -1, 25 ) );

			ImGui::Text( XS( "Tecla do Crashar Sala" ) );
			ImGui::Hotkey( XS( "##Key3" ), &config.Misc.CrashRoomKey, ImVec2( -1, 25 ) );

			break;
		}
		ImGui::EndChild();
	}
};

auto RenderVisualTab = []( float& w ) -> void
{
	static const char* Tabs[] =
	{
		"Visuais",
		"Cores"
	};

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

	static int act_tab = 0;
	ImGui::RenderTabInside( Tabs, act_tab, w + 14.f, 40.f );

	ImGui::NewLine();
	ImGui::Separator();
	ImGui::BeginChild( XS( "##CHLD_VISUALS" ), ImVec2( -1, -1 ) );
	{
		switch ( act_tab )
		{
		case 0:
			ImGui::Columns( 2, nullptr, false );
			ImGui::Checkbox( XS( "ESP Nomes" ), &config.ESP.Names );
			ImGui::Checkbox( XS( "ESP Esqueleto" ), &config.ESP.Skeleton );

			ImGui::Text( XS( "ESP Caixas" ) );
			ImGui::Combo( XS( "##BoxMode" ), &config.ESP.Boxes, ESPBoxOpts, IM_ARRAYSIZE( ESPBoxOpts ) );

			ImGui::Text( XS( "ESP Vida" ) );
			ImGui::Combo( XS( "##HealthMode" ), &config.ESP.Health, ESPHealthOpts, IM_ARRAYSIZE( ESPHealthOpts ) );

			ImGui::Checkbox( XS( "ESP Armas" ), &config.ESP.Weapons );
			ImGui::Checkbox( XS( "Mostrar C4" ), &config.ESP.ShowC4 );
			ImGui::Checkbox( XS( "Wallhack" ), &config.ESP.Chams.Wallhack );
			ImGui::Checkbox( XS( "Chams" ), &config.ESP.Chams.Chams );

			ImGui::NextColumn();

			ImGui::Checkbox( XS( "Apenas Visível" ), &config.ESP.OnlyVisible );
			ImGui::Checkbox( XS( "Ativar em Modo Zumbi" ), &config.ESP.UseOnZM );
			ImGui::Checkbox( XS( "Glow" ), &config.ESP.Glow ); ImGui::SameLine(); ImGui::HelpMarker( XS( "Ativa um brilho em volta dos jogadores." ) );
			ImGui::Checkbox( XS( "Brilho total" ), &config.ESP.Chams.Fullbright );
			ImGui::Checkbox( XS( "Chams Ghost" ), &config.ESP.Chams.GhostChams );

			if ( config.ESP.Chams.Chams )
				ImGui::Checkbox( XS( "Apenas Bonecos Visíveis" ), &config.ESP.Chams.OnlyVisible );

			ImGui::NextColumn();

			break;

		case 1:

			ImGui::PushItemWidth( -1 );

			ImGui::Text( XS( "Cor do Inimigo" ) );
			ImGui::ColorEdit3( XS( "##COL_ENEMY_ESP" ), config.ESP.EnemyCol, ImGuiColorEditFlags_Float | ImGuiColorEditFlags_DisplayRGB );
			ImGui::Spacing();
			ImGui::Text( XS( "Cor do Inimigo Quando Visivel" ) );
			ImGui::ColorEdit3( XS( "##COL_ENEMY_ESP_VIS" ), config.ESP.EnemyVisCol, ImGuiColorEditFlags_Float | ImGuiColorEditFlags_DisplayRGB );
			ImGui::Spacing();
			ImGui::Text( XS( "Cor do Glow" ) );
			ImGui::ColorEdit3( XS( "##COL_ENEMY_GLOW" ), config.ESP.GlowEnemyCol, ImGuiColorEditFlags_Float | ImGuiColorEditFlags_DisplayRGB );
			ImGui::Spacing();
			ImGui::Text( XS( "Cor do Glow Quando Visivel" ) );
			ImGui::ColorEdit3( XS( "##COL_ENEMY_GLOW_VIS" ), config.ESP.GlowEnemyVisCol, ImGuiColorEditFlags_Float | ImGuiColorEditFlags_DisplayRGB );
			ImGui::Spacing();
			ImGui::Text( XS( "Cor do Chams" ) );
			ImGui::ColorEdit3( XS( "##COL_ENEMY_CHAMS" ), config.ESP.Chams.EnemyCol, ImGuiColorEditFlags_Float | ImGuiColorEditFlags_DisplayRGB );
			ImGui::Spacing();
			ImGui::Text( XS( "Cor do Chams Quando Visivel" ) );
			ImGui::ColorEdit3( XS( "##COL_ENEMY_CHAMS_VIS" ), config.ESP.Chams.EnemyVisCol, ImGuiColorEditFlags_Float | ImGuiColorEditFlags_DisplayRGB );
			ImGui::Spacing();

			ImGui::PopItemWidth();
		}

		ImGui::EndChild();
	}
};

void CGUI::UpdateDevice( IDirect3DDevice9* dev )
{
	_device = dev;

	if ( _texture )
	{
		_texture->Release();
		_texture = nullptr;
	}

	if ( !_texture )
		D3DXCreateTextureFromFileInMemory( _device, ucForumLogo.data(), ucForumLogo.size(), &_texture );
}

void CGUI::Initialize( IDirect3DDevice9* device, HWND hwnd )
{
	ImGui::CreateContext();

	ImGui_ImplWin32_Init( hwnd );
	ImGui_ImplDX9_Init( device );

	if( !_texture )
		D3DXCreateTextureFromFileInMemory( device, ucForumLogo.data(), ucForumLogo.size(), &_texture );

	ImGui::Spectrum::StyleColorsSpectrum();

	ImGuiIO& io = ImGui::GetIO();
	io.IniFilename = nullptr;

	auto Font = io.Fonts->AddFontFromMemoryCompressedTTF( OpenSans_data.data(), OpenSans_data.size(), 18.5f );
	OpenSans_data.clear();

	if ( Font )
		io.FontDefault = Font;

	ImGuiStyle& _style = ImGui::GetStyle();
	_style.WindowRounding = 1.0f;
	_style.WindowBorderSize = 0.0f;
	_style.WindowPadding = ImVec2( 2, 2 );
	_style.ChildBorderSize = 0.0f;
	_style.ChildRounding = 1.0f;
	_style.WindowBorderSize = 0.f;
	_style.FrameRounding = 1.0f;
	_style.WindowTitleAlign = ImVec2( 0.5f, 0.5f );

	_style.Colors[ ImGuiCol_Text ] = ImColor( 255, 255, 255 );
	_style.Colors[ ImGuiCol_ChildBg ] = ImColor( 15, 15, 15, 245 );
	_style.Colors[ ImGuiCol_WindowBg ] = ImColor( 1, 1, 1, 255 );
	_style.Colors[ ImGuiCol_Button ] = ImColor( 79, 1, 1 );
	_style.Colors[ ImGuiCol_ButtonActive ] = ImColor( 35, 35, 35 );
	_style.Colors[ ImGuiCol_ButtonHovered ] = ImColor( 56, 56, 56 );
	_style.Colors[ ImGuiCol_FrameBg ] = ImColor( 20, 20, 20 );
	_style.Colors[ ImGuiCol_FrameBgActive ] = ImColor( 94, 57, 57 );
	_style.Colors[ ImGuiCol_FrameBgHovered ] = ImColor( 43, 1, 1 );
	_style.Colors[ ImGuiCol_SliderGrab ] = ImColor( 63, 1, 1 );
	_style.Colors[ ImGuiCol_SliderGrabActive ] = ImColor( 43, 1, 1 );
	_style.Colors[ ImGuiCol_Header ] = ImColor( 66, 66, 66 );
	_style.Colors[ ImGuiCol_HeaderActive ] = ImColor( 66, 66, 66 );
	_style.Colors[ ImGuiCol_HeaderHovered ] = ImColor( 92, 92, 92 );
	_style.Colors[ ImGuiCol_CheckMark ] = ImColor( 204, 204, 204 );

	_style.Colors[ ImGuiCol_Separator ] = ImColor( 66, 66, 66 );
	_style.Colors[ ImGuiCol_SeparatorHovered ] = ImColor( 92, 92, 92 );
	_style.Colors[ ImGuiCol_SeparatorActive ] = ImColor( 66, 66, 66 );

	ImGui::GetStyle() = _style;

	_device = device;
	_ready = true;
}

#pragma warning( push )
#pragma warning( disable : 4456)
void CGUI::Render()
{
	/*auto clip = []( float n, float lower, float upper )
	{
		n = ( n > lower ) * n + !( n > lower ) * lower;
		return ( n < upper ) * n + !( n < upper ) * upper;
	};*/

	ImGui::GetIO().MouseDrawCursor = _visible;
	//static constexpr auto frequency = 1 / 0.55f;
	//static float flAlpha = 0.0f;

	if ( !_visible )
	{
	//	flAlpha = 0.0f;
		return;
	}

	//flAlpha = clip( flAlpha + frequency * ImGui::GetIO().DeltaTime, 0.f, 1.f );
	//ImGui::GetStyle().Alpha = flAlpha;
	//engine.UseCursor( _visible );

	ImGui::SetNextWindowSize( ImVec2( 850, 540 ) );
	ImGui::Begin( XS( "##MAINMENU_WND" ), &_visible, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse );
	ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 0, 0 ) );
	ImVec2 p = ImGui::GetCursorScreenPos();
	ImGui::GetWindowDrawList()->AddRectFilled( ImVec2( p.x, p.y ), ImVec2( p.x + 850, p.y + 5 ), ImGui::GetColorU32( ImVec4( 0.478f, 0.f, 0.f, 1.f ) ) );
	ImGui::PopStyleVar();
	ImGui::Columns( 2, NULL, false );

	static float initial_spacing = 220.f;
	if ( initial_spacing )
		ImGui::SetColumnWidth( 0, initial_spacing ), initial_spacing = 0;

	ImGui::SetCursorPosY( ImGui::GetCursorPosY() + 15 );
	ImGui::NewLine(); ImGui::NewLine();
	ImGui::SameLine( 15 );
	ImGui::Image( _texture, ImVec2( 188, 188 ) );

	ImGui::SetCursorPosX( ImGui::GetCursorPosX() + 35 );
	ImGui::Text( XS( " [lol] " ) );
	ImGui::NewLine(); ImGui::NewLine(); ImGui::NewLine();

	static const char* Tabs[] = { "ESP", "Aimbot", "Outros", "Configuração" };

	ImGui::SetCursorPosY( ImGui::GetCursorPosY() - 55 );
	ImGui::SetCursorPosX( ImGui::GetCursorPosX() + 35 );

	static int active = 0;
	ImGui::RenderTab( Tabs, active, 190, 35 );

	ImGui::NextColumn();

	ImGui::PushItemWidth( 623 );
	ImGui::BeginChild( XS( "##SECOND_WND" ), ImVec2( 623, 650 ), true );
	{
		ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 0, 0 ) );
		ImGui::PopStyleVar();
		ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();
		ImGui::GetWindowDrawList()->AddRectFilled( ImVec2( p.x, p.y ), ImVec2( p.x + 850, p.y + 5 ), ImGui::GetColorU32( ImVec4( 0.478f, 0.f, 0.f, 1.f ) ) );

		auto w = ImGui::GetColumnWidth() - 15.f;
		switch ( active )
		{
		default:
		case 0:
			RenderVisualTab( w );
			break;
		case 1:
			RenderAimbotTab( w );
			break;
		case 2:
			RenderMiscTab( w );
			break;
		case 3:
			RenderConfigTab( w );
			break;
		}
		ImGui::EndChild();
	}
	ImGui::End();
}
#pragma warning( pop ) 