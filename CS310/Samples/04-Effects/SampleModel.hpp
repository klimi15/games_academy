#pragma once

#include <DirectXMath.h>

struct ID3D11Buffer;
struct ID3D11DeviceContext;

namespace GamesAcademy
{
	using namespace DirectX;

	class SampleDevice;
	class SampleFilesystem;
	struct MemoryBlock;

	struct SampleModelVertex
	{
		XMFLOAT3	postion;
		XMFLOAT2	texCoord;
		XMFLOAT3	normal;
		XMFLOAT4	tangentFlip;
	};

	class SampleModel
	{
	public:

						SampleModel();
						~SampleModel();

		bool			createFromFile( SampleDevice& device, SampleFilesystem& filesystem, const char* pFilename, bool wrongNormal = false );
		bool			createFromMemory( SampleDevice& device, const MemoryBlock& vertexData, const MemoryBlock& indexData, size_t elementCount, size_t vertexStride );
		void			destroy();

		void			render( ID3D11DeviceContext* pContext ) const;

	private:

		ID3D11Buffer*	m_pVertexBuffer	= nullptr;
		ID3D11Buffer*	m_pIndexBuffer	= nullptr;
		size_t			m_elementCount	= 0u;
		size_t			m_vertexStride	= 0u;
	};
}

