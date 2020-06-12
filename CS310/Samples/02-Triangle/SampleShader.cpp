#include "SampleShader.hpp"

#include "../01-Window/SampleDevice.hpp"
#include "../01-Window/SampleWindow.hpp"

#include <d3d11.h>
#include <d3dcompiler.h>

namespace GamesAcademy
{
	SampleShader::SampleShader()
	{
	}

	SampleShader::~SampleShader()
	{
		destroy();
	}

	bool SampleShader::createVertexShaderFromMemory( SampleDevice& device, const MemoryBlock& data, const D3D11_INPUT_ELEMENT_DESC* pVertexElements, size_t vertexElementCount )
	{
		return createVertexShaderInternal( device, data, "vertex_shader", pVertexElements, vertexElementCount, nullptr, nullptr );
	}

	bool SampleShader::createPixelShaderFromMemory( SampleDevice& device, const MemoryBlock& data )
	{
		return createShaderInternal( device, data, "pixel_shader", SampleShaderType::PixelShader, nullptr, nullptr );
	}

	void SampleShader::destroy()
	{
		SAFE_RELEASE( m_pVertexLayout );

		SAFE_RELEASE( m_pVertexShader );
		SAFE_RELEASE( m_pHullShader );
		SAFE_RELEASE( m_pDomainShader );
		SAFE_RELEASE( m_pGeometryShader );
		SAFE_RELEASE( m_pPixelShader );
		SAFE_RELEASE( m_pComputeShader );
	}

	void SampleShader::apply( ID3D11DeviceContext* pContext ) const
	{
		if (m_pVertexShader != nullptr)
		{
			pContext->IASetInputLayout( m_pVertexLayout );
			pContext->VSSetShader( m_pVertexShader, nullptr, 0u );
		}
		else if (m_pHullShader != nullptr)
		{
			pContext->HSSetShader( m_pHullShader, nullptr, 0u );
		}
		else if (m_pDomainShader != nullptr)
		{
			pContext->DSSetShader( m_pDomainShader, nullptr, 0u );
		}
		else if( m_pGeometryShader != nullptr )
		{
			pContext->GSSetShader( m_pGeometryShader, nullptr, 0u );
		}
		else if( m_pPixelShader != nullptr )
		{
			pContext->PSSetShader( m_pPixelShader, nullptr, 0u );
		}
		else if (m_pComputeShader != nullptr)
		{
			pContext->CSSetShader( m_pComputeShader, nullptr, 0u );
		}
		else
		{
			OutputDebugStringW( L"Failed to set Shader.\n" );
		}
	}

	bool SampleShader::createVertexShaderInternal( SampleDevice& device, const MemoryBlock& data, const char* pFilename, const D3D11_INPUT_ELEMENT_DESC* pVertexElements, size_t vertexElementCount, const _D3D_SHADER_MACRO* pDefines, ID3DInclude* pIncludeHandler )
	{
		ID3DBlob* pShaderBlob = compileShader( device, data, pFilename, "vs_5_0", pDefines, pIncludeHandler );
		if( pShaderBlob == nullptr )
		{
			destroy();
			return false;
		}

		HRESULT result = device.getDevice()->CreateVertexShader( pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), nullptr, &m_pVertexShader );
		if( FAILED( result ) )
		{
			destroy();
			pShaderBlob->Release();
			device.getWindow().showMessageBox( L"Failed to create Vertex Shader." );
			return false;
		}

		result = device.getDevice()->CreateInputLayout( pVertexElements, UINT( vertexElementCount ), pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), &m_pVertexLayout );
		if( FAILED( result ) )
		{
			destroy();
			pShaderBlob->Release();
			device.getWindow().showMessageBox( L"Failed to create Input Layout." );
			return false;
		}

		pShaderBlob->Release();

		return true;
	}

	bool SampleShader::createShaderInternal( SampleDevice& device, const MemoryBlock& data, const char* pFilename, SampleShaderType type, const _D3D_SHADER_MACRO* pDefines, ID3DInclude* pIncludeHandler )
	{
		const char* pTarget = nullptr;
		switch( type )
		{
		case SampleShaderType::VertexShader:	pTarget = "vs_5_0"; break;
		case SampleShaderType::HullShader:		pTarget = "hs_5_0"; break;
		case SampleShaderType::DomainShader:	pTarget = "ds_5_0"; break;
		case SampleShaderType::GeometryShader:	pTarget = "gs_5_0"; break;
		case SampleShaderType::PixelShader:		pTarget = "ps_5_0"; break;
		case SampleShaderType::ComputeShader:	pTarget = "cs_5_0"; break;
		}

		ID3DBlob* pShaderBlob = compileShader( device, data, pFilename, pTarget, pDefines, pIncludeHandler );
		if( pShaderBlob == nullptr )
		{
			destroy();
			return false;
		}

		HRESULT result = S_FALSE;
		switch( type )
		{
		case SampleShaderType::VertexShader:
			result = device.getDevice()->CreateVertexShader( pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), nullptr, &m_pVertexShader );
			break;

		case SampleShaderType::HullShader:
			result = device.getDevice()->CreateHullShader( pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), nullptr, &m_pHullShader );
			break;

		case SampleShaderType::DomainShader:
			result = device.getDevice()->CreateDomainShader( pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), nullptr, &m_pDomainShader );
			break;

		case SampleShaderType::GeometryShader:
			result = device.getDevice()->CreateGeometryShader( pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), nullptr, &m_pGeometryShader );
			break;

		case SampleShaderType::PixelShader:
			result = device.getDevice()->CreatePixelShader( pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), nullptr, &m_pPixelShader );
			break;

		case SampleShaderType::ComputeShader:
			result = device.getDevice()->CreateComputeShader( pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), nullptr, &m_pComputeShader );
			break;
		}

		if( FAILED( result ) )
		{
			destroy();
			pShaderBlob->Release();
			device.getWindow().showMessageBox( L"Failed to create Shader." );
			return false;
		}

		pShaderBlob->Release();
		return true;
	}

	ID3D10Blob* SampleShader::compileShader( const SampleDevice& device, const MemoryBlock& data, const char* pFilename, const char* pTarget, const _D3D_SHADER_MACRO* pDefines, ID3DInclude* pIncludeHandler )
	{
		if( pIncludeHandler == nullptr )
		{
			pIncludeHandler = D3D_COMPILE_STANDARD_FILE_INCLUDE;
		}

		ID3DBlob* pShaderBlob = nullptr;
		ID3DBlob* pErrorBlob = nullptr;
		UINT flags = 0u;
#ifdef _DEBUG
		flags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
		HRESULT result = D3DCompile( data.pData, data.size, pFilename, pDefines, pIncludeHandler, "main", pTarget, flags, 0u, &pShaderBlob, &pErrorBlob );
		if( FAILED( result ) )
		{
			if( pErrorBlob != nullptr )
			{
				WCHAR wideBuffer[ 2048u ];
				MultiByteToWideChar( CP_UTF8, 0, (const char*)pErrorBlob->GetBufferPointer(), -1, wideBuffer, ARRAY_COUNT( wideBuffer ) );

				device.getWindow().showMessageBox( wideBuffer );

				pErrorBlob->Release();
				pErrorBlob = nullptr;
			}
			else
			{
				device.getWindow().showMessageBox( L"Failed to compile Shader." );
			}

			return nullptr;
		}

		if( pErrorBlob != nullptr )
		{
			pErrorBlob->Release();
			pErrorBlob = nullptr;
		}

		return pShaderBlob;
	}
}
