#include "SampleCamera.hpp"

#include "../01-Window/SampleWindow.hpp"
#include "../02-Triangle/SampleTimer.hpp"

#include "SampleInput.hpp"

namespace GamesAcademy
{
	SampleCamera::SampleCamera()
	{
	}

	SampleCamera::~SampleCamera()
	{
		destroy();
	}

	void SampleCamera::create( SampleInput& input, bool enabled, XMVECTOR position, XMFLOAT2 orientation, float speed )
	{
		m_pInput		= &input;
		m_enabled		= enabled;
		m_speed			= speed;
		m_position		= position;
		m_orientation	= orientation;

		m_pInput->getWindow().setPinMouse( enabled );
	}

	void SampleCamera::destroy()
	{
		if( m_enabled )
		{
			m_pInput->getWindow().setPinMouse( false );
		}

		m_pInput		= nullptr;
		m_enabled		= false;
		m_position		= XMVectorZero();
		m_orientation	= { 0.0f, 0.0f };
	}

	void SampleCamera::update( const SampleTimer& timer )
	{
		if( m_pInput->wasMouseButtonPressed( SampleInputMouseButton::Right ) )
		{
			m_enabled = !m_enabled;
			m_pInput->getWindow().setPinMouse( m_enabled );
		}

		if( !m_enabled )
		{
			return;
		}

		m_orientation.x += m_pInput->getMouseDeltaY() / m_pInput->getWindow().getClientWidth();
		m_orientation.y += m_pInput->getMouseDeltaX() / m_pInput->getWindow().getClientHeight();

		XMFLOAT2 movement ={};
		if( m_pInput->isKeyboardKeyDown( SampleInputKeyboardKey::Left ) || m_pInput->isKeyboardKeyDown( SampleInputKeyboardKey::A ) )
		{
			movement.x -= 1.0f;
		}
		if( m_pInput->isKeyboardKeyDown( SampleInputKeyboardKey::Right ) || m_pInput->isKeyboardKeyDown( SampleInputKeyboardKey::D ) )
		{
			movement.x += 1.0f;
		}
		if( m_pInput->isKeyboardKeyDown( SampleInputKeyboardKey::Up ) || m_pInput->isKeyboardKeyDown( SampleInputKeyboardKey::W ) )
		{
			movement.y += 1.0f;
		}
		if( m_pInput->isKeyboardKeyDown( SampleInputKeyboardKey::Down ) || m_pInput->isKeyboardKeyDown( SampleInputKeyboardKey::S ) )
		{
			movement.y -= 1.0f;
		}

		const XMVECTOR cameraOrientation	= XMQuaternionRotationRollPitchYawFromVector( XMLoadFloat2( &m_orientation ) );
		const XMVECTOR cameraForward		= XMVector3Rotate( XMVectorSet( 0.0f, 0.0f, 1.0f, 0.0f ), cameraOrientation );
		const XMVECTOR cameraLeft			= XMVector3Rotate( XMVectorSet( 1.0f, 0.0f, 0.0f, 0.0f ), cameraOrientation );

		float cameraSpeed = m_speed;
		if( m_pInput->isKeyboardKeyDown( SampleInputKeyboardKey::Shift ) )
		{
			cameraSpeed *= 10.0f;
		}

		m_position	= XMVectorMultiplyAdd( cameraLeft, XMVectorReplicate( float( movement.x * timer.getDeltaTime() * cameraSpeed ) ), m_position );
		m_position	= XMVectorMultiplyAdd( cameraForward, XMVectorReplicate( float( movement.y * timer.getDeltaTime() * cameraSpeed ) ), m_position );
	}

	XMVECTOR SampleCamera::getPosition() const
	{
		return m_position;
	}

	XMVECTOR SampleCamera::getForward() const
	{
		const XMVECTOR cameraOrientation = XMQuaternionRotationRollPitchYawFromVector( XMLoadFloat2( &m_orientation ) );
		return XMVector3Rotate( XMVectorSet( 0.0f, 0.0f, 1.0f, 0.0f ), cameraOrientation );
	}

	XMMATRIX SampleCamera::getViewMatrix() const
	{
		return XMMatrixLookToLH( m_position, getForward(), XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0 ) );
	}
}
