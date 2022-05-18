#include "Includes.hpp"

IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );

InputSys::InputSys() : m_hTargetWindow( nullptr ), m_ulOldWndProc( 0 )
{
}

InputSys::~InputSys()
{
	if ( m_ulOldWndProc )
		SetWindowLongW( m_hTargetWindow, GWLP_WNDPROC, m_ulOldWndProc );

	m_ulOldWndProc = 0;
}

void InputSys::Initialize( IDirect3DDevice9* device )
{
	D3DDEVICE_CREATION_PARAMETERS params{};

	if ( FAILED( device->GetCreationParameters( &params ) ) )
		return;

	m_hTargetWindow = params.hFocusWindow;
	m_ulOldWndProc = SetWindowLongW( m_hTargetWindow, GWLP_WNDPROC, ( LONG )hkWndProc );
}

LRESULT CALLBACK InputSys::hkWndProc( const HWND hwnd, const UINT message, const WPARAM w_param, const LPARAM l_param )
{
	if ( ins().ProcessMessage( message, w_param, l_param ) && gui.MenuVisible() )
	{
		ImGui_ImplWin32_WndProcHandler( hwnd, message, w_param, l_param );
		return 0;
	}

	return CallWindowProcW( ( WNDPROC )ins().m_ulOldWndProc, hwnd, message, w_param, l_param );
};

bool InputSys::ProcessMessage( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch ( uMsg ) {
	case WM_MBUTTONDBLCLK:
	case WM_RBUTTONDBLCLK:
	case WM_LBUTTONDBLCLK:
	case WM_XBUTTONDBLCLK:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_LBUTTONDOWN:
	case WM_XBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
	case WM_LBUTTONUP:
	case WM_XBUTTONUP:
		return ProcessMouseMessage( uMsg, wParam, lParam );
	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYDOWN:
	case WM_SYSKEYUP:
		return ProcessKeybdMessage( uMsg, wParam, lParam );
	default:
		return false;
	}
}

bool InputSys::ProcessMouseMessage( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	auto key = VK_LBUTTON;
	auto state = KeyState::None;
	switch ( uMsg ) {
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MBUTTONDBLCLK:
		state = uMsg == WM_MBUTTONUP ? KeyState::Up : KeyState::Down;
		key = VK_MBUTTON;
		break;
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_RBUTTONDBLCLK:
		state = uMsg == WM_RBUTTONUP ? KeyState::Up : KeyState::Down;
		key = VK_RBUTTON;
		break;
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_LBUTTONDBLCLK:
		state = uMsg == WM_LBUTTONUP ? KeyState::Up : KeyState::Down;
		key = VK_LBUTTON;
		break;
	case WM_XBUTTONDOWN:
	case WM_XBUTTONUP:
	case WM_XBUTTONDBLCLK:
		state = uMsg == WM_XBUTTONUP ? KeyState::Up : KeyState::Down;
		key = ( HIWORD( wParam ) == XBUTTON1 ? VK_XBUTTON1 : VK_XBUTTON2 );
		break;
	default:
		return false;
	}

	if ( state == KeyState::Up && m_iKeyMap[ key ] == KeyState::Down )
		m_iKeyMap[ key ] = KeyState::Pressed;
	else
		m_iKeyMap[ key ] = state;
	return true;
}

bool InputSys::ProcessKeybdMessage( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	auto key = wParam;
	auto state = KeyState::None;

	switch ( uMsg ) {
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		state = KeyState::Down;
		break;
	case WM_KEYUP:
	case WM_SYSKEYUP:
		state = KeyState::Up;
		break;
	default:
		return false;
	}

	if ( state == KeyState::Up && m_iKeyMap[ int( key ) ] == KeyState::Down ) {
		m_iKeyMap[ int( key ) ] = KeyState::Pressed;

		auto& hotkey_callback = m_Hotkeys[ key ];

		if ( hotkey_callback )
			hotkey_callback();

	}
	else {
		m_iKeyMap[ int( key ) ] = state;
	}

	return true;
}

KeyState InputSys::GetKeyState( std::uint32_t vk )
{
	return m_iKeyMap[ vk ];
}

bool InputSys::IsKeyDown( std::uint32_t vk )
{
	return m_iKeyMap[ vk ] == KeyState::Down;
}

bool InputSys::WasKeyPressed( std::uint32_t vk )
{
	if ( m_iKeyMap[ vk ] == KeyState::Pressed ) {
		m_iKeyMap[ vk ] = KeyState::Up;
		return true;
	}
	return false;
}

void InputSys::RegisterHotkey( std::uint32_t vk, std::function<void( void )> f )
{
	m_Hotkeys[ vk ] = f;
}

void InputSys::RemoveHotkey( std::uint32_t vk )
{
	m_Hotkeys[ vk ] = nullptr;
}