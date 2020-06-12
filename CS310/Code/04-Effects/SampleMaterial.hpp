#pragma once

#include "../03-Box/SampleTexture.hpp"

struct ID3D11DeviceContext;

namespace GamesAcademy
{
	enum class SampleMaterialMap : uint8
	{
		Color,
		Normal,
		AmbientOcclusion,
		Roughness,
		Metalness,

		Count
	};

	class SampleMaterial
	{
	public:

								SampleMaterial();
								~SampleMaterial();

		bool					createFromFiles( SampleDevice& device, SampleFilesystem& filesystem, const char* pFileBasename );
		void					destroy();

		const SampleTexture&	getMap( SampleMaterialMap map ) const { return m_maps[ (uint8)map ]; }

		void					apply( ID3D11DeviceContext* pContext, size_t slotOffset = 0u );

	private:

		SampleTexture			m_maps[ (uint8)SampleMaterialMap::Count ];
	};
}
