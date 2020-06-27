#pragma once

#include <stdint.h>

namespace GamesAcademy
{
	using uint8 = uint8_t;
	using uint16 = uint16_t;
	using uint32 = uint32_t;
	using uint64 = uint64_t;

	using sint8 = int8_t;
	using sint16 = int16_t;
	using sint32 = int32_t;
	using sint64 = int64_t;

	enum class MessageType : uint16
	{
		Invalid,

		// Client -> Server
		LoginRequest	= 10,
		PlayerAction	= 11,

		// Server -> Client
		LoginResponse	= 20,
		GameState		= 21,
	};

	struct MessageHeader
	{
		uint16		messageSize;
		MessageType	messageType;
	};

	struct MessageLoginRequest
	{
		uint16		usernameLength;
		char		username[ 1u ];
	};

	struct MessageLoginResponse
	{
		bool		ok;
		uint8		playerId;
	};

	struct MessagePlayerState
	{
		uint8						playerId;
		uint8						positionX;
		uint8						positionY;
	};

	struct MessageShootState
	{
		uint8				playerId;
		uint8				startPositionX;
		uint8				startPositionY;
		uint8				endPositionX;
		uint8				endPositionY;
	};

	struct MessageGameState
	{
		MessagePlayerState			players[ 8u ];
		uint8						playerCount;
		MessageShootState			shoots[ 8u ];
		uint8						shootCount;
	};

	enum class MessagePlayerActionType : uint8
	{
		Invalid,
		MoveUp,
		MoveDown,
		MoveLeft,
		MoveRight,
		ShootUpLeft,
		ShootUpRight,
		ShootDownLeft,
		ShootDownRight
	};

	struct MessagePlayerAction
	{
		MessagePlayerActionType		action;
	};
}
