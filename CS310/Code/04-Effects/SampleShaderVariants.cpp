#include "SampleShaderVariants.hpp"

#include "../01-Window/SampleDevice.hpp"
#include "../01-Window/SampleWindow.hpp"
#include "../02-Triangle/SampleShader.hpp"
#include "../03-Box/SampleFilesystem.hpp"

#include <d3d11.h>
#include <stdio.h>

namespace GamesAcademy
{
	class SampleShaderVariantsIncludeHandler : public ID3DInclude
	{
	public:

		SampleShaderVariantsIncludeHandler( SampleFilesystem& filesystem, const char* pFilename )
			: m_filesystem( filesystem )
		{
			const char* pLastSlash = strrchr( pFilename, '/' );
			const size_t pathPartLength = (pLastSlash - pFilename) + 1u;
			strncpy_s( m_localPath, pFilename, pathPartLength );
		}

		STDMETHOD( Open )(THIS_ D3D_INCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID* ppData, UINT* pBytes) override final
		{
			MemoryBlock data;
			if( IncludeType == D3D_INCLUDE_LOCAL )
			{
				char fullPath[ 256u ];
				sprintf_s( fullPath, sizeof( fullPath ), "%s%s", m_localPath, pFileName );

				data = m_filesystem.readFile( fullPath );
			}
			else
			{
				data = m_filesystem.readFile( pFileName );
			}

			if( data.pData == nullptr )
			{
				return ERROR_FILE_NOT_FOUND;
			}

			*ppData	= data.pData;
			*pBytes	= UINT( data.size );

			return S_OK;
		}

		STDMETHOD( Close )(THIS_ LPCVOID pData) override final
		{
			m_filesystem.freeFile( { pData, 0u } );
			return S_OK;
		}

	private:

		char				m_localPath[ 256u ];
		SampleFilesystem&	m_filesystem;
	};

	SampleShaderVariants::SampleShaderVariants()
	{
	}

	SampleShaderVariants::~SampleShaderVariants()
	{
		destroy();
	}

	bool SampleShaderVariants::createVertexShaderFromFile( SampleDevice& device, SampleFilesystem& filesystem, const char* pFilename, const D3D11_INPUT_ELEMENT_DESC* pVertexElements, size_t vertexElementCount )
	{
		D3D11_INPUT_ELEMENT_DESC* pNewVertexElements = new D3D11_INPUT_ELEMENT_DESC[ vertexElementCount ];
		memcpy( pNewVertexElements, pVertexElements, sizeof( D3D11_INPUT_ELEMENT_DESC ) * vertexElementCount );

		m_pVertexElements		= pNewVertexElements;
		m_vertexElementCount	= vertexElementCount;

		return createShaderFromFileInternal( device, filesystem, pFilename, SampleShaderType::VertexShader, true );
	}

	bool SampleShaderVariants::createHullShaderFromFile( SampleDevice& device, SampleFilesystem& filesystem, const char* pFilename )
	{
		return createShaderFromFileInternal( device, filesystem, pFilename, SampleShaderType::HullShader, true );
	}

	bool SampleShaderVariants::createDomainShaderFromFile( SampleDevice& device, SampleFilesystem& filesystem, const char* pFilename )
	{
		return createShaderFromFileInternal( device, filesystem, pFilename, SampleShaderType::DomainShader, true );
	}

	bool SampleShaderVariants::createGeometryShaderFromFile( SampleDevice& device, SampleFilesystem& filesystem, const char* pFilename )
	{
		return createShaderFromFileInternal( device, filesystem, pFilename, SampleShaderType::GeometryShader, true );
	}

	bool SampleShaderVariants::createPixelShaderFromFile( SampleDevice& device, SampleFilesystem& filesystem, const char* pFilename )
	{
		return createShaderFromFileInternal( device, filesystem, pFilename, SampleShaderType::PixelShader, true );
	}

	bool SampleShaderVariants::createComputeShaderFromFile( SampleDevice& device, SampleFilesystem& filesystem, const char* pFilename )
	{
		return createShaderFromFileInternal( device, filesystem, pFilename, SampleShaderType::ComputeShader, true );
	}

