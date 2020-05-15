#include "Player.h"

namespace GamesAcademy
{
	void Player::create( ga_graphics_t* pGraphics )
	{
		m_isDead	= false;
		m_positionX	= ga_graphics_get_back_buffer_width( pGraphics ) / 2.0f;
		m_targetX	= m_positionX;
	}

	bool Player::hasHit( const Enemy& enemy )
	{
		for( int i = 0; i < m_projectles.size(); ++i )
		{
			if( m_projectles[ i ].checkCollistion( enemy ) )
			{
				m_projectles.erase( m_projectles.begin() + i );
				return true;
			}
		}

		return false;
	}

	void Player::kill()
	{
		m_isDead = true;
		m_projectles.clear();
	}

	void Player::update( ga_graphics_t* pGraphics, ga_input_t* pInput, float deltaTime )
	{
		if( m_isDead )
		{
			return;
		}

		const float screenWidth = ga_graphics_get_back_buffer_width( pGraphics );

		m_targetX = ga_input_get_mouse_position_x( pInput );
		m_positionX = m_targetX;
		//m_positionX -= ((m_positionX - m_targetX) / screenWidth) * PositionSpeedX * deltaTime;

		if( m_positionX < HalfSize )
		{
			m_positionX = HalfSize;
		}
		else if( m_positionX > screenWidth - HalfSize )
		{
			m_positionX = screenWidth - HalfSize;
		}

		m_positionY = ga_graphics_get_back_buffer_height( pGraphics ) - PositionOffsetY;

		if( ga_input_was_mouse_button_pressed( pInput, ga_input_mouse_button_left ) )
		{
			shoot();
		}

		for( int i = 0; i < m_projectles.size(); ++i )
		{
			Projectile& projectile = m_projectles[ i ];

			projectile.update( pGraphics, deltaTime );

			if( projectile.hasBorderReached() )
			{
				m_projectles.erase( m_projectles.begin() + i );
				i--;
			}
		}
	}

	void Player::render( ga_graphics_t* pGraphics )
	{
		ga_graphics_color color = ga_graphics_color_rgb( 0x2d, 0xfe, 0x08 );
		if( m_isDead )
		{
			color = ga_graphics_color_rgb( 0xff, 0x00, 0x0d );
		}

		//  2
		// / \
		// 1--3
		const float x1 = m_positionX - HalfSize;
		const float y1 = m_positionY + HalfSize;
		const float x2 = m_positionX;
		const float y2 = m_positionY - HalfSize;
		const float x3 = m_positionX + HalfSize;
		const float y3 = m_positionY + HalfSize;
		ga_graphics_draw_triangle( pGraphics, x1, y1, x2, y2, x3, y3, color );

		for( Projectile& projectile : m_projectles )
		{
			projectile.render( pGraphics );
		}
	}

	void Player::shoot()
	{
		if( !m_projectles.empty() )
		{
			return;
		}

		Projectile projectile;
		projectile.create( m_positionX, m_positionY );
		m_projectles.push_back( projectile );
	}
}