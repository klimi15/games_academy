#pragma once

#include <framework.h>

#include "Enemy.h"
#include "Projectile.h"

#include <vector>

namespace GamesAcademy
{
	class Player
	{
	public:

		void						create( ga_graphics_t* pGraphics );

		bool						hasHit( const Enemy& enemy );

		void						kill();

		void						update( ga_graphics_t* pGraphics, ga_input_t* pInput, float deltaTime );
		void						render( ga_graphics_t* pGraphics );

	private:

		static constexpr float		PositionSpeedX	= 4096.0f;
		static constexpr float		PositionOffsetY	= 64.0f;
		static constexpr float		HalfSize		= 32.0f;

		bool						m_isDead;
		float						m_targetX;
		float						m_positionX;
		float						m_positionY;

		std::vector< Projectile >	m_projectles;

		void						shoot();
	};
}
