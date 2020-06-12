#pragma once

#include "../01-Window/SampleTypes.hpp"

struct ID3D11DepthStencilView;
struct ID3D11DeviceContext;
struct ID3D11ShaderResourceView;
struct ID3D11Texture2D;

struct ID3D11RenderTargetView;

namespace GamesAcademy
{
	class SampleDevice;
	class SampleFilesystem;
	struct SampleFileWatch;

	class SampleTexture
	{
	public:

									SampleTexture();
									~SampleTexture();

		bool						createFromFile( SampleDevice& device, SampleFilesystem& filesystem, const char* pFilename );
		bool						createEmptyShadowMap( SampleDevice& device, uint16 width, uint16 height );
		void						destroy();

		uint16						getWidth() const { return m_width; }
		uint16						getHeight() const { return m_height; }

		void						applyToDomainShader( ID3D11DeviceContext* pContext, size_t slotIndex );
		void						applyToPixelShader( ID3D11DeviceContext* pContext, size_t slotIndex );
		void						applyRenderTargetAndClear( ID3D11DeviceContext* pContext );

	private:

		SampleDevice*				m_pDevice				= nullptr;
		SampleFilesystem*			m_pFilesystem			= nullptr;

		ID3D11Texture2D*			m_pTexture				= nullptr;
		ID3D11ShaderResourceView*	m_pShaderResourceView	= nullptr;
		ID3D11DepthStencilView*		m_pDepthStencilView		= nullptr;

		uint16						m_width					= 0u;
		uint16						m_height				= 0u;

		SampleFileWatch*			m_pFileWatch			= nullptr;

		void						checkForChanges();
	};
}
