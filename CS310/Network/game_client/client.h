#pragma once

#include "framework/font.h"
#include "framework/graphics.hpp"
#include "framework/input.hpp"
#include "framework/timer.hpp"

#include "connection.h"

namespace GamesAcademy
{
	class Client
	{
	public:

		int			run();

	private:

		enum class PlayerInputState
		{
			Walk,
			Shoot
		};

		Graphics			m_graphics;
		Font				m_font;
		Input				m_input;
		Timer				m_timer;

		Connection			m_connection;

		float				m_minSize			= 0.0f;
		float				m_border			= 0.0f;
		float				m_spacing			= 0.0f;
		float				m_cellSize			= 0.0f;
		float				m_halfSize			= 0.0f;
		float				m_playerBegin		= 0.0f;
		float				m_playerEnd			= 0.0f;

		PlayerInputState	m_playerInput		=  PlayerInputState::Walk;
		float				m_playerTargetX		= 0.0f;
		float				m_playerTargetY		= 0.0f;

		bool		create();
		void		destroy();

		void		update();
		void		render();
		void		renderShoot( const MessageShootState& shoot );

		float		getCellPosition( uint8 pos ) const;
	};
}
