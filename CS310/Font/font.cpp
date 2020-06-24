#include "font.h"

#include "framework/graphics.hpp"

#define STB_TRUETYPE_IMPLEMENTATION
#include "third_party/stb_truetype.h"

#include <stdio.h>
#include <algorithm>
#include <d3d11.h>

#define ARRAY_COUNT( arr ) (sizeof(arr) / sizeof(*(arr)))

namespace GamesAcademy
{
	using uchar = unsigned char;

	struct CharInfo
	{
		char	c;
		int		width;
		int		height;
		int		originX;
		int		originY;
		uchar*	pCharData;
	};

	bool Font::create( Graphics& graphics, const char* pTtfFilepath, int sizePerChar )
	{
		void* pData = nullptr;
		{
			FILE* pFile = nullptr;
			fopen_s( &pFile, pTtfFilepath, "rb" );

			fseek( pFile, 0, SEEK_END );
			fpos_t fileSize;
			fgetpos( pFile, &fileSize );
			fseek( pFile, 0, SEEK_SET );

			pData = malloc( fileSize );
			fread( pData, fileSize, 1, pFile );
			fclose( pFile );
		}

		stbtt_fontinfo fontInfo;
		stbtt_InitFont( &fontInfo, (const uchar*)pData, 0 );

		const float scale = stbtt_ScaleForPixelHeight( &fontInfo, (float)sizePerChar );

		CharInfo chars[ 127 - 32 ];
		for( int c = 32; c < 127; c++ )
		{
			CharInfo& charInfo = chars[ c - 32u ];
			charInfo.c = (char)c;

			charInfo.pCharData = stbtt_GetGlyphSDF( &fontInfo, scale, c, 5, 180, 36.0f, &charInfo.width, &charInfo.height, &charInfo.originX, &charInfo.originY );
		}

		int textureWidth = 0;
		int textureHeight = 0;
		for( int charIndex = 0; charIndex < ARRAY_COUNT( chars ); ++charIndex )
		{
			textureWidth += chars[ charIndex ].width;
			textureHeight = std::max< int >( textureHeight, chars[ charIndex ].height );
		}

		int offsetX = 0;
		uchar* pTextureData = (uchar*)malloc( textureWidth * textureHeight );
		for( int charIndex = 0; charIndex < ARRAY_COUNT( chars ); ++charIndex )
		{
			const CharInfo& charInfo = chars[ charIndex ];

			for( int y = 0; y < charInfo.height; ++y )
			{
				const int sourceOffset = y * charInfo.width;
				const int targetOffset = (y * textureWidth) + offsetX;
				memcpy( pTextureData + targetOffset, charInfo.pCharData + sourceOffset, charInfo.width );
			}

			offsetX += charInfo.width;

			stbtt_FreeSDF( charInfo.pCharData, nullptr );
		}

		free( pData );

		D3D11_TEXTURE2D_DESC textureDesc = {};
		textureDesc.Width				= textureWidth;
		textureDesc.Height				= textureHeight;
		textureDesc.Format				= DXGI_FORMAT_R8_SINT;
		textureDesc.MipLevels			= 1u;
		textureDesc.ArraySize			= 1u;
		textureDesc.BindFlags			= D3D11_BIND_SHADER_RESOURCE;
		textureDesc.Usage				= D3D11_USAGE_IMMUTABLE;
		textureDesc.SampleDesc.Count	= 1u;

		D3D11_SUBRESOURCE_DATA textureData;
		textureData.pSysMem		= pTextureData;
		textureData.SysMemPitch	= textureWidth;

		if( graphics.getDevice()->CreateTexture2D( &textureDesc, &textureData, &m_pSdfTexture ) != S_OK )
		{
			free( pTextureData );
			return false;
		}

		free( pTextureData );
		return true;
	}

	void Font::destroy()
	{
		if( m_pSdfTexture != nullptr )
		{
			m_pSdfTexture->Release();
			m_pSdfTexture = nullptr;
		}
	}

	void Font::draw( Graphics& graphics, float x, float y, float fontSize, const char* pText )
	{

	}
}
