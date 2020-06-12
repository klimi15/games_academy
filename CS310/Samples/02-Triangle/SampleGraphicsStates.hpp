#pragma once

#include "../01-Window/SampleTypes.hpp"

struct ID3D11BlendState;
struct ID3D11DepthStencilState;
struct ID3D11DeviceContext;
struct ID3D11RasterizerState;
struct ID3D11SamplerState;

namespace GamesAcademy
{
	class SampleDevice;

	enum class SampleDepthCompare : uint8
	{
		Never,
		Less,
		Equal,
		LessEqual,
		Greater,
		NotEqual,
		GreaterEqual,
		Always,

		Count
	};

	enum class SampleFillMode : uint8
	{
		Wireframe,
		Solid,

		Count
	};

	enum class SampleCullMode : uint8
	{
		None,
		Front,
		Back,

		Count
	};

	enum class SampleSamplerFilter : uint8
	{
		Point,
		Linear,
		Anisotropic,

		Count
	};

	enum class SampleSamplerAddressMode : uint8
	{
		Wrap,
		Clamp,
		Mirror,

		Count
	};

	class SampleGraphicsStates
	{
	public:

									SampleGraphicsStates();
									~SampleGraphicsStates();

		bool						create( SampleDevice& device );
		void						destroy();

		void						applyBlendState( ID3D11DeviceContext* pContext, bool enabled = false ) const;
		void						applyDepthState( ID3D11DeviceContext* pContext, bool enabled = true, SampleDepthCompare compare = SampleDepthCompare::LessEqual ) const;
		void						applyRasterizerState( ID3D11DeviceContext* pContext, SampleFillMode fillMode = SampleFillMode::Solid, SampleCullMode cullMode = SampleCullMode::Back ) const;

		void						applySamplerStateToDomainShader( ID3D11DeviceContext* pContext, size_t slotIndex, SampleSamplerFilter filter = SampleSamplerFilter::Linear, SampleSamplerAddressMode mode = SampleSamplerAddressMode::Wrap ) const;
		void						applySamplerStateToPixelShader( ID3D11DeviceContext* pContext, size_t slotIndex, SampleSamplerFilter filter = SampleSamplerFilter::Linear, SampleSamplerAddressMode mode = SampleSamplerAddressMode::Wrap ) const;

	private:

		ID3D11BlendState*			m_blendStates[ 2u ]																				= {};
		ID3D11DepthStencilState*	m_depthStates[ 2u ][ (uint8)SampleDepthCompare::Count ]											= {};
		ID3D11RasterizerState*		m_rasterizerStates[ (uint8)SampleFillMode::Count ][ (uint8)SampleCullMode::Count ]				= {};
		ID3D11SamplerState*			m_samplerStates[ (uint8)SampleSamplerFilter::Count ][ (uint8)SampleSamplerAddressMode::Count ]	= {};
	};
}
