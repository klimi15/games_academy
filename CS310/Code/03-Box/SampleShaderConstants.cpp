#include "SampleShaderConstants.hpp"

#include "../01-Window/SampleDevice.hpp"
#include "../01-Window/SampleWindow.hpp"

#include <d3d11.h>

namespace GamesAcademy
{
	SampleShaderConstants::SampleShaderConstants()
	{
	}

	SampleShaderConstants::~SampleShaderConstants()
	{
		destroy();
	}

	bool SampleShaderConstants::create( SampleDevice& device, size_t size )
	{
		D3D11_BUFFER_DESC bufferDesc = {};
		bufferDesc.ByteWidth		= UINT( ALIGN_VALUE( size, 16u ) );
		bufferDesc.Usage			= D3D11_USAGE_DYNAMIC;
		bufferDesc.BindFlags		= D3D11_BIND_CONSTANT_BUFFER;
		bufferDesc.CPUAccessFlags	= D3D11_CPU_ACCESS_WRITE;

		HRESULT result = device.getDevice()->CreateBuffer( &bufferDesc, nullptr, &m_pBuffer );
		if( FAILED( result ) )
		{
			device.getWindow().showMessageBox( L"Failed to create Constant Buffer." );
			return false;
		}

		return true;
	}

	void SampleShaderConstants::destroy()
	{
		if( m_pBuffer != nullptr )
		{
			m_pBuffer->Release();
			m_pBuffer = nullptr;
		}
	}

	void SampleShaderConstants::applyToComputeShader( ID3D11DeviceContext* pContext, size_t slotIndex ) const
	{
		pContext->CSSetConstantBuffers( UINT( slotIndex ), 1u, &m_pBuffer );
	}

	void SampleShaderConstants::applyToVertexShader( ID3D11DeviceContext* pContext, size_t slotIndex ) const
	{
		pContext->VSSetConstantBuffers( UINT( slotIndex ), 1u, &m_pBuffer );
	}

	void SampleShaderConstants::applyToHullShader( ID3D11DeviceContext* pContext, size_t slotIndex ) const
	{
		pContext->HSSetConstantBuffers( UINT( slotIndex ), 1u, &m_pBuffer );
	}

	void SampleShaderConstants::applyToDomainShader( ID3D11DeviceContext* pContext, size_t slotIndex ) const
	{
		pContext->DSSetConstantBuffers( UINT( slotIndex ), 1u, &m_pBuffer );
	}

	void SampleShaderConstants::applyToGeometryShader( ID3D11DeviceContext* pContext, size_t slotIndex ) const
	{
		pContext->GSSetConstantBuffers( UINT( slotIndex ), 1u, &m_pBuffer );
	}

	void SampleShaderConstants::applyToPixelShader( ID3D11DeviceContext* pContext, size_t slotIndex ) const
	{
		pContext->PSSetConstantBuffers( UINT( slotIndex ), 1u, &m_pBuffer );
	}

	void* SampleShaderConstants::mapUntyped( ID3D11DeviceContext* pContext ) const
	{
		D3D11_MAPPED_SUBRESOURCE bufferData;
		pContext->Map( m_pBuffer, 0u, D3D11_MAP_WRITE_DISCARD, 0u, &bufferData );

		return bufferData.pData;
	}

	void SampleShaderConstants::unmap( ID3D11DeviceContext* pContext ) const
	{
		pContext->Unmap( m_pBuffer, 0u );
	}
}
