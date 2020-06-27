#include "server.h"

#include "server_types.h"

#include <algorithm>
#include <arpa/inet.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

namespace GamesAcademy
{
	template<class T>
	const T& clamp( const T& x, const T& lower, const T& upper )
	{
		return std::min( upper, std::max( x, lower ) );
	}

	double getTime()
	{
		timespec spec;
		clock_gettime( CLOCK_REALTIME, &spec );
		return double( spec.tv_sec ) + (double( spec.tv_nsec ) / 1000000000.0);
	}

	bool Server::create()
	{
		m_serverSocket = socket( AF_INET, SOCK_DGRAM, 0 );
		if( m_serverSocket == -1 )
		{
			printf( "Failed to create socket.\n" );
			return false;
		}

		int socketFlags = fcntl( m_serverSocket, F_GETFL );
		if( socketFlags < 0 )
		{
			printf( "Failed to get socket flags.\n" );
			return false;
		}

		socketFlags |= O_NONBLOCK;

		if( fcntl( m_serverSocket, F_SETFL, socketFlags ) < 0 )
		{
			printf( "Failed to set socket flags.\n" );
			return false;
		}

		sockaddr_in serverAddress;
		memset( &serverAddress, 0, sizeof( serverAddress ) );

		serverAddress.sin_family		= AF_INET;
		serverAddress.sin_addr.s_addr	= INADDR_ANY;
		serverAddress.sin_port			= htons( 1546 );

		if( bind( m_serverSocket, (const sockaddr*)&serverAddress, sizeof( serverAddress ) ) < 0 )
		{
			printf( "Bind failed.\n" );
			return false;
		}

		printf( "Server started.\n" );

		return true;
	}

	void Server::destroy()
	{
		if( m_serverSocket != 0 )
		{
			close( m_serverSocket );
			m_serverSocket = 0;
		}
	}

	void Server::update()
	{
		const double serverTime = getTime();

		receiveMessages( serverTime );

		std::vector< uint32 > clientsToRemove;
		for( const std::pair< uint32, ClientState >& kvp : m_clients )
		{
			if( serverTime - kvp.second.lastMesssageTime > 15.0 )
			{
				clientsToRemove.push_back( kvp.first );
			}
		}

		for( uint32 clientToRemove : clientsToRemove )
		{
			ClientMap::iterator clientIt = m_clients.find( clientToRemove );
			printf( "Player %d on Client %08x timed out, so remove!\n", clientIt->second.playerId, clientToRemove );
			m_clients.erase( clientIt );
		}

		for( std::vector< ShootState >::iterator it = m_shoots.begin(); it != m_shoots.end(); ++it )
		{
			if( serverTime - it->time < 5.0 )
			{
				continue;
			}

			it = m_shoots.erase( it );
			if( it == m_shoots.end() )
			{
				break;
			}
		}

		sendGameState();
	}

	void Server::receiveMessages( double serverTime )
	{
		char buffer[ 2048u ];
		sockaddr_in clientAddress;
		while( true )
		{
			socklen_t clientAddressLength = sizeof( clientAddress );
			const int packetSize = recvfrom( m_serverSocket, buffer, sizeof( buffer ), 0, (struct sockaddr*)&clientAddress, &clientAddressLength );
			if( packetSize <= 0 )
			{
				break;
			}

			printf( "Received packet from %08x with %d bytes.\n", clientAddress.sin_addr.s_addr, packetSize );

			if( packetSize < sizeof( MessageHeader ) )
			{
				printf( "Received packet from %08x is too small(%d bytes)\n", clientAddress.sin_addr.s_addr, packetSize );
				continue;
			}

			const uint32 clientIp = clientAddress.sin_addr.s_addr;
			ClientState& client = m_clients[ clientIp ];
			client.address			= clientAddress;
			client.lastMesssageTime	= serverTime;

			const MessageHeader* pHeader = (const MessageHeader*)buffer;
			if( sizeof( MessageHeader ) + pHeader->messageSize > packetSize )
			{
				printf( "Received packet from %08x with invalid messageSize(%d > %d)\n", clientAddress.sin_addr.s_addr, sizeof( MessageHeader ) + pHeader->messageSize, packetSize );
				continue;
			}

			const void* pMessageData = &pHeader[ 1u ];
			switch( pHeader->messageType )
			{
			case MessageType::LoginRequest:
				handleLoginRequestMessage( client, pMessageData, pHeader->messageSize );
				break;

			case MessageType::PlayerAction:
				handlePlayerActionMessage( client, pMessageData, pHeader->messageSize, serverTime );

			default:
				printf( "Received packet from %08x with invalid messageType: %d\n", clientAddress.sin_addr.s_addr, (int)pHeader->messageType );
				break;
			}

			if( client.status != ClientStatus::LoggedIn )
			{
				printf( "Client from %08x is not logged in, so remove!\n", clientAddress.sin_addr.s_addr );
				m_clients.erase( m_clients.find( clientAddress.sin_addr.s_addr ) );
			}
		}
	}

