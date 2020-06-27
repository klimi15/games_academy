#pragma once

#include "server_types.h"

#include <netinet/in.h>

#include <map>
#include <string>
#include <vector>

namespace GamesAcademy
{
	class Server
	{
	public:

		bool	create();
		void	destroy();

		void	update();

	private:

		enum class ClientStatus
		{
			Invalid,
			LoggedIn
		};

		struct ClientState
		{
			sockaddr_in		address;
			ClientStatus	status				= ClientStatus::Invalid;
			double			lastMesssageTime	= 0.0;
			std::string		username;

			uint8			playerId			= 0xffu;
			uint8			positionX			= 0u;
			uint8			positionY			= 0u;

			uint16			kills				= 0u;
		};

		struct ShootState
		{
			double			time				= 0.0;

			uint8			playerId			= 0xffu;
			uint8			startPositionX		= 0u;
			uint8			startPositionY		= 0u;
			uint8			endPositionX		= 0u;
			uint8			endPositionY		= 0u;
		};

		using ClientMap = std::map< uint32, ClientState >;
		using ShootArray = std::vector< ShootState >;

		int			m_serverSocket	= 0;

		ClientMap	m_clients;
		uint8		m_nextPlayerId	= 0u;
		ShootArray	m_shoots;

		void		receiveMessages( double serverTime );
		void		handleLoginRequestMessage( ClientState& client, const void* pMessage, uint16 messageSize );
		void		handlePlayerActionMessage( ClientState& client, const void* pMessage, uint16 messageSize, double serverTime );

		void		sendGameState();
		void		sendMessage( const ClientState& client, MessageType type, const void* pData, uint16 dataSize );

		void		killPlayer( ClientState& target, ClientState* pKiller );
	};
}
