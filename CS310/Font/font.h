#pragma once

#include <d3d11.h>

namespace GamesAcademy
{
	class Graphics;

	class Font
	{
	public:

		bool					create( Graphics& graphics, const char* pTtfFilepath, int sizePerChar );
		void					destroy();

		void					draw( Graphics& graphics, float x, float y, float fontSize, const char* pText );

	private:

		struct ShaderCharInfo
		{
			float				u0;
			float				v0;
			float				u1;
			float				v1;
		};

		static const int		FirstChar				= 32u;
		static const int		LastChar				= 126u;
		static const int		EndChar					= LastChar + 1u;
		static const int		CharCount				= EndChar - FirstChar;

		ID3D11VertexShader*		m_pVertexShader			= nullptr;
		ID3D11PixelShader*		m_pPixelShader			= nullptr;
		ID3D11InputLayout*		m_pInputLayout			= nullptr;
		ID3D11Buffer*			m_pVertexBuffer			= nullptr;

		ID3D11Texture2D*		m_pSdfTexture			= nullptr;

		ShaderCharInfo			n_chars[ CharCount ];
	};
}
