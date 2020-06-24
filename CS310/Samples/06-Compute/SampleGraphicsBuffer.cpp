#include "SampleGraphicsBuffer.hpp"

#include "../01-Window/SampleDevice.hpp"
#include "../01-Window/SampleWindow.hpp"

#include <d3d11.h>

namespace GamesAcademy
{
	SampleGraphicsBuffer::SampleGraphicsBuffer()
	{
	}

	SampleGraphicsBuffer::~SampleGraphicsBuffer()
	{
		destroy();
	}

	bool SampleGraphicsBuffer::create( SampleDevice& device, size_t elementSize, size_t elementCount )
	{
		D3D11_BUFFER_DESC bufferDesc = {};
		bufferDesc.ByteWidth			= UINT( elementSize * elementCount );
		bufferDesc.Usage				= D3D11_USAGE_DEFAULT;
		bufferDesc.BindFlags			= D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
		bufferDesc.MiscFlags            = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		bufferDesc.StructureByteStride	= UINT( elementSize );

		HRESULT result = device.getDevice()->CreateBuffer( &bufferDesc, nullptr, &m_pBuffer );
		if( FAILED( result ) )
		{
			device.getWindow().showMessageBox( L"Failed to create Buffer." );
			return false;
		}

		D3D11_SHADER_RESOURCE_VIEW_DESC resourceViewDesc = {};
		resourceViewDesc.ViewDimension			= D3D11_SRV_DIMENSION_BUFFER;
		resourceViewDesc.Buffer.NumElements		= UINT( elementCount );

		result = device.getDevice()->CreateShaderResourceView( m_pBuffer, &resourceViewDesc, &m_pResourceView );
		if( FAILED( result ) )
		{
			device.getWindow().showMessageBox( L"Failed to create Resource View." );
			return false;
		}

		D3D11_UNORDERED_ACCESS_VIEW_DESC unorderedAccessViewDesc = {};
		unorderedAccessViewDesc.ViewDimension		= D3D11_UAV_DIMENSION_BUFFER;
		unorderedAccessViewDesc.Buffer.NumElements	= UINT( elementCount );

		result = device.getDevice()->CreateUnorderedAccessView( m_pBuffer, &unorderedAccessViewDesc, &m_pUnorderedAccessView );
		if( FAILED( result ) )
		{
			device.getWindow().showMessageBox( L"Failed to create UAV." );
			return false;
		}

		m_elementSize	= elementSize;
		m_elementCount	= elementCount;

		return true;
	}

	void SampleGraphicsBuffer::destroy()
	{
		if( m_pUnorderedAccessView != nullptr )
		{
			m_pUnorderedAccessView->Release();
			m_pUnorderedAccessView = nullptr;
		}

		if( m_pResourceView != nullptr )
		{
			m_pResourceView->Release();
			m_pResourceView = nullptr;
		}

		if( m_pBuffer != nullptr )
		{
			m_pBuffer->Release();
			m_pBuffer = nullptr;
		}

		m_elementSize	= 0u;
		m_elementCount	= 0u;
	}

	void SampleGraphicsBuffer::applyResourceToComputeShader( ID3D11DeviceContext* pContext, size_t slotIndex ) const
	{
		pContext->CSSetShaderResources( UINT( slotIndex ), 1u, &m_pResourceView );
	}

	void SampleGraphicsBuffer::applyResourceToVertexShader( ID3D11DeviceContext* pContext, size_t slotIndex ) const
	{
		pContext->VSSetShaderResources( UINT( slotIndex ), 1u, &m_pResourceView );
	}

	void SampleGraphicsBuffer::applyUnorderedAccess( ID3D11DeviceContext* pContext, size_t slotIndex ) const
	{
		UINT initialCount = UINT( -1 );
		pContext->CSSetUnorderedAccessViews( UINT( slotIndex ), 1u, &m_pUnorderedAccessView, &initialCount );
	}
}