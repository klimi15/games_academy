#pragma once

#include <framework.h>

namespace GamesAcademy
{
	struct AABB
	{
		float	minX;
		float	minY;
		float	maxX;
		float	maxY;
	};

	enum class EnemyType
	{
		Type1,
		Type2,
		Type3
	};

	class Enemy
	{
	public:

		void					create( float positionX, float positionY, EnemyType type );

		bool					hasBottomReached( float screenHeight ) const;
		bool					hasBorderReached( float screenWidth, float directionX ) const;
		AABB					getAABB() const;

		void					moveDown( float offsetY );

		void					update( ga_graphics_t* pGraphics, float speedX, float directionX, double deltaTime );
		void					render( ga_graphics_t* pGraphics );

	private:

		static constexpr float	BottomOffset	= 100.0f;
		static constexpr float	HalfSize		= 20.0f;

		float					m_positionX;
		float					m_positionY;
		EnemyType				m_type;
	};
}