	void SampleShaderVariants::destroy()
	{
		if( m_pFileWatch != nullptr )
		{
			m_pFilesystem->stopWatchFile( m_pFileWatch );
			m_pFileWatch = nullptr;
		}

		if( m_pShaders != nullptr )
		{
			delete[] m_pShaders;
			m_pShaders = nullptr;
		}

		if( m_pVertexElements != nullptr )
		{
			delete[] m_pVertexElements;
			m_pVertexElements = nullptr;
		}

		m_vertexElementCount	= 0u;
		m_pDevice				= nullptr;
		m_pFilesystem			= nullptr;
		m_shaderCount			= 0u;
		m_shaderType			= SampleShaderType::VertexShader;
	}

	void SampleShaderVariants::apply( ID3D11DeviceContext* pContext, size_t variant /* = 0u */ )
	{
		if( m_pFilesystem->hasWatchedFileChanged( m_pFileWatch ) )
		{
			SampleShader* pShaderBackup		= m_pShaders;
			const size_t shaderCountBackup	= m_shaderCount;
			m_pShaders						= nullptr;
			m_shaderCount					= 0u;

			if( createShaderFromFileInternal( *m_pDevice, *m_pFilesystem, m_pFilesystem->getWatchedFileName( m_pFileWatch ), m_shaderType, false ) )
			{
				delete[] pShaderBackup;
			}
			else
			{
				m_pShaders		= pShaderBackup;
				m_shaderCount	= shaderCountBackup;
			}
		}

		if( variant >= m_shaderCount )
		{
			OutputDebugStringW( L"Shader variant out of range.\n" );
			return;
		}

		m_pShaders[ variant ].apply( pContext );
	}

	size_t SampleShaderVariants::readVariants( char* pVarantNameBuffer, size_t variantNameBufferSize, const char* pFileContent ) const
	{
		if( pFileContent[ 0u ] != '/' ||
			pFileContent[ 1u ] != '/' ||
			pFileContent[ 2u ] != ' ' )
		{
			return 1u;
		}

		pFileContent += 3u;

		while( *pFileContent != '=' && *pFileContent != '\n' && *pFileContent != '\0' )
		{
			if( variantNameBufferSize == 1u )
			{
				return 1u;
			}

			*pVarantNameBuffer++ = *pFileContent++;
			variantNameBufferSize--;
		}
		*pVarantNameBuffer = '\0';

		if( *pFileContent != '=' ||
			pFileContent[ 1u ] < '0' ||
			pFileContent[ 1u ] > '9' )
		{
			return 1u;
		}

		return size_t( pFileContent[ 1u ] ) - '0';
	}

	bool SampleShaderVariants::createShaderFromFileInternal( SampleDevice& device, SampleFilesystem& filesystem, const char* pFilename, SampleShaderType type, bool destroyOnFailure )
	{
		const MemoryBlock data = filesystem.readFile( pFilename );
		if( data.pData == nullptr )
		{
			return false;
		}

		SampleShaderVariantsIncludeHandler includeHandler( filesystem, pFilename );

		char variantNameBuffer[ 64u ];
		m_shaderCount = readVariants( variantNameBuffer, ARRAY_COUNT( variantNameBuffer ), (const char*)data.pData );
		m_pShaders = new SampleShader[ m_shaderCount ];

		bool result = true;
		for( size_t i = 0u; i < m_shaderCount; ++i )
		{
			char defineBuffer[ 16u ];
			sprintf_s( defineBuffer, sizeof( defineBuffer ), "%zd", i );

			D3D_SHADER_MACRO defines[ 2u ];
			defines[ 0u ].Name			= variantNameBuffer;
			defines[ 0u ].Definition	= defineBuffer;
			defines[ 1u ].Name			= nullptr;
			defines[ 1u ].Definition	= nullptr;

			const D3D_SHADER_MACRO* pDefines = m_shaderCount > 1u ? defines : nullptr;
			if( type == SampleShaderType::VertexShader )
			{
				result &= m_pShaders[ i ].createVertexShaderInternal( device, data, pFilename, m_pVertexElements, m_vertexElementCount, pDefines, &includeHandler );
			}
			else
			{
				result &= m_pShaders[ i ].createShaderInternal( device, data, pFilename, type, pDefines, &includeHandler );
			}
		}

		free( (void*)data.pData );

		if( !result )
		{
			if( destroyOnFailure )
			{
				destroy();
			}
			return false;
		}

		m_pDevice		= &device;
		m_pFilesystem	= &filesystem;
		m_shaderType	= type;

		if( m_pFileWatch == nullptr )
		{
			m_pFileWatch = filesystem.startWatchFile( pFilename );
		}

		return true;
	}
}
