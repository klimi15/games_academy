#pragma once

struct ID3D11Buffer;
struct ID3D11DeviceContext;

namespace GamesAcademy
{
	class SampleDevice;

	class SampleShaderConstants
	{
	public:

						SampleShaderConstants();
						~SampleShaderConstants();

		bool			create( SampleDevice& device, size_t size );
		void			destroy();

		void			applyToComputeShader( ID3D11DeviceContext* pContext, size_t slotIndex ) const;
		void			applyToVertexShader( ID3D11DeviceContext* pContext, size_t slotIndex ) const;
		void			applyToHullShader( ID3D11DeviceContext* pContext, size_t slotIndex ) const;
		void			applyToDomainShader( ID3D11DeviceContext* pContext, size_t slotIndex ) const;
		void			applyToGeometryShader( ID3D11DeviceContext* pContext, size_t slotIndex ) const;
		void			applyToPixelShader( ID3D11DeviceContext* pContext, size_t slotIndex ) const;

		template< typename T >
		T*				map( ID3D11DeviceContext* pContext ) const { return (T*)mapUntyped( pContext ); }
		void*			mapUntyped( ID3D11DeviceContext* pContext ) const;
		void			unmap( ID3D11DeviceContext* pContext ) const;

	private:

		ID3D11Buffer*	m_pBuffer	= nullptr;
	};
}

