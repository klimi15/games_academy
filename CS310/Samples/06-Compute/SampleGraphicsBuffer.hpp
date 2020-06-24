#pragma once

struct ID3D11Buffer;
struct ID3D11DeviceContext;
struct ID3D11ShaderResourceView;
struct ID3D11UnorderedAccessView;

namespace GamesAcademy
{
	class SampleDevice;

	class SampleGraphicsBuffer
	{
	public:

									SampleGraphicsBuffer();
									~SampleGraphicsBuffer();

		bool						create( SampleDevice& device, size_t elementSize, size_t elementCount );
		void						destroy();

		void						applyResourceToComputeShader( ID3D11DeviceContext* pContext, size_t slotIndex ) const;
		void						applyResourceToVertexShader( ID3D11DeviceContext* pContext, size_t slotIndex ) const;
		void						applyUnorderedAccess( ID3D11DeviceContext* pContext, size_t slotIndex ) const;

	private:

		size_t						m_elementSize			= 0u;
		size_t						m_elementCount			= 0u;
		ID3D11Buffer*				m_pBuffer				= nullptr;
		ID3D11ShaderResourceView*	m_pResourceView			= nullptr;
		ID3D11UnorderedAccessView*	m_pUnorderedAccessView	= nullptr;
	};
}
