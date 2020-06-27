#include "connection.h"

#include <stdio.h>
#include <ws2tcpip.h>

namespace GamesAcademy
{
	bool Connection::create()
	{
		WORD wVersionRequested = MAKEWORD( 2, 2 );
		WSADATA wsaData;
		int result = WSAStartup( wVersionRequested, &wsaData );
		if( result != ERROR_SUCCESS )
		{
			printf( "WinSock2 could not be initialized. error: %08x\n", result );
			return false;
		}

		m_socket = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );
		if( m_socket == INVALID_SOCKET )
		{
			printf( "Could not create socket. error: %08x\n", WSAGetLastError() );
			return false;
		}

		unsigned long nonblocking = 1;
		result = ioctlsocket( m_socket, FIONBIO, &nonblocking );

		if( result == SOCKET_ERROR )
		{
			printf( "cannot put socket into non-blocking mode. error: %08x\n", WSAGetLastError() );
			return false;
		}

		m_serverAddress.sin_family	= AF_INET;
		m_serverAddress.sin_port	= htons( 1546 );
		InetPtonA( AF_INET, "62.171.169.138", &m_serverAddress.sin_addr );

		return true;
	}

	void Connection::destroy()
	{
		if( m_socket != INVALID_SOCKET )
		{
			closesocket( m_socket );
			m_socket = INVALID_SOCKET;
		}
	}

	void Connection::update()
	{
		char buffer[ 2048 ];
		sockaddr_in address;
		while( true )
		{
			int addressSize = sizeof( address );
			const int packetSize = recvfrom( m_socket, buffer, sizeof( buffer ), 0, (sockaddr*)&address, &addressSize );
			if( packetSize <= 0 )
			{
				break;
			}

			if( packetSize < sizeof( MessageHeader ) )
			{
				continue;
			}

			const MessageHeader* pHeader = (const MessageHeader*)buffer;

			if( sizeof( *pHeader ) + pHeader->messageSize > packetSize )
			{
				continue;
			}

			const void* pMessage = &pHeader[ 1u ];
			switch( pHeader->messageType )
			{
			case MessageType::LoginResponse:
				handleLoginResponse( pMessage, pHeader->messageSize );
				break;

			case MessageType::GameState:
				handleGameState( pMessage, pHeader->messageSize );
				break;

			default:
				break;
			}
		}
	}

	void Connection::login( const char* pUsername )
	{
		char buffer[ 2048u ];
		MessageLoginRequest* pLoginRequest = (MessageLoginRequest*)buffer;
		pLoginRequest->usernameLength = uint16( strlen( pUsername ) );
		memcpy( pLoginRequest->username, pUsername, pLoginRequest->usernameLength );

		sendMessage( MessageType::LoginRequest, pLoginRequest, sizeof( *pLoginRequest ) + pLoginRequest->usernameLength );
	}

	void Connection::action( MessagePlayerActionType action )
	{
		MessagePlayerAction playerAction;
		playerAction.action = action;

		sendMessage( MessageType::PlayerAction, &playerAction, sizeof( playerAction ) );
	}

	void Connection::handleLoginResponse( const void* pMessage, size_t messageSize )
	{
		if( messageSize < sizeof( MessageLoginResponse ) )
		{
			return;
		}

		const MessageLoginResponse* pLoginResponse = (const MessageLoginResponse*)pMessage;
		if( !pLoginResponse->ok )
		{
			m_state = ConnectionState::Failed;
			return;
		}

		m_state			= ConnectionState::LoggedIn;
		m_playerId		= pLoginResponse->playerId;
	}

	void Connection::handleGameState( const void* pMessage, size_t messageSize )
	{
		if( messageSize < sizeof( MessageGameState ) )
		{
			return;
		}

		m_gameState = *(const MessageGameState*)pMessage;

		if( m_state == ConnectionState::LoggedIn )
		{
			for( size_t i = 0; i < m_gameState.playerCount; ++i )
			{
				if( m_gameState.players[ i ].playerId == m_playerId )
				{
					m_state = ConnectionState::Playing;
				}
			}
		}
	}

	void Connection::sendMessage( MessageType type, const void* pMessage, size_t dataSize )
	{
		MessageHeader header;
		header.messageSize	= uint16( dataSize );
		header.messageType	= type;

		char buffer[ 2048u ];
		memcpy( buffer, &header, sizeof( header ) );
		memcpy( buffer + sizeof( header ), pMessage, dataSize );

		sendto( m_socket, buffer, int( sizeof( header ) + dataSize ), 0, (const sockaddr*)&m_serverAddress, sizeof( m_serverAddress ) );
	}
}