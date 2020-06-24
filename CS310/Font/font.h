#pragma once

struct ID3D11DeviceContext;
struct ID3D11Texture2D;

namespace GamesAcademy
{
	class Graphics;

	class Font
	{
	public:

		bool				create( Graphics& graphics, const char* pTtfFilepath, int sizePerChar );
		void				destroy();

		void				draw( ID3D11DeviceContext* pContext, float x, float y, float fontSize, const char* pText );

	private:

		ID3D11Texture2D*	m_pSdfTexture	= nullptr;
	};
}
