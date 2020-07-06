#include "client.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <minmax.h>

#include <stdio.h>

namespace GamesAcademy
{
	template <typename T> T sgn( T val )
	{
		return T((T( 0 ) < val) - (val < T( 0 )));
	}

	int Client::run()
	{
		if( !create() )
		{
			return 1;
		}

		m_connection.login( "Tim" );

		while( m_graphics.isWindowOpen() )
		{
			update();
			render();
		}

		destroy();
		return 0;
	}

	bool Client::create()
	{
		if( !m_graphics.create( 1280u, 720u, "Font" ) )
		{
			return false;
		}

		if( !m_font.create( m_graphics, "C:\\Windows\\Fonts\\arial.ttf", 64 ) )
		{
			return false;
		}

		if( !m_input.create( m_graphics ) )
		{
			return false;
		}

		if( !m_connection.create() )
		{
			return false;
		}

		m_timer.create();

		return true;
	}

	void Client::destroy()
	{
		m_connection.destroy();
		m_input.destroy();
		m_font.destroy();
		m_graphics.destroy();
	}

	void Client::update()
	{
		m_input.update();
		m_connection.update( m_timer.get() );

		if( m_connection.getState() != ConnectionState::Playing )
		{
			return;
		}

		const MessageGameState& gameState = m_connection.getGameState();
		const MessagePlayerState* pPlayer = nullptr;
		for( size_t i = 0u; i < gameState.playerCount; ++i )
		{
			if( gameState.players[ i ].playerId != m_connection.getPlayerId() )
			{
				continue;
			}

			pPlayer = &gameState.players[ i ];
		}

		if( pPlayer == nullptr )
		{
			return;
		}

		const float mouseOffsetX	= m_input.getMousePositionX() - (getCellPosition( pPlayer->positionX ) + m_halfSize);
		const float mouseOffsetY	= m_input.getMousePositionY() - (getCellPosition( pPlayer->positionY ) + m_halfSize);
		const float mouseAngle		= atan2f( mouseOffsetY, mouseOffsetX );
		const float angleRange		= float( M_PI * 2.0f ) / 8.0f;
		const float angleValue		= mouseAngle / angleRange;

		MessagePlayerActionType actionType = MessagePlayerActionType::Invalid;
		if( angleValue > -3.5f && angleValue < -2.5f )
		{
			actionType			= MessagePlayerActionType::ShootUpLeft;
			m_playerInput		= PlayerInputState::Shoot;
			m_playerTargetX		= -1.0f;
			m_playerTargetY		= -1.0f;
		}
		else if( angleValue > -2.5f && angleValue < -1.5f )
		{
			actionType			= MessagePlayerActionType::MoveUp;
			m_playerInput		= PlayerInputState::Walk;
			m_playerTargetX		= 0.0f;
			m_playerTargetY		= -1.0f;
		}
		else if( angleValue > -1.5f && angleValue < -0.5f )
		{
			actionType			= MessagePlayerActionType::ShootUpRight;
			m_playerInput		= PlayerInputState::Shoot;
			m_playerTargetX		= 1.0f;
			m_playerTargetY		= -1.0f;
		}
		else if( angleValue > -0.5f && angleValue < 0.5f )
		{
			actionType			= MessagePlayerActionType::MoveRight;
			m_playerInput		= PlayerInputState::Walk;
			m_playerTargetX		= 1.0f;
			m_playerTargetY		= 0.0f; 
		}
		else if( angleValue > 0.5f && angleValue < 1.5f )
		{
			actionType			= MessagePlayerActionType::ShootDownRight;
			m_playerInput		= PlayerInputState::Shoot;
			m_playerTargetX		= 1.0f;
			m_playerTargetY		= 1.0f;
		}
		else if( angleValue > 1.5f && angleValue < 2.5f )
		{
			actionType			= MessagePlayerActionType::MoveDown;
			m_playerInput		= PlayerInputState::Walk;
			m_playerTargetX		= 0.0f;
			m_playerTargetY		= 1.0f;
		}
		else if( angleValue > 2.5f && angleValue < 3.5f )
		{
			actionType			= MessagePlayerActionType::ShootDownLeft;
			m_playerInput		= PlayerInputState::Shoot;
			m_playerTargetX		= -1.0f;
			m_playerTargetY		= 1.0f;
		}
		else
		{
			actionType			= MessagePlayerActionType::MoveLeft;
			m_playerInput		= PlayerInputState::Walk;
			m_playerTargetX		= -1.0f;
			m_playerTargetY		= 0.0f;
		}

		if( actionType != MessagePlayerActionType::Invalid &&
			m_input.wasMouseButtonPressed( 0u ) )
		{
			m_connection.action( actionType );
		}
	}

	void Client::render()
	{
		m_graphics.beginFrame();
		m_graphics.clear( 0xff000000 );

		if( m_connection.getState() != ConnectionState::Playing )
		{
			m_graphics.drawText( 50.0f, 50.0f, 24.0f, m_font, "Connecting...", 0xffffffffu );
		}

		m_minSize		= min( m_graphics.getBackBufferWidth(), m_graphics.getBackBufferHeight() );
		m_border		= m_minSize * 0.05f;
		m_spacing		= 2.0f;
		m_cellSize		= (m_minSize - (2.0f * m_border) - (16.0f * m_spacing)) / 16.0f;
		m_halfSize		= m_cellSize * 0.5f;
		m_playerBegin	= m_cellSize * 0.1f;
		m_playerEnd		= m_cellSize * 0.9f;

		for( uint8 y = 0u; y < 16u; ++y )
		{
			for( uint8 x = 0u; x < 16u; ++x )
			{
				const float cellX = getCellPosition( x );
				const float cellY = getCellPosition( y );

				m_graphics.drawRect( cellX, cellY, m_cellSize, m_cellSize, 0xffdd6622u );
			}
		}

		const MessageGameState& gameState = m_connection.getGameState();
		const MessagePlayerState* pPlayer = nullptr;
		for( uint8 playerIndex = 0u; playerIndex < gameState.playerCount; ++playerIndex )
		{
			const MessagePlayerState& player = gameState.players[ playerIndex ];

			if( player.playerId == m_connection.getPlayerId() )
			{
				pPlayer = &player;
			}

			const float playerX = getCellPosition( player.positionX );
			const float playerY = getCellPosition( player.positionY );

			m_graphics.drawTriangle(
				playerX + m_halfSize,		playerY + m_playerBegin,
				playerX + m_playerEnd,		playerY + m_playerEnd,
				playerX + m_playerBegin,	playerY + m_playerEnd,
				0xff77cc44u
			);
		}

		if( pPlayer != nullptr )
		{
			const float playerX = getCellPosition( pPlayer->positionX );
			const float playerY = getCellPosition( pPlayer->positionY );

			if( m_playerInput == PlayerInputState::Walk )
			{
				const float targetX	= playerX + (m_playerTargetX * m_cellSize);
				const float targetY	= playerY + (m_playerTargetY * m_cellSize);

				m_graphics.drawTriangle(
					targetX + m_halfSize,		targetY + m_playerBegin,
					targetX + m_playerEnd,		targetY + m_playerEnd,
					targetX + m_playerBegin,	targetY + m_playerEnd,
					0xff3a6622u
				);
			}
			else
			{
				const float targetX			= playerX + (m_playerTargetX * m_halfSize) + m_halfSize;
				const float targetY			= playerY + (m_playerTargetY * m_halfSize) + m_halfSize;
				const float offsetX			= m_playerTargetX * m_halfSize;
				const float offsetY			= m_playerTargetY * m_halfSize;
				const float largeOffsetX	= offsetX * 5.0f;
				const float largeOffsetY	= offsetY * 5.0f;

				if( m_playerTargetX != m_playerTargetY )
				{
					m_graphics.drawTriangle(
						targetX, targetY + offsetY,
						targetX + largeOffsetX, targetY + largeOffsetY,
						targetX + offsetX, targetY,
						0xff1144ddu
					);
				}
				else
				{
					m_graphics.drawTriangle(
						targetX, targetY + offsetY,
						targetX + offsetX, targetY,
						targetX + largeOffsetX, targetY + largeOffsetY,
						0xff1144ddu
					);
				}
			}

		}

		m_graphics.endFrame();
	}

	float Client::getCellPosition( uint8 pos ) const
	{
		return m_border + (pos * (m_cellSize + m_spacing)) + m_spacing;
	}
}