	void Server::handleLoginRequestMessage( ClientState& client, const void* pMessage, uint16 messageSize )
	{
		if( messageSize < sizeof( MessageLoginRequest ) )
		{
			printf( "Login request too small(%d < %d)\n", messageSize, sizeof( MessageLoginRequest ) );
			return;
		}

		const MessageLoginRequest* pLoginRequest = (const MessageLoginRequest*)pMessage;
		std::string username = std::string( pLoginRequest->username, pLoginRequest->usernameLength );

		client.status	= ClientStatus::LoggedIn;
		client.username	= username;
		client.playerId	= m_nextPlayerId++;

		killPlayer( client, nullptr );

		printf( "User logged in. Name: %s\n", username.c_str() );

		MessageLoginResponse loginResponse;
		loginResponse.ok		= true;
		loginResponse.playerId	= client.playerId;

		sendMessage( client, MessageType::LoginResponse, &loginResponse, sizeof( loginResponse ) );
	}

	void Server::handlePlayerActionMessage( ClientState& client, const void* pMessage, uint16 messageSize, double serverTime )
	{
		if( messageSize < sizeof( MessagePlayerAction ) )
		{
			printf( "Player action too small(%d < %d)\n", messageSize, sizeof( MessagePlayerAction ) );
			return;
		}

		const MessagePlayerAction* pPlayerAction = (const MessagePlayerAction*)pMessage;

		if( client.status != ClientStatus::LoggedIn )
		{
			printf( "Client %d is not logged in. Action will be ignored.\n", client.address.sin_addr.s_addr );
			return;
		}

		int moveX = 0;
		int moveY = 0;
		int shootX = 0;
		int shootY = 0;
		switch( pPlayerAction->action )
		{
		case MessagePlayerActionType::MoveUp:
			moveY = -1;
			break;

		case MessagePlayerActionType::MoveDown:
			moveY = 1;
			break;

		case MessagePlayerActionType::MoveLeft:
			moveX = -1;
			break;

		case MessagePlayerActionType::MoveRight:
			moveX = 1;
			break;

		case MessagePlayerActionType::ShootUpLeft:
			shootX = -1;
			shootY = -1;
			break;

		case MessagePlayerActionType::ShootUpRight:
			shootX = 1;
			shootY = -1;
			break;

		case MessagePlayerActionType::ShootDownLeft:
			shootX = -1;
			shootY = 1;
			break;

		case MessagePlayerActionType::ShootDownRight:
			shootX = 1;
			shootY = 1;
			break;

		default:
			printf( "Invalid Player action: %d\n", (int)pPlayerAction->action );
			break;
		}

		if( moveX != 0 || moveY != 0 )
		{
			const uint8 oldPositionX = client.positionX;
			const uint8 oldPositionY = client.positionY;
			client.positionX = clamp( client.positionX + moveX, 0, 16 );
			client.positionY = clamp( client.positionY + moveY, 0, 16 );

			printf( "Move Player %d from (%d, %d) to (%d, %d)\n", client.playerId, oldPositionX, oldPositionY, client.positionX, client.positionY );
		}
		else if( shootX != 0 || shootY != 0 )
		{
			ShootState shoot;
			shoot.time				= serverTime;
			shoot.playerId			= client.playerId;
			shoot.startPositionX	= client.positionX;
			shoot.startPositionY	= client.positionY;

			int positionX = shoot.startPositionX;
			int positionY = shoot.startPositionY;
			while( positionX >= 0 && positionX < 16 &&
				positionY >= 0 && positionY < 16 )
			{
				for( std::map< uint32, ClientState >::iterator it = m_clients.begin(); it != m_clients.end(); ++it )
				{
					ClientState& target = it->second;
					if( target.playerId == client.playerId ||
						target.positionX != positionX ||
						target.positionY != positionY )
					{
						continue;
					}

					killPlayer( target, &client );
				}

				positionX += shootX;
				positionY += shootY;
			}

			shoot.endPositionX	= positionX;
			shoot.endPositionY	= positionY;

			m_shoots.push_back( shoot );

			printf( "Player %d shoot from (%d, %d) to (%d, %d)\n", client.playerId, shoot.startPositionX, shoot.startPositionY, shoot.endPositionX, shoot.endPositionY );
		}
	}

