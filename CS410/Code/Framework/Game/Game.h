#pragma once

#include <framework.h>

#include "Player.h"
#include "Enemy.h"

#include <vector>

namespace GamesAcademy
{
	class Game
	{
	public:

		int						run();

	private:

		static constexpr int	EnemyCountX			= 11;
		static constexpr int	EnemyCountY			= 5;
		static constexpr int	EnemyCount			= EnemyCountX * EnemyCountY;
		static constexpr float	EnemyBasePositionY	= 150.0f;
		static constexpr float	EnemyDistanceX		= 50.0f;
		static constexpr float	EnemyDistanceY		= 50.0f;
		static constexpr float	EnemyBaseSpeed		= 40.0f;
		static constexpr float	EnemyMaxSpeed		= 200.0f;

		ga_graphics_t*			m_pGraphics			= nullptr;
		ga_input_t*				m_pInput			= nullptr;
		ga_timer_t*				m_pTimer			= nullptr;

		double					m_lastGameTime		= 0.0;

		Player					m_player;
		std::vector< Enemy >	m_enemies;
		float					m_enemyDirectionX	= 1.0f;

		void					update();
		void					render();
	};
}
