#pragma once

#include <d3d11.h>

namespace GamesAcademy
{
	class Graphics;

	struct FontCharInfo
	{
		float				width;
		float				height;
		float				offsetY;
		float				u0;
		float				v0;
		float				u1;
		float				v1;
	};

	class Font
	{
	public:

		bool						create( Graphics& graphics, const char* pTtfFilepath, int sizePerChar );
		void						destroy();

		float						getSize() const { return m_sdfSize; }
		ID3D11ShaderResourceView*	getTextureView() const { return m_pSdfView; }
		const FontCharInfo&			getCharInfo( int index ) const { return m_chars[ index - FirstChar ]; }

	private:

		static const int			FirstChar		= 33u;
		static const int			LastChar		= 126u;
		static const int			EndChar			= LastChar + 1u;
		static const int			CharCount		= EndChar - FirstChar;

		float						m_sdfSize		= 0;
		ID3D11Texture2D*			m_pSdfTexture	= nullptr;
		ID3D11ShaderResourceView*	m_pSdfView		= nullptr;

		FontCharInfo				m_chars[ CharCount ];
	};
}
