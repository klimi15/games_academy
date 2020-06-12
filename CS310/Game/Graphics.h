#pragma once

#include "Types.h"

#include <stdint.h>

struct HWND__;
using HWND = HWND__*;

struct ID3D10Blob;
struct ID3D11Buffer;
struct ID3D11DepthStencilView;
struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11DeviceContext1;
struct ID3D11InputLayout;
struct ID3D11PixelShader;
struct ID3D11RenderTargetView;
struct ID3D11Texture2D;
struct ID3D11VertexShader;
struct IDXGISwapChain;

namespace GA
{
	using GraphicsShaderHandle = uintptr_t;

	struct GraphicsVertexPosition3_Uv2
	{
		float	position[ 3u ];
		float	uv[ 2u ];
	};


	enum class GraphicsVertexFormat
	{
		Position3_Uv2
	};

	class Graphics
	{
	public:

		bool						create( const char* pWindowTitleUtf8, uint16_t width, uint16_t height );
		void						destroy();

		void						update();

		GraphicsShaderHandle		createVertexShader( MemoryBlock data, GraphicsVertexFormat vertexFormat );
		GraphicsShaderHandle		createPixelShader( MemoryBlock data );

		ID3D11DeviceContext*		beginFrame( const float backBufferClearColor[ 4u ] );
		void						endFrame();

		void						applyShader( GraphicsShaderHandle shaderHandle );
		void*						applyVertexConstantData( uint32_t slotIndex, size_t size );
		void*						applyPixelConstantData( uint32_t slotIndex, size_t size );

		bool						isOpen() const { return m_isOpen; }
		HWND						getHandle() const { return m_windowHandle; }
		ID3D11Device*				getDevice() { return m_pDevice; }

		void						showMessageBox( const wchar_t* pMessage ) const;

		bool						handleMessage( unsigned int messageCode, size_t wParam );

	private:

		struct Shader
		{
			Shader*					pNext				= nullptr;

			ID3D11InputLayout*		pVertexLayout		= nullptr;

			ID3D11VertexShader*		pVertexShader		= nullptr;
			ID3D11PixelShader*		pPixelShader		= nullptr;
		};

		HWND						m_windowHandle		= nullptr;
		wchar_t*					m_pWindowTitle		= nullptr;

		bool						m_isOpen			= false;
		int							m_clientWidth		= 0;
		int							m_clientHeight		= 0;

		IDXGISwapChain*				m_pSwapChain		= nullptr;
		ID3D11Device*				m_pDevice			= nullptr;
		ID3D11DeviceContext*		m_pContext			= nullptr;
		ID3D11DeviceContext1*		m_pContext1			= nullptr;

		int							m_backBufferWidth	= 0;
		int							m_backBufferHeight	= 0;
		ID3D11RenderTargetView*		m_pBackBufferView	= nullptr;
		ID3D11Texture2D*			m_pDepthStencil		= nullptr;
		ID3D11DepthStencilView*		m_pDepthStencilView	= nullptr;

		Shader*						m_pFirstShader		= nullptr;

		ID3D11Buffer*				m_pConstantBuffer	= nullptr;
		uint8_t*					m_pConstantData		= nullptr;
		size_t						m_constantOffset	= 0u;

		ID3D10Blob*					compileShader( const MemoryBlock& data, const char* pFilename, const char* pTarget );
	};
}

