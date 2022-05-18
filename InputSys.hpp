#pragma once

#include <functional>

enum class KeyState
{
	None = 1,
	Down,
	Up,
	Pressed /*Down and then up*/
};

class InputSys
{
	InputSys();
	~InputSys();

public:
	void Initialize( IDirect3DDevice9* device );

	HWND GetMainWindow() const { return m_hTargetWindow; }

	KeyState      GetKeyState( uint32_t vk );
	bool          IsKeyDown( uint32_t vk );
	bool          WasKeyPressed( uint32_t vk );

	void RegisterHotkey( uint32_t vk, std::function<void( void )> f );
	void RemoveHotkey( uint32_t vk );

	static InputSys& ins()
	{
		static InputSys i;
		return i;
	}

	static LRESULT CALLBACK hkWndProc( const HWND hwnd, const UINT message, const WPARAM w_param, const LPARAM l_param );

private:
	bool ProcessMessage( UINT uMsg, WPARAM wParam, LPARAM lParam );
	bool ProcessMouseMessage( UINT uMsg, WPARAM wParam, LPARAM lParam );
	bool ProcessKeybdMessage( UINT uMsg, WPARAM wParam, LPARAM lParam );

	HWND            m_hTargetWindow;
	LONG_PTR        m_ulOldWndProc;
	KeyState       m_iKeyMap[ 256 ];

	std::function<void( void )> m_Hotkeys[ 256 ];
};
