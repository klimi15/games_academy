#include "SampleTexture.hpp"

#include "../01-Window/SampleDevice.hpp"
#include "../01-Window/SampleTypes.hpp"
#include "../01-Window/SampleWindow.hpp"

#include "SampleFilesystem.hpp"

#include <DDSTextureLoader/DDSTextureLoader.h>
#include <d3d11.h>

namespace GamesAcademy
{
	SampleTexture::SampleTexture()
	{
	}

	SampleTexture::~SampleTexture()
	{
		destroy();
	}

	bool SampleTexture::createFromFile( SampleDevice& device, SampleFilesystem& filesystem, const char* pFilename )
	{
		MemoryBlock fileData = filesystem.readFile( pFilename );
		if( fileData.pData == nullptr )
		{
			return false;
		}

		const HRESULT result = DirectX::CreateDDSTextureFromMemory( device.getDevice(), (const uint8*)fileData.pData, fileData.size, (ID3D11Resource**)&m_pTexture, &m_pShaderResourceView );
		filesystem.freeFile( fileData );

		if( FAILED( result ) )
		{
			device.getWindow().showMessageBox( L"Failed to load Texture." );
			return false;
		}

		D3D11_TEXTURE2D_DESC desc;
		m_pTexture->GetDesc( &desc );

		m_width			= uint16( desc.Width );
		m_height		= uint16( desc.Height );

		m_pDevice		= &device;
		m_pFilesystem	= &filesystem;
		m_pFileWatch	= filesystem.startWatchFile( pFilename );

		return true;
	}

	bool SampleTexture::createEmptyShadowMap( SampleDevice& device, uint16 width, uint16 height )
	{
		m_width		= width;
		m_height	= height;

		D3D11_TEXTURE2D_DESC textureDesc = {};
		textureDesc.Width				= width;
		textureDesc.Height				= height;
		textureDesc.MipLevels			= 1u;
		textureDesc.ArraySize			= 1u;
		textureDesc.Format				= DXGI_FORMAT_R16_TYPELESS;
		textureDesc.SampleDesc.Count	= 1u;
		textureDesc.Usage				= D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags			= D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;

		HRESULT result = device.getDevice()->CreateTexture2D( &textureDesc, nullptr, &m_pTexture );
		if( FAILED( result ) )
		{
			device.getWindow().showMessageBox( L"Failed to create Shadow Map." );
			return false;
		}

		D3D11_SHADER_RESOURCE_VIEW_DESC shaderView = {};
		shaderView.Format				= DXGI_FORMAT_R16_UNORM;
		shaderView.ViewDimension		= D3D11_SRV_DIMENSION_TEXTURE2D;
		shaderView.Texture2D.MipLevels	= 1u;

		result = device.getDevice()->CreateShaderResourceView( m_pTexture, &shaderView, &m_pShaderResourceView );
		if( FAILED( result ) )
		{
			device.getWindow().showMessageBox( L"Failed to create Shadow Map Shader View." );
			return false;
		}

		D3D11_DEPTH_STENCIL_VIEW_DESC depthView = {};
		depthView.Format				= DXGI_FORMAT_D16_UNORM;
		depthView.ViewDimension			= D3D11_DSV_DIMENSION_TEXTURE2D;

		result = device.getDevice()->CreateDepthStencilView( m_pTexture, &depthView, &m_pDepthStencilView );
		if( FAILED( result ) )
		{
			device.getWindow().showMessageBox( L"Failed to create Shadow Map Render Target View." );
			return false;
		}

		return true;
	}

	void SampleTexture::destroy()
	{
		if( m_pFileWatch != nullptr )
		{
			m_pFilesystem->stopWatchFile( m_pFileWatch );
			m_pFileWatch = nullptr;
		}

		if( m_pDepthStencilView != nullptr )
		{
			m_pDepthStencilView->Release();
			m_pDepthStencilView = nullptr;
		}

		if( m_pShaderResourceView != nullptr )
		{
			m_pShaderResourceView->Release();
			m_pShaderResourceView = nullptr;
		}

		if( m_pTexture != nullptr )
		{
			m_pTexture->Release();
			m_pTexture = nullptr;
		}

		m_height		= 0u;
		m_width			= 0u;
		m_pFilesystem	= nullptr;
		m_pDevice		= nullptr;
	}

	void SampleTexture::applyToDomainShader( ID3D11DeviceContext* pContext, size_t slotIndex )
	{
		checkForChanges();

		pContext->DSSetShaderResources( UINT( slotIndex ), 1u, &m_pShaderResourceView );
	}

	void SampleTexture::applyToPixelShader( ID3D11DeviceContext* pContext, size_t slotIndex )
	{
		checkForChanges();

		pContext->PSSetShaderResources( UINT( slotIndex ), 1u, &m_pShaderResourceView );
	}

	void SampleTexture::applyRenderTargetAndClear( ID3D11DeviceContext* pContext )
	{
		pContext->ClearDepthStencilView( m_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0u );
		pContext->OMSetRenderTargets( 0u, nullptr, m_pDepthStencilView );

		D3D11_VIEWPORT viewport = {};
		viewport.Width		= (float)m_width;
		viewport.Height		= (float)m_height;
		viewport.MaxDepth	= 1.0f;
		pContext->RSSetViewports( 1u, &viewport );
	}

	void SampleTexture::checkForChanges()
	{
		if( !m_pFilesystem->hasWatchedFileChanged( m_pFileWatch ) )
		{
			return;
		}

		ID3D11Texture2D* pTextureBackup			= m_pTexture;
		ID3D11ShaderResourceView* pViewBackup	= m_pShaderResourceView;

		if( createFromFile( *m_pDevice, *m_pFilesystem, m_pFilesystem->getWatchedFileName( m_pFileWatch ) ) )
		{
			pViewBackup->Release();
			pTextureBackup->Release();
		}
		else
		{
			m_pTexture				= pTextureBackup;
			m_pShaderResourceView	= pViewBackup;
		}
	}
}
