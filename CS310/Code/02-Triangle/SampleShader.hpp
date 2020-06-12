#pragma once

#include "../01-Window/SampleTypes.hpp"

struct _D3D_SHADER_MACRO;
struct D3D11_INPUT_ELEMENT_DESC;
struct ID3D10Blob;
struct ID3D11ComputeShader;
struct ID3D11DeviceContext;
struct ID3D11DomainShader;
struct ID3D11GeometryShader;
struct ID3D11HullShader;
struct ID3D11InputLayout;
struct ID3D11PixelShader;
struct ID3D11VertexShader;
struct ID3DInclude;

namespace GamesAcademy
{
	class SampleDevice;
	class SampleShaderVariants;
	struct MemoryBlock;

	enum class SampleShaderType : uint8
	{
		VertexShader,
		HullShader,
		DomainShader,
		GeometryShader,
		PixelShader,
		ComputeShader
	};

	class SampleShader
	{
		friend class SampleShaderVariants;

	public:

								SampleShader();
								~SampleShader();

		bool					createVertexShaderFromMemory( SampleDevice& device, const MemoryBlock& data, const D3D11_INPUT_ELEMENT_DESC* pVertexElements, size_t vertexElementCount );
		bool					createPixelShaderFromMemory( SampleDevice& device, const MemoryBlock& data );
		void					destroy();

		void					apply( ID3D11DeviceContext* pContext ) const;

	private:

		ID3D11InputLayout*		m_pVertexLayout		= nullptr;

		ID3D11VertexShader*		m_pVertexShader		= nullptr;
		ID3D11HullShader*		m_pHullShader		= nullptr;
		ID3D11DomainShader*		m_pDomainShader		= nullptr;
		ID3D11GeometryShader*	m_pGeometryShader	= nullptr;
		ID3D11PixelShader*		m_pPixelShader		= nullptr;
		ID3D11ComputeShader*	m_pComputeShader	= nullptr;

		bool					createVertexShaderInternal( SampleDevice& device, const MemoryBlock& data, const char* pFilename, const D3D11_INPUT_ELEMENT_DESC* pVertexElements, size_t vertexElementCount, const _D3D_SHADER_MACRO* pDefines, ID3DInclude* pIncludeHandler );
		bool					createShaderInternal( SampleDevice& device, const MemoryBlock& data, const char* pFilename, SampleShaderType type, const _D3D_SHADER_MACRO* pDefines, ID3DInclude* pIncludeHandler );
		ID3D10Blob*				compileShader( const SampleDevice& device, const MemoryBlock& data, const char* pFilename, const char* pTarget, const _D3D_SHADER_MACRO* pDefines, ID3DInclude* pIncludeHandler );
	};
}
