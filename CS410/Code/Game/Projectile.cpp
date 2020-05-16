#include "Projectile.h"

namespace GamesAcademy
{
	void Projectile::create( float positionX, float positionY )
	{
		m_positionX = positionX;
		m_positionY	= positionY;
	}

	bool Projectile::checkCollistion( const Enemy& enemy ) const
	{
		const AABB enemyAabb = enemy.getAABB();

		const float y = m_positionY - (SizeY / 2.0f);

		return m_positionX >= enemyAabb.minX &&
			m_positionX <= enemyAabb.maxX &&
			y >= enemyAabb.minY &&
			y <= enemyAabb.maxY;
	}

	bool Projectile::hasBorderReached() const
	{
		return m_positionY < SizeY / 2.0f;
	}

	void Projectile::update( ga_graphics_t* pGraphics, double deltaTime )
	{
		m_positionY -= float( SpeedY * deltaTime );
	}

	void Projectile::render( ga_graphics_t* pGraphics )
	{
		const float x = m_positionX - (SizeX / 2.0f);
		const float y = m_positionY - (SizeY / 2.0f);

		ga_graphics_draw_rect( pGraphics, x, y, SizeX, SizeY, ga_graphics_color_rgb( 0xff, 0x00, 0x00 ) );
	}
}