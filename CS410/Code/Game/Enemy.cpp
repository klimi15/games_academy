#include "Enemy.h"

namespace GamesAcademy
{
	void Enemy::create( float positionX, float positionY, EnemyType type )
	{
		m_positionX		= positionX;
		m_positionY		= positionY;
		m_type			= type;
	}

	bool Enemy::hasBottomReached( float screenHeight ) const
	{
		return m_positionY > screenHeight - BottomOffset;
	}

	bool Enemy::hasBorderReached( float screenWidth, float directionX ) const
	{
		if( directionX > 0.0f )
		{
			return m_positionX + HalfSize >= screenWidth;
		}
		else
		{
			return m_positionX < HalfSize;
		}
	}

	AABB Enemy::getAABB() const
	{
		const AABB aabb =
		{
			m_positionX - HalfSize,
			m_positionY - HalfSize,
			m_positionX + HalfSize,
			m_positionY + HalfSize
		};

		return aabb;
	}

	void Enemy::moveDown( float offsetY )
	{
		m_positionY += offsetY;
	}

	void Enemy::update( ga_graphics_t* pGraphics, float speedX, float directionX, double deltaTime )
	{
		m_positionX += directionX * speedX * float( deltaTime );
	}

	void Enemy::render( ga_graphics_t* pGraphics )
	{
		ga_graphics_color color = ga_graphics_color_rgb( 0xff, 0xff, 0xff );
		switch( m_type )
		{
		case EnemyType::Type1: color = ga_graphics_color_rgb( 0xa0, 0x04, 0x98 ); break;
		case EnemyType::Type2: color = ga_graphics_color_rgb( 0xfe, 0x4b, 0x03 ); break;
		case EnemyType::Type3: color = ga_graphics_color_rgb( 0x1e, 0x48, 0x8f ); break;
		}

		// 1--2
		// \ /
		//  3
		const float x1 = m_positionX - HalfSize;
		const float y1 = m_positionY - HalfSize;
		const float x2 = m_positionX + HalfSize;
		const float y2 = m_positionY - HalfSize;
		const float x3 = m_positionX;
		const float y3 = m_positionY + HalfSize;
		ga_graphics_draw_triangle( pGraphics, x1, y1, x2, y2, x3, y3, color );
	}
}
