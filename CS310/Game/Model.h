#pragma once

struct ID3D11Buffer;
struct ID3D11DeviceContext;

namespace GA
{
	class Graphics;

	class Model
	{
	public:

		bool			createPlane( Graphics& graphics );
		void			destroy();

		void			render( Graphics& graphics, ID3D11DeviceContext* pContext );

	private:

		ID3D11Buffer*	m_pVertexBuffer	= nullptr;
	};
}
