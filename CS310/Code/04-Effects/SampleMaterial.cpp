#include "SampleMaterial.hpp"

#include "../03-Box/SampleFilesystem.hpp"

#include <stdio.h>

namespace GamesAcademy
{
	static const char* s_materialFilenames[] =
	{
		"color",
		"normal",
		"ambient_occlusion",
		"roughness",
		"metalness"
	};

	static const char* s_materialDefaultFilenames[] =
	{
		"Content/default_color.dds",
		"Content/default_normal.dds",
		"Content/default_ambient_occlusion.dds",
		"Content/default_roughness.dds",
		"Content/default_metalness.dds"
	};

	SampleMaterial::SampleMaterial()
	{
		static_assert( ARRAY_COUNT( s_materialFilenames ) == ARRAY_COUNT( m_maps ), "" );
		static_assert( ARRAY_COUNT( s_materialDefaultFilenames ) == ARRAY_COUNT( m_maps ), "" );
	}

	SampleMaterial::~SampleMaterial()
	{
		destroy();
	}

	bool SampleMaterial::createFromFiles( SampleDevice& device, SampleFilesystem& filesystem, const char* pFileBasename )
	{
		for( size_t i = 0u; i < ARRAY_COUNT( m_maps ); ++i )
		{
			char filenameBuffer[ 256u ];
			sprintf_s( filenameBuffer, ARRAY_COUNT( filenameBuffer ), "%s_%s.dds", pFileBasename, s_materialFilenames[ i ] );

			if( !filesystem.doesFileExist( filenameBuffer ) )
			{
				if( !m_maps[ i ].createFromFile( device, filesystem, s_materialDefaultFilenames[ i ] ) )
				{
					destroy();
					return false;
				}
			}
			else if( !m_maps[ i ].createFromFile( device, filesystem, filenameBuffer ) )
			{
				destroy();
				return false;
			}
		}


		return true;
	}

	void SampleMaterial::destroy()
	{
		for( size_t i = 0u; i < ARRAY_COUNT( m_maps ); ++i )
		{
			m_maps[ i ].destroy();
		}
	}

	void SampleMaterial::apply( ID3D11DeviceContext* pContext, size_t slotOffset /* = 0u */ )
	{
		for( size_t i = 0u; i < ARRAY_COUNT( m_maps ); ++i )
		{
			m_maps[ i ].applyToPixelShader( pContext, slotOffset + i );
		}
	}
}
