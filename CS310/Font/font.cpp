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
		int		width;
		int		height;
		int		originX;
		int		originY;
		uchar*	pCharData;
	};

	struct FontVertex
	{
		float	position[ 2u ];
		float	uv[ 2u ];
	};

	bool Font::create( Graphics& graphics, const char* pTtfFilepath, int sizePerChar )
	{
		m_sdfSize = float( sizePerChar );

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

		CharInfo chars[ CharCount ];
		for( int c = FirstChar; c < EndChar; c++ )
		{
			CharInfo& charInfo = chars[ c - FirstChar ];
			charInfo.pCharData = stbtt_GetCodepointSDF( &fontInfo, scale, c, 5, 180, 36.0f, &charInfo.width, &charInfo.height, &charInfo.originX, &charInfo.originY );
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
		for( int charIndex = 0; charIndex < CharCount; ++charIndex )
		{
			const CharInfo& charInfo = chars[ charIndex ];

			for( int y = 0; y < charInfo.height; ++y )
			{
				const int sourceOffset = y * charInfo.width;
				const int targetOffset = (y * textureWidth) + offsetX;
				memcpy( pTextureData + targetOffset, charInfo.pCharData + sourceOffset, charInfo.width );
			}

			FontCharInfo& shaderInfo = m_chars[ charIndex ];
			shaderInfo.width	= float( charInfo.width );
			shaderInfo.height	= float( charInfo.height );
			shaderInfo.offsetY	= float( charInfo.originY ) + float( textureHeight );
			shaderInfo.u0		= float( offsetX ) / float( textureWidth );
			shaderInfo.v0		= 0.0f;
			shaderInfo.u1		= float( offsetX + charInfo.width ) / float( textureWidth );
			shaderInfo.v1		= float( charInfo.height ) / float( textureHeight );

			offsetX += charInfo.width;

			stbtt_FreeSDF( charInfo.pCharData, nullptr );
		}

		free( pData );

		D3D11_TEXTURE2D_DESC textureDesc = {};
		textureDesc.Width				= textureWidth;
		textureDesc.Height				= textureHeight;
		textureDesc.Format				= DXGI_FORMAT_R8_UNORM;
		textureDesc.MipLevels			= 1u;
		textureDesc.ArraySize			= 1u;
		textureDesc.BindFlags			= D3D11_BIND_SHADER_RESOURCE;
		textureDesc.Usage				= D3D11_USAGE_IMMUTABLE;
		textureDesc.SampleDesc.Count	= 1u;

		D3D11_SUBRESOURCE_DATA textureData;
		textureData.pSysMem		= pTextureData;
		textureData.SysMemPitch	= textureWidth;

		const bool textureResult = graphics.getDevice()->CreateTexture2D( &textureDesc, &textureData, &m_pSdfTexture ) == S_OK;
		free( pTextureData );

		if( !textureResult )
		{
			return false;
		}

		const bool viewResult = graphics.getDevice()->CreateShaderResourceView( m_pSdfTexture, nullptr, &m_pSdfView ) == S_OK;
		if( !viewResult )
		{
			destroy();
			return false;
		}

		return true;
	}

	void Font::destroy()
	{
		if( m_pSdfView != nullptr )
		{
			m_pSdfView->Release();
			m_pSdfView = nullptr;
		}

		if( m_pSdfTexture != nullptr )
		{
			m_pSdfTexture->Release();
			m_pSdfTexture = nullptr;
		}
	}
}
