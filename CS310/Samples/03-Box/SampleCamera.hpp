#pragma once

#include <DirectXMath.h>

namespace GamesAcademy
{
	using namespace DirectX;

	class SampleInput;
	class SampleTimer;

	class SampleCamera
	{
	public:

						SampleCamera();
						~SampleCamera();

		void			create( SampleInput& input, bool enabled, XMVECTOR position, XMFLOAT2 orientation, float speed );
		void			destroy();

		void			update( const SampleTimer& timer );

		XMVECTOR		getPosition() const;
		XMVECTOR		getForward() const;
		XMMATRIX		getViewMatrix() const;

	private:

		SampleInput*	m_pInput		= nullptr;

		bool			m_enabled		= false;
		float			m_speed			= 10.0f;
		XMVECTOR		m_position		= XMVectorZero();
		XMFLOAT2		m_orientation	= { 0.0f, 0.0f };
	};
}
