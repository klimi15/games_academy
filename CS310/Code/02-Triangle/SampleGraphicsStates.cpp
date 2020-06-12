#include "SampleGraphicsStates.hpp"

#include "../01-Window/SampleDevice.hpp"

#include <d3d11.h>

namespace GamesAcademy
{
	SampleGraphicsStates::SampleGraphicsStates()
	{
	}

	SampleGraphicsStates::~SampleGraphicsStates()
	{
		destroy();
	}

	bool SampleGraphicsStates::create( SampleDevice& device )
	{
		{
			D3D11_BLEND_DESC blendDesc = {};
			blendDesc.RenderTarget[ 0u ].SrcBlend				= D3D11_BLEND_SRC_ALPHA;
			blendDesc.RenderTarget[ 0u ].DestBlend				= D3D11_BLEND_INV_SRC_ALPHA;
			blendDesc.RenderTarget[ 0u ].BlendOp				= D3D11_BLEND_OP_ADD;
			blendDesc.RenderTarget[ 0u ].SrcBlendAlpha			= D3D11_BLEND_ONE;
			blendDesc.RenderTarget[ 0u ].DestBlendAlpha			= D3D11_BLEND_ZERO;
			blendDesc.RenderTarget[ 0u ].BlendOpAlpha			= D3D11_BLEND_OP_ADD;
			blendDesc.RenderTarget[ 0u ].RenderTargetWriteMask	= D3D11_COLOR_WRITE_ENABLE_ALL;

			for( size_t i = 0u; i < ARRAY_COUNT( m_blendStates ); ++i )
			{
				blendDesc.RenderTarget[ 0u ].BlendEnable		= BOOL( i );

				HRESULT result = device.getDevice()->CreateBlendState( &blendDesc, &m_blendStates[ i ] );
				if( FAILED( result ) )
				{
					destroy();
					return false;
				}
			}
		}

		{
			D3D11_DEPTH_STENCIL_DESC depthDesc = {};

			for( size_t i = 0u; i < ARRAY_COUNT( m_depthStates ); ++i )
			{
				depthDesc.DepthEnable = BOOL( i );

				for( size_t j = 0u; j < ARRAY_COUNT( m_depthStates[ i ] ); ++j )
				{
					switch( (SampleDepthCompare)j )
					{
					case SampleDepthCompare::Never:			depthDesc.DepthFunc = D3D11_COMPARISON_NEVER; break;
					case SampleDepthCompare::Less:			depthDesc.DepthFunc = D3D11_COMPARISON_LESS; break;
					case SampleDepthCompare::Equal:			depthDesc.DepthFunc = D3D11_COMPARISON_EQUAL; break;
					case SampleDepthCompare::LessEqual:		depthDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL; break;
					case SampleDepthCompare::Greater:		depthDesc.DepthFunc = D3D11_COMPARISON_GREATER; break;
					case SampleDepthCompare::NotEqual:		depthDesc.DepthFunc = D3D11_COMPARISON_NOT_EQUAL; break;
					case SampleDepthCompare::GreaterEqual:	depthDesc.DepthFunc = D3D11_COMPARISON_GREATER_EQUAL; break;
					case SampleDepthCompare::Always:		depthDesc.DepthFunc = D3D11_COMPARISON_ALWAYS; break;
					case SampleDepthCompare::Count:			return false;
					}

					HRESULT result = device.getDevice()->CreateDepthStencilState( &depthDesc, &m_depthStates[ i ][ j ] );
					if( FAILED( result ) )
					{
						destroy();
						return false;
					}
				}
			}
		}

		{
			D3D11_RASTERIZER_DESC rasterizerDesc = {};

			for( size_t i = 0u; i < ARRAY_COUNT( m_rasterizerStates ); ++i )
			{
				switch( (SampleFillMode)i )
				{
				case SampleFillMode::Wireframe:	rasterizerDesc.FillMode			= D3D11_FILL_WIREFRAME; break;
				case SampleFillMode::Solid:		rasterizerDesc.FillMode			= D3D11_FILL_SOLID; break;
				case SampleFillMode::Count:		return false;
				}

				for( size_t j = 0u; j < ARRAY_COUNT( m_rasterizerStates[ i ]); ++j )
				{
					switch( (SampleCullMode)j )
					{
					case SampleCullMode::None:		rasterizerDesc.CullMode		= D3D11_CULL_NONE; break;
					case SampleCullMode::Front:		rasterizerDesc.CullMode		= D3D11_CULL_FRONT; break;
					case SampleCullMode::Back:		rasterizerDesc.CullMode		= D3D11_CULL_BACK; break;
					case SampleCullMode::Count:		return false;
					}

					HRESULT result = device.getDevice()->CreateRasterizerState( &rasterizerDesc, &m_rasterizerStates[ i ][ j ] );
					if( FAILED( result ) )
					{
						destroy();
						return false;
					}
				}
			}
		}

		{
			D3D11_SAMPLER_DESC samplerDesc = {};
			samplerDesc.MaxLOD			= D3D11_FLOAT32_MAX;
			samplerDesc.MaxAnisotropy	= 16u;

			for( size_t i = 0u; i < ARRAY_COUNT( m_samplerStates ); ++i )
			{
				switch( (SampleSamplerFilter)i )
				{
				case SampleSamplerFilter::Point:		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT; break;
				case SampleSamplerFilter::Linear:		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR; break;
				case SampleSamplerFilter::Anisotropic:	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC; break;
				case SampleSamplerFilter::Count:		return false;
				}

				for( size_t j = 0u; j < ARRAY_COUNT( m_samplerStates[ i ] ); ++j )
				{
					switch( (SampleSamplerAddressMode)j )
					{
					case SampleSamplerAddressMode::Wrap:
						samplerDesc.AddressU	= D3D11_TEXTURE_ADDRESS_WRAP;
						samplerDesc.AddressV	= D3D11_TEXTURE_ADDRESS_WRAP;
						samplerDesc.AddressW	= D3D11_TEXTURE_ADDRESS_WRAP;
						break;

					case SampleSamplerAddressMode::Clamp:
						samplerDesc.AddressU	= D3D11_TEXTURE_ADDRESS_CLAMP;
						samplerDesc.AddressV	= D3D11_TEXTURE_ADDRESS_CLAMP;
						samplerDesc.AddressW	= D3D11_TEXTURE_ADDRESS_CLAMP;
						break;

					case SampleSamplerAddressMode::Mirror:
						samplerDesc.AddressU	= D3D11_TEXTURE_ADDRESS_MIRROR;
						samplerDesc.AddressV	= D3D11_TEXTURE_ADDRESS_MIRROR;
						samplerDesc.AddressW	= D3D11_TEXTURE_ADDRESS_MIRROR;
						break;
					}

					HRESULT result = device.getDevice()->CreateSamplerState( &samplerDesc, &m_samplerStates[ i ][ j ] );
					if( FAILED( result ) )
					{
						destroy();
						return false;
					}
				}
			}
		}

		return true;
	}