	void Server::sendGameState()
	{
		MessageGameState gameState;
		uint8 i = 0u;
		for( const std::pair< uint32, ClientState >& kvp : m_clients )
		{
			MessagePlayerState& playerState = gameState.players[ i ];
			playerState.playerId	= kvp.second.playerId;
			playerState.positionX	= kvp.second.positionX;
			playerState.positionY	= kvp.second.positionY;

			i++;
			if( i == 8u )
			{
				break;
			}
		}
		gameState.playerCount = i;

		i = 0;
		for( const ShootState& shoot : m_shoots )
		{
			MessageShootState& shootState = gameState.shoots[ i ];
			shootState.playerId			= shoot.playerId;
			shootState.startPositionX	= shoot.startPositionX;
			shootState.startPositionY	= shoot.startPositionY;
			shootState.endPositionX		= shoot.endPositionX;
			shootState.endPositionY		= shoot.endPositionY;

			i++;
			if( i == 8u )
			{
				break;
			}
		}
		gameState.shootCount = i;

		for( const std::pair< uint32, ClientState >& kvp : m_clients )
		{
			sendMessage( kvp.second, MessageType::GameState, &gameState, sizeof( gameState ) );
		}
	}

	void Server::sendMessage( const ClientState& client, MessageType type, const void* pData, uint16 dataSize )
	{
		if( dataSize > 2048u - sizeof( MessageHeader ) )
		{
			printf( "Packet for %08x is to large(%d bytes)\n", client.address.sin_addr.s_addr, dataSize );
			return;
		}

		MessageHeader header;
		header.messageSize	= dataSize;
		header.messageType	= type;

		char buffer[ 2048u ];
		memcpy( buffer, &header, sizeof( header ) );
		memcpy( buffer + sizeof( header ), pData, dataSize );

		const int sendResult = sendto( m_serverSocket, buffer, sizeof( header ) + dataSize, MSG_DONTWAIT, (const sockaddr*)&client.address, sizeof( client.address ) );
		if( sendResult < 0 )
		{
			printf( "Failed to send packet to %08x(result: %d)\n", client.address.sin_addr.s_addr, sendResult );
		}
	}

	void Server::killPlayer( ClientState& target, ClientState* pKiller )
	{
		const int position = target.playerId % 4u;

		switch( position )
		{
		case 0:
			target.positionX = 1;
			target.positionY = 1;
			break;

		case 1:
			target.positionX = 14;
			target.positionY = 1;
			break;

		case 2:
			target.positionX = 1;
			target.positionY = 14;
			break;

		case 3:
			target.positionX = 14;
			target.positionY = 14;
			break;

		default:
			break;
		}

		if( pKiller != nullptr )
		{
			pKiller->kills++;
		}
	}
}
