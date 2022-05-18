#pragma once

#include "ImGUI/imgui.h"
#include "ImGUI/imgui_internal.h"
#include "ImGUI/imgui_impl_dx9.h"

extern ImFont* g_pESPFont;
extern ImFont* g_pDefaultFont;

class CRenderer
{

public:
	CRenderer() = default;
	~CRenderer() = default;

	void Initialize();
	
	void BeginScene();
	void RenderScene();
	void EndScene();

	ImVec2 GetFontRect(const std::string& text, float size, ImFont* g_pFont = g_pDefaultFont);
	float RenderText(const std::string& text, const ImVec2& position, float size, uint32_t color, bool center = false, ImFont* g_pFont = g_pDefaultFont);

	template <class T>
	inline void RenderBoxByType(T x1, T y1, T x2, T y2, DWORD color, float thickness = 1.f, int type = 0) {
		if (type == 0)
			RenderBox(x1, y1, x2, y2, color, thickness);
		else if (type == 1)
			RenderCoalBox(x1, y1, x2, y2, color);
		else if (type == 2)
			RenderBox(x1, y1, x2, y2, color, thickness, 8.f);
	}

	template <class T>
	inline void RenderBoxFilledByType(T x1, T y1, T x2, T y2, DWORD color, float thickness = 1.f, int type = 0) {
		if (type == 0 || type == 1)
			RenderBoxFilled(x1, y1, x2, y2, color, thickness);
		else if (type == 2)
			RenderBoxFilled(x1, y1, x2, y2, color, thickness, 8.f);
	}

	template <class T>
	inline void RenderCoalBox(T x1, T y1, T x2, T y2, DWORD color, float th = 1.f) 
	{
		int w = x2 - x1;
		int h = y2 - y1;

		int iw = w / 4;
		int ih = h / 4;
		// top
		RenderLine(x1, y1, x1 + iw, y1, color, th);					// left
		RenderLine(x1 + w - iw, y1, x1 + w, y1, color, th);			// right
		RenderLine(x1, y1, x1, y1 + ih, color, th);					// top left
		RenderLine(x1 + w - 1, y1, x1 + w - 1, y1 + ih, color, th);	// top right
																	// bottom
		RenderLine(x1, y1 + h, x1 + iw, y1 + h, color, th);			// left
		RenderLine(x1 + w - iw, y1 + h, x1 + w, y1 + h, color, th);	// right
		RenderLine(x1, y1 + h - ih, x1, y1 + h, color, th);			// bottom left
		RenderLine(x1 + w - 1, y1 + h - ih, x1 + w - 1, y1 + h, color, th);	// bottom right
	}

	template <class T>
	inline void RenderBox(T x1, T y1, T x2, T y2, DWORD color, float thickness = 1.f, float rounding = 0.f) 
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();

		float a = static_cast<float>((color >> 24) & 0xff);
		float r = static_cast<float>((color >> 16) & 0xff);
		float g = static_cast<float>((color >> 8) & 0xff);
		float b = static_cast<float>((color) & 0xff);

		window->DrawList->AddRect(ImVec2(static_cast<float>(x1), static_cast<float>(y1)), ImVec2(static_cast<float>(x2), static_cast<float>(y2)), ImGui::GetColorU32({ r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f }), rounding, 15, thickness);
	}

	inline void RenderBox(RECT r, DWORD color, float thickness = 1.f, float rounding = 0.f)
	{
		RenderBox(r.left, r.top, r.right, r.bottom, color, thickness, rounding);
	}

	template <class T>
	inline void RenderBoxFilled(T x1, T y1, T x2, T y2, DWORD color, float rounding = 0.f)
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();

		float a = static_cast<float>((color >> 24) & 0xff);
		float r = static_cast<float>((color >> 16) & 0xff);
		float g = static_cast<float>((color >> 8) & 0xff);
		float b = static_cast<float>((color) & 0xff);

		window->DrawList->AddRectFilled(ImVec2(static_cast<float>(x1), static_cast<float>(y1)), ImVec2(static_cast<float>(x2), static_cast<float>(y2)), ImGui::GetColorU32({ r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f }), rounding, 15);
	}

	template <class T>
	inline void RenderLine(T x1, T y1, T x2, T y2, DWORD color, float thickness = 1.f) 
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();

		float a = static_cast<float>((color >> 24) & 0xff);
		float r = static_cast<float>((color >> 16) & 0xff);
		float g = static_cast<float>((color >> 8) & 0xff);
		float b = static_cast<float>((color) & 0xff);

		window->DrawList->AddLine(ImVec2(static_cast<float>(x1), static_cast<float>(y1)), ImVec2(static_cast<float>(x2), static_cast<float>(y2)), ImGui::GetColorU32({ r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f }), thickness);
	}

	template <class T>
	inline void RenderCircle(T x, T y, float radius, int points, DWORD color, float thickness = 1.f) 
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();

		float a = static_cast<float>((color >> 24) & 0xff);
		float r = static_cast<float>((color >> 16) & 0xff);
		float g = static_cast<float>((color >> 8) & 0xff);
		float b = static_cast<float>((color) & 0xff);

		window->DrawList->AddCircle(ImVec2(static_cast<float>(x), static_cast<float>(y)), radius, ImGui::GetColorU32({ r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f }), points, thickness);
	}

	template <class T>
	inline void RenderCircleFilled(T x, T y, float radius, int points, DWORD color) 
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();

		float a = static_cast<float>((color >> 24) & 0xff);
		float r = static_cast<float>((color >> 16) & 0xff);
		float g = static_cast<float>((color >> 8) & 0xff);
		float b = static_cast<float>((color) & 0xff);

		window->DrawList->AddCircleFilled(ImVec2(x, y), radius, ImGui::GetColorU32({ r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f }), points);
	}
};

extern CRenderer render;