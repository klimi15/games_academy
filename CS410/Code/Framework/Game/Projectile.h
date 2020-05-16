#pragma once

#include <framework.h>

#include "Enemy.h"

namespace GamesAcademy
{
	class Projectile
	{
	public:

		void					create( float positionX, float positionY );

		bool					checkCollistion( const Enemy& enemy ) const;
		bool					hasBorderReached() const;

		void					update( ga_graphics_t* pGraphics, double deltaTime );
		void					render( ga_graphics_t* pGraphics );

	private:

		static constexpr float	SizeX	= 5.0f;
		static constexpr float	SizeY	= 25.0f;
		static constexpr float	SpeedY	= 300.0f;

		float					m_positionX;
		float					m_positionY;
	};
}
