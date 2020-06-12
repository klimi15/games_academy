#pragma once

#include "../01-Window/SampleTypes.hpp"
#include "../02-Triangle/SampleShader.hpp"

struct D3D11_INPUT_ELEMENT_DESC;
struct ID3D11DeviceContext;

namespace GamesAcademy
{
	class SampleDevice;
	class SampleFilesystem;
	struct SampleFileWatch;

	class SampleShaderVariants
	{
	public:

										SampleShaderVariants();
										~SampleShaderVariants();

		bool							createVertexShaderFromFile( SampleDevice& device, SampleFilesystem& filesystem, const char* pFilename, const D3D11_INPUT_ELEMENT_DESC* pVertexElements, size_t vertexElementCount );
		bool							createHullShaderFromFile( SampleDevice& device, SampleFilesystem& filesystem, const char* pFilename );
		bool							createDomainShaderFromFile( SampleDevice& device, SampleFilesystem& filesystem, const char* pFilename );
		bool							createGeometryShaderFromFile( SampleDevice& device, SampleFilesystem& filesystem, const char* pFilename );
		bool							createPixelShaderFromFile( SampleDevice& device, SampleFilesystem& filesystem, const char* pFilename );
		bool							createComputeShaderFromFile( SampleDevice& device, SampleFilesystem& filesystem, const char* pFilename );
		void							destroy();

		void							apply( ID3D11DeviceContext* pContext, size_t variant = 0u );

	private:

		SampleDevice*					m_pDevice				= nullptr;
		SampleFilesystem*				m_pFilesystem			= nullptr;
		SampleFileWatch*				m_pFileWatch			= nullptr;

		SampleShader*					m_pShaders				= nullptr;
		size_t							m_shaderCount			= 0u;
		SampleShaderType				m_shaderType			= SampleShaderType::VertexShader;

		const D3D11_INPUT_ELEMENT_DESC* m_pVertexElements		= nullptr;
		size_t							m_vertexElementCount	= 0u;

		size_t							readVariants( char* pVarantNameBuffer, size_t variantNameBufferSize, const char* pFileContent ) const;

		bool							createShaderFromFileInternal( SampleDevice& device, SampleFilesystem& filesystem, const char* pFilename, SampleShaderType type, bool destroyOnFailure );
	};
}