	void SampleGraphicsStates::destroy()
	{
		for( size_t i = 0u; i < ARRAY_COUNT( m_blendStates ); ++i )
		{
			if( m_blendStates[ i ] == nullptr )
			{
				continue;
			}

			m_blendStates[ i ]->Release();
			m_blendStates[ i ] = nullptr;
		}

		for( size_t i = 0u; i < ARRAY_COUNT( m_depthStates ); ++i )
		{
			for( size_t j = 0u; j < ARRAY_COUNT( m_depthStates[ i ] ); ++j )
			{
				if( m_depthStates[ i ][ j ] == nullptr )
				{
					continue;
				}

				m_depthStates[ i ][ j ]->Release();
				m_depthStates[ i ][ j ] = nullptr;
			}
		}

		for( size_t i = 0u; i < ARRAY_COUNT( m_rasterizerStates ); ++i )
		{
			for( size_t j = 0u; j < ARRAY_COUNT( m_rasterizerStates[ i ] ); ++j )
			{
				if( m_rasterizerStates[ i ][ j ] == nullptr )
				{
					continue;
				}

				m_rasterizerStates[ i ][ j ]->Release();
				m_rasterizerStates[ i ][ j ] = nullptr;
			}
		}

		for( size_t i = 0u; i < ARRAY_COUNT( m_samplerStates ); ++i )
		{
			for( size_t j = 0u; j < ARRAY_COUNT( m_samplerStates[ i ] ); ++j )
			{
				if( m_samplerStates[ i ][ j ] == nullptr )
				{
					continue;
				}

				m_samplerStates[ i ][ j ]->Release();
				m_samplerStates[ i ][ j ] = nullptr;
			}
		}
	}

	void SampleGraphicsStates::applyBlendState( ID3D11DeviceContext* pContext, bool enabled /* = false */ ) const
	{
		 ID3D11BlendState* pBlendState = m_blendStates[ enabled ? 1u : 0u ];
		 float blendFactor[ 4u ] = { 1.0f, 1.0f, 1.0f, 1.0f };
		 pContext->OMSetBlendState( pBlendState, blendFactor, 0xffffffffu );
	}

	void SampleGraphicsStates::applyDepthState( ID3D11DeviceContext* pContext, bool enabled /* = true */, SampleDepthCompare compare /* = SampleDepthCompare::LessEqual */ ) const
	{
		ID3D11DepthStencilState* pDepthState = m_depthStates[ enabled ? 1u : 0u ][ (uint8)compare ];
		pContext->OMSetDepthStencilState( pDepthState, 0u );
	}

	void SampleGraphicsStates::applyRasterizerState( ID3D11DeviceContext* pContext, SampleFillMode fillMode /* = SampleFillMode::Solid */, SampleCullMode cullMode /* = SampleCullMode::Back */ ) const
	{
		ID3D11RasterizerState* pRasterizerState = m_rasterizerStates[ (uint8)fillMode ][ (uint8)cullMode ];
		pContext->RSSetState( pRasterizerState );
	}

	void SampleGraphicsStates::applySamplerStateToDomainShader( ID3D11DeviceContext* pContext, size_t slotIndex, SampleSamplerFilter filter /*= SampleSamplerFilter::Linear*/, SampleSamplerAddressMode mode /*= SampleSamplerAddressMode::Wrap */ ) const
	{
		ID3D11SamplerState* pSamplerState = m_samplerStates[ (uint8)filter ][ (uint8)mode ];
		pContext->DSSetSamplers( UINT( slotIndex ), 1u, &pSamplerState );
	}

	void SampleGraphicsStates::applySamplerStateToPixelShader( ID3D11DeviceContext* pContext, size_t slotIndex, SampleSamplerFilter filter /* = SampleSamplerFilter::Linear */, SampleSamplerAddressMode mode /* = SampleSamplerAddressMode::Wrap */ ) const
	{
		ID3D11SamplerState* pSamplerState = m_samplerStates[ (uint8)filter ][ (uint8)mode ];
		pContext->PSSetSamplers( UINT( slotIndex ), 1u, &pSamplerState );
	}
}
