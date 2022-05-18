#include "Includes.hpp"

void CAutoMessage::Init()
{
	Term();

	LTRESULT nResult = g_pLTCommon->CreateMessage( m_pMsg );
	if ( nResult == LT_OK )
		m_pMsg->IncRef();
	else
		m_pMsg = LTNULL;
}

float& CPlayerClient::GetPitch()
{
	if ( engine.IsGDMRoom() || engine.IsZMRoom() )
		return *( float* )( ( DWORD )this + 0xDBC );

	return *( float* )( ( DWORD )this + 0x568 );
}

float& CPlayerClient::GetYaw()
{
	if ( engine.IsGDMRoom() || engine.IsZMRoom() )
		return *( float* )( ( DWORD )this + 0xDC0 );

	return *( float* )( ( DWORD )this + 0x56C );
}

void CPlayerClient::SetLocalAngles( const D3DXVECTOR3& Angles )
{
	GetPitch() = DEG2RAD( Angles[ PITCH ] );
	GetYaw() = DEG2RAD( Angles[ YAW ] );
}