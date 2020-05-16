#include "Game.h"

namespace GamesAcademy
{
	int Game::run()
	{
		m_pGraphics = ga_graphics_create( 1280, 720, "Space Invaders" );
		if( !m_pGraphics )
		{
			return 1;
		}

		m_pInput = ga_input_create( m_pGraphics );
		if( !m_pInput )
		{
			return 1;
		}

		m_pTimer = ga_timer_create();
		if( !m_pTimer )
		{
			return 1;
		}

		m_player.create( m_pGraphics );

		m_enemies.resize( EnemyCount );

		const float halfScreenWidth = ga_graphics_get_back_buffer_width( m_pGraphics ) / 2.0f;
		for( int y = 0; y < EnemyCountY; ++y )
		{
			EnemyType type;
			if( y == 0 )
			{
				type = EnemyType::Type1;
			}
			else if( y > 0 && y < 3 )
			{
				type = EnemyType::Type2;
			}
			else if( y >= 3 )
			{
				type = EnemyType::Type3;
			}

			for( int x = 0; x < EnemyCountX; ++x )
			{
				const int enemyIndex = (y * EnemyCountX) + x;
				Enemy& enemy = m_enemies[ enemyIndex ];

				const float positionX = halfScreenWidth + ((x - 5) * EnemyDistanceX);
				const float positionY = EnemyBasePositionY + (y * EnemyDistanceY);

				enemy.create( positionX, positionY, type );
			}
		}

		while( ga_graphics_is_window_open( m_pGraphics ) )
		{
			update();
			render();
		}

		ga_timer_destroy( m_pTimer );
		m_pTimer = nullptr;

		ga_input_destroy( m_pInput );
		m_pInput = nullptr;

		ga_graphics_destroy( m_pGraphics );
		m_pGraphics = nullptr;

		return 0;
	}

	void Game::update()
	{
		const double gameTime	= ga_timer_get( m_pTimer );
		const double deltaTime	= gameTime - m_lastGameTime;
		m_lastGameTime = gameTime;

		ga_input_update( m_pInput );

		m_player.update( m_pGraphics, m_pInput, float( deltaTime ) );

		const float inverseProgress = float( m_enemies.size() ) / EnemyCount;
		const float progress		= 1.0f - inverseProgress;
		const float speed			= (EnemyBaseSpeed * inverseProgress) + (EnemyMaxSpeed * progress);

		bool isOver = false;
		bool switchDirection = false;
		const float screenWidth = ga_graphics_get_back_buffer_width( m_pGraphics );
		const float screenHeight = ga_graphics_get_back_buffer_height( m_pGraphics );
		for( int i = 0; i < m_enemies.size(); ++i )
		{
			Enemy& enemy = m_enemies[ i ];

			enemy.update( m_pGraphics, speed, m_enemyDirectionX, deltaTime );

			if( m_player.hasHit( enemy ) )
			{
				m_enemies.erase( m_enemies.begin() + i );
				i--;
			}

			isOver |= enemy.hasBottomReached( screenHeight );
			switchDirection |= enemy.hasBorderReached( screenWidth, m_enemyDirectionX );
		}

		if( isOver )
		{
			m_player.kill();
		}

		if( switchDirection )
		{
			m_enemyDirectionX *= -1.0f;

			for( int i = 0; i < m_enemies.size(); ++i )
			{
				m_enemies[ i ].moveDown( EnemyDistanceY );
			}
		}
	}

	void Game::render()
	{
		ga_graphics_begin_frame( m_pGraphics );
		ga_graphics_clear( m_pGraphics, ga_graphics_color_rgb( 0x00, 0x00, 0x00 ) );

		m_player.render( m_pGraphics );

		for( int i = 0; i < m_enemies.size(); ++i )
		{
			m_enemies[ i ].render( m_pGraphics );
		}

		ga_graphics_end_frame( m_pGraphics );
	}
}
