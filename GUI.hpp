#pragma once

#include "ImGUI/imgui.h"
#include "ImGUI/imgui_internal.h"
#include "ImGUI/imgui_impl_dx9.h"
#include "ImGUI/imgui_impl_win32.h"

//const char* const KeyNames[] = {
//	"Unknown",
//	"VK_LBUTTON",
//	"VK_RBUTTON",
//	"VK_CANCEL",
//	"VK_MBUTTON",
//	"VK_XBUTTON1",
//	"VK_XBUTTON2",
//	"Unknown",
//	"VK_BACK",
//	"VK_TAB",
//	"Unknown",
//	"Unknown",
//	"VK_CLEAR",
//	"VK_RETURN",
//	"Unknown",
//	"Unknown",
//	"VK_SHIFT",
//	"VK_CONTROL",
//	"VK_MENU",
//	"VK_PAUSE",
//	"VK_CAPITAL",
//	"VK_KANA",
//	"Unknown",
//	"VK_JUNJA",
//	"VK_FINAL",
//	"VK_KANJI",
//	"Unknown",
//	"VK_ESCAPE",
//	"VK_CONVERT",
//	"VK_NONCONVERT",
//	"VK_ACCEPT",
//	"VK_MODECHANGE",
//	"VK_SPACE",
//	"VK_PRIOR",
//	"VK_NEXT",
//	"VK_END",
//	"VK_HOME",
//	"VK_LEFT",
//	"VK_UP",
//	"VK_RIGHT",
//	"VK_DOWN",
//	"VK_SELECT",
//	"VK_PRINT",
//	"VK_EXECUTE",
//	"VK_SNAPSHOT",
//	"VK_INSERT",
//	"VK_DELETE",
//	"VK_HELP",
//	"0",
//	"1",
//	"2",
//	"3",
//	"4",
//	"5",
//	"6",
//	"7",
//	"8",
//	"9",
//	"Unknown",
//	"Unknown",
//	"Unknown",
//	"Unknown",
//	"Unknown",
//	"Unknown",
//	"Unknown",
//	"A",
//	"B",
//	"C",
//	"D",
//	"E",
//	"F",
//	"G",
//	"H",
//	"I",
//	"J",
//	"K",
//	"L",
//	"M",
//	"N",
//	"O",
//	"P",
//	"Q",
//	"R",
//	"S",
//	"T",
//	"U",
//	"V",
//	"W",
//	"X",
//	"Y",
//	"Z",
//	"VK_LWIN",
//	"VK_RWIN",
//	"VK_APPS",
//	"Unknown",
//	"VK_SLEEP",
//	"VK_NUMPAD0",
//	"VK_NUMPAD1",
//	"VK_NUMPAD2",
//	"VK_NUMPAD3",
//	"VK_NUMPAD4",
//	"VK_NUMPAD5",
//	"VK_NUMPAD6",
//	"VK_NUMPAD7",
//	"VK_NUMPAD8",
//	"VK_NUMPAD9",
//	"VK_MULTIPLY",
//	"VK_ADD",
//	"VK_SEPARATOR",
//	"VK_SUBTRACT",
//	"VK_DECIMAL",
//	"VK_DIVIDE",
//	"VK_F1",
//	"VK_F2",
//	"VK_F3",
//	"VK_F4",
//	"VK_F5",
//	"VK_F6",
//	"VK_F7",
//	"VK_F8",
//	"VK_F9",
//	"VK_F10",
//	"VK_F11",
//	"VK_F12",
//	"VK_F13",
//	"VK_F14",
//	"VK_F15",
//	"VK_F16",
//	"VK_F17",
//	"VK_F18",
//	"VK_F19",
//	"VK_F20",
//	"VK_F21",
//	"VK_F22",
//	"VK_F23",
//	"VK_F24",
//	"Unknown",
//	"Unknown",
//	"Unknown",
//	"Unknown",
//	"Unknown",
//	"Unknown",
//	"Unknown",
//	"Unknown",
//	"VK_NUMLOCK",
//	"VK_SCROLL",
//	"VK_OEM_NEC_EQUAL",
//	"VK_OEM_FJ_MASSHOU",
//	"VK_OEM_FJ_TOUROKU",
//	"VK_OEM_FJ_LOYA",
//	"VK_OEM_FJ_ROYA",
//	"Unknown",
//	"Unknown",
//	"Unknown",
//	"Unknown",
//	"Unknown",
//	"Unknown",
//	"Unknown",
//	"Unknown",
//	"Unknown",
//	"VK_LSHIFT",
//	"VK_RSHIFT",
//	"VK_LCONTROL",
//	"VK_RCONTROL",
//	"VK_LMENU",
//	"VK_RMENU"
//};

struct TO_RENDER
{
	std::string str;
	unsigned int duration;
	DWORD time;
};

class CGUI
{
private:
	bool _visible = true;
	bool _ready = false;
	IDirect3DDevice9* _device = nullptr;
	IDirect3DTexture9* _texture = nullptr;
	std::vector< TO_RENDER > _to_render{ };

public:

	void Initialize(IDirect3DDevice9* device, HWND hwnd);
	void Render();
	void UpdateDevice( IDirect3DDevice9* dev );

	inline void LightTheme()
	{
		using namespace ImGui;

		ImGuiStyle* style = &ImGui::GetStyle();
		ImVec4* colors = style->Colors;
		colors[ImGuiCol_Text] = ColorConvertU32ToFloat4(Spectrum::GRAY800); // text on hovered controls is gray900
		colors[ImGuiCol_TextDisabled] = ColorConvertU32ToFloat4(Spectrum::GRAY500);
		colors[ImGuiCol_WindowBg] = ColorConvertU32ToFloat4(Spectrum::GRAY100);
		colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_PopupBg] = ColorConvertU32ToFloat4(Spectrum::GRAY50); // not sure about this. Note: applies to tooltips too.
		colors[ImGuiCol_Border] = ColorConvertU32ToFloat4(Spectrum::GRAY300);
		colors[ImGuiCol_BorderShadow] = ColorConvertU32ToFloat4(Spectrum::Static::NONE); // We don't want shadows. Ever.
		colors[ImGuiCol_FrameBg] = ColorConvertU32ToFloat4(Spectrum::GRAY75); // this isnt right, spectrum does not do this, but it's a good fallback
		colors[ImGuiCol_FrameBgHovered] = ColorConvertU32ToFloat4(Spectrum::GRAY50);
		colors[ImGuiCol_FrameBgActive] = ColorConvertU32ToFloat4(Spectrum::GRAY200);
		colors[ImGuiCol_TitleBg] = ColorConvertU32ToFloat4(Spectrum::GRAY300); // those titlebar values are totally made up, spectrum does not have this.
		colors[ImGuiCol_TitleBgActive] = ColorConvertU32ToFloat4(Spectrum::GRAY200);
		colors[ImGuiCol_TitleBgCollapsed] = ColorConvertU32ToFloat4(Spectrum::GRAY400);
		colors[ImGuiCol_MenuBarBg] = ColorConvertU32ToFloat4(Spectrum::GRAY100);
		colors[ImGuiCol_ScrollbarBg] = ColorConvertU32ToFloat4(Spectrum::GRAY100); // same as regular background
		colors[ImGuiCol_ScrollbarGrab] = ColorConvertU32ToFloat4(Spectrum::GRAY400);
		colors[ImGuiCol_ScrollbarGrabHovered] = ColorConvertU32ToFloat4(Spectrum::GRAY600);
		colors[ImGuiCol_ScrollbarGrabActive] = ColorConvertU32ToFloat4(Spectrum::GRAY700);
		colors[ImGuiCol_CheckMark] = ColorConvertU32ToFloat4(Spectrum::BLUE500);
		colors[ImGuiCol_SliderGrab] = ColorConvertU32ToFloat4(Spectrum::GRAY700);
		colors[ImGuiCol_SliderGrabActive] = ColorConvertU32ToFloat4(Spectrum::GRAY800);
		colors[ImGuiCol_Button] = ColorConvertU32ToFloat4(Spectrum::GRAY75); // match default button to Spectrum's 'Action Button'.
		colors[ImGuiCol_ButtonHovered] = ColorConvertU32ToFloat4(Spectrum::GRAY50);
		colors[ImGuiCol_ButtonActive] = ColorConvertU32ToFloat4(Spectrum::GRAY200);
		colors[ImGuiCol_Header] = ColorConvertU32ToFloat4(Spectrum::BLUE400);
		colors[ImGuiCol_HeaderHovered] = ColorConvertU32ToFloat4(Spectrum::BLUE500);
		colors[ImGuiCol_HeaderActive] = ColorConvertU32ToFloat4(Spectrum::BLUE600);
		colors[ImGuiCol_Separator] = ColorConvertU32ToFloat4(Spectrum::GRAY400);
		colors[ImGuiCol_SeparatorHovered] = ColorConvertU32ToFloat4(Spectrum::GRAY600);
		colors[ImGuiCol_SeparatorActive] = ColorConvertU32ToFloat4(Spectrum::GRAY700);
		colors[ImGuiCol_ResizeGrip] = ColorConvertU32ToFloat4(Spectrum::GRAY400);
		colors[ImGuiCol_ResizeGripHovered] = ColorConvertU32ToFloat4(Spectrum::GRAY600);
		colors[ImGuiCol_ResizeGripActive] = ColorConvertU32ToFloat4(Spectrum::GRAY700);
		colors[ImGuiCol_PlotLines] = ColorConvertU32ToFloat4(Spectrum::BLUE400);
		colors[ImGuiCol_PlotLinesHovered] = ColorConvertU32ToFloat4(Spectrum::BLUE600);
		colors[ImGuiCol_PlotHistogram] = ColorConvertU32ToFloat4(Spectrum::BLUE400);
		colors[ImGuiCol_PlotHistogramHovered] = ColorConvertU32ToFloat4(Spectrum::BLUE600);
		colors[ImGuiCol_TextSelectedBg] = ColorConvertU32ToFloat4((Spectrum::BLUE400 & 0x00FFFFFF) | 0x33000000);
		colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
		colors[ImGuiCol_NavHighlight] = ColorConvertU32ToFloat4((Spectrum::GRAY900 & 0x00FFFFFF) | 0x0A000000);
		colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
		colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
		colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);
	}

	inline void DarkTheme()
	{

		//ImGuiStyle* style = &ImGui::GetStyle();
		//ImVec4* colors = style->Colors;

		ImVec4* colors = ImGui::GetStyle().Colors;
		colors[ImGuiCol_Text] = ImVec4(0.98f, 0.98f, 0.98f, 1.00f);
		colors[ImGuiCol_WindowBg] = ImVec4(0.01f, 0.01f, 0.01f, 0.87f);
		colors[ImGuiCol_ChildBg] = ImVec4(0.05f, 0.04f, 0.04f, 0.83f);
		colors[ImGuiCol_PopupBg] = ImVec4(0.06f, 0.05f, 0.05f, 0.93f);
		colors[ImGuiCol_Border] = ImVec4(0.26f, 0.26f, 0.27f, 0.50f);
		colors[ImGuiCol_FrameBg] = ImVec4(0.04f, 0.04f, 0.04f, 0.86f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.08f, 0.08f, 0.08f, 0.79f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.01f, 0.02f, 0.02f, 0.91f);
		colors[ImGuiCol_TitleBg] = ImVec4(0.05f, 0.04f, 0.04f, 0.91f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.02f, 0.02f, 0.02f, 0.88f);
		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.01f, 0.01f, 0.01f, 0.87f);
		colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.63f, 0.62f, 0.62f, 0.77f);
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.12f, 0.11f, 0.11f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.09f, 0.09f, 0.09f, 1.00f);
		colors[ImGuiCol_CheckMark] = ImVec4(0.68f, 0.68f, 0.68f, 0.88f);
		colors[ImGuiCol_SliderGrab] = ImVec4(0.27f, 0.27f, 0.28f, 0.93f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(0.08f, 0.09f, 0.09f, 1.00f);
		colors[ImGuiCol_Button] = ImVec4(0.07f, 0.07f, 0.07f, 0.91f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.10f, 0.11f, 0.11f, 0.88f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.02f, 0.02f, 0.02f, 0.89f);
		colors[ImGuiCol_Header] = ImVec4(0.04f, 0.04f, 0.04f, 0.85f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(0.04f, 0.05f, 0.05f, 0.90f);
		colors[ImGuiCol_HeaderActive] = ImVec4(0.03f, 0.03f, 0.03f, 0.92f);
		colors[ImGuiCol_Separator] = ImVec4(0.17f, 0.17f, 0.19f, 0.50f);
		colors[ImGuiCol_SeparatorHovered] = ImVec4(0.13f, 0.14f, 0.15f, 0.93f);
		colors[ImGuiCol_SeparatorActive] = ImVec4(0.10f, 0.10f, 0.11f, 1.00f);
		colors[ImGuiCol_ResizeGrip] = ImVec4(0.07f, 0.07f, 0.08f, 0.25f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.08f, 0.08f, 0.09f, 0.67f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(0.00f, 0.01f, 0.01f, 0.95f);
		colors[ImGuiCol_Tab] = ImVec4(0.04f, 0.05f, 0.05f, 0.93f);
		colors[ImGuiCol_TabHovered] = ImVec4(0.06f, 0.06f, 0.07f, 0.86f);
		colors[ImGuiCol_TabActive] = ImVec4(0.10f, 0.12f, 0.13f, 1.00f);
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.04f, 0.04f, 0.05f, 1.00f);
		colors[ImGuiCol_TextSelectedBg] = ImVec4(0.14f, 0.14f, 0.15f, 0.87f);

		/*colors[ImGuiCol_Text] = ImVec4(0.98f, 0.98f, 0.98f, 1.00f);
		colors[ImGuiCol_Separator] = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
		colors[ImGuiCol_SeparatorActive] = ImVec4(0.90f, 0.90f, 0.93f, 1.00f);
		colors[ImGuiCol_SeparatorHovered] = ImVec4(0.85f, 0.85f, 0.85f, 1.00f);
		colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
		colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.05f, 0.07f, 0.95f);
		colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
		colors[ImGuiCol_Border] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
		colors[ImGuiCol_BorderShadow] = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
		colors[ImGuiCol_FrameBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		colors[ImGuiCol_TitleBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 0.98f, 0.95f, 0.75f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
		colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		colors[ImGuiCol_CheckMark] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
		colors[ImGuiCol_SliderGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		colors[ImGuiCol_Button] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		colors[ImGuiCol_Header] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		colors[ImGuiCol_HeaderActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		colors[ImGuiCol_PlotLines] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
		colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
		colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
		colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
		colors[ImGuiCol_ModalWindowDimBg] = ImVec4(1.00f, 0.98f, 0.95f, 0.73f);*/
	}

	inline void DrawToScene(const char* text, unsigned int duration)
	{
		_to_render.push_back({ text, duration, NULL });
	}

	inline void RenderScene()
	{
		D3DVIEWPORT9 vp{ };

		if (FAILED(_device->GetViewport(&vp)))
			return;

		int i = -1;

		for (auto r = _to_render.begin(); r != _to_render.end(); ++r)
		{
			++i;

			if (r->time == NULL)
				r->time = GetTickCount();

			auto diff = (GetTickCount() - r->time);

			if (diff <= r->duration * 1000)
			{
				render.RenderText(r->str, ImVec2(static_cast<float>((vp.Width / 2.f) - (render.GetFontRect(r->str, 18.f).x / 2.f)), (100.f + (i * (18.f + 2.f)))), 18.f, D3DCOLOR_XRGB(255, 255, 255));
			}
			else
			{
				r = _to_render.erase(r);
				break;
			}
		}
	}

	bool IsReady() { return _ready; };
	bool &MenuVisible() { return _visible; };
};

extern CGUI gui;