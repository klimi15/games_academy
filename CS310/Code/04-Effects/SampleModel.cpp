#include "SampleModel.hpp"

#include "../01-Window/Sample01Window.hpp"
#include "../01-Window/SampleDevice.hpp"
#include "../03-Box/SampleFilesystem.hpp"

#include "Sample04Types.hpp"

#include <MikkTSpace/mikktspace.h>
#include <d3d11.h>

namespace GamesAcademy
{
	template< typename TStorage, size_t TCount >
	struct SampleModelElement
	{
		TStorage			data[ TCount ];
	};

	template< typename TStorage, size_t TCount >
	struct SampleModelChunk
	{
		using Element = SampleModelElement< TStorage, TCount >;

		static const size_t ChunkSize			= 256u;

		SampleModelChunk*	pNext				= nullptr;
		Element				data[ ChunkSize ]	= { };
		size_t				length				= 0u;
	};

	struct SampleModelFace
	{
		size_t				positionIndex;
		size_t				texCoordIndex;
		size_t				normalIndex;
	};

	struct MikkUserData
	{
		SampleModelVertex*	pVertices;
		int					vertexCount;
	};

	using SampleModelVertexPositionChunk = SampleModelChunk< float, 3u >;
	using SampleModelVertexTexCoordChunk = SampleModelChunk< float, 2u >;
	using SampleModelVertexNormalChunk = SampleModelChunk< float, 3u >;
	using SampleModelFaceChunk = SampleModelChunk< SampleModelFace, 1u >;

	namespace SampleModelHelper
	{
		static bool												readLine( char* pLineBuffer, size_t lineBufferSize, size_t& lineLength, const char** ppData, size_t& remainingDataSize );
		static bool												doesStringStartsWith( const char* pString, const char* pSearch );

		template< typename TStorage, size_t TCount >
		static size_t											readElement( const char* pLine, SampleModelChunk< TStorage, TCount >** ppCurrentChunk, bool multiElement );
		template< typename TStorage >
		static const char*										readElementData( TStorage& target, const char* pData );

		template< typename TStorage, size_t TCount >
		static const SampleModelElement< TStorage, TCount >*	findElementForIndex( const SampleModelChunk< TStorage, TCount >* pChunk, size_t index );

		template< typename TChunk >
		static void												freeChunkedData( TChunk* pChunk );

		static bool												calculateTangentFlip( SampleModelVertex* pVertices, size_t vertexCount );

		static int												mikkTSpaceGetNumFaces( const SMikkTSpaceContext* pContext );
		static int												mikkTSpaceGetNumVerticesOfFace( const SMikkTSpaceContext* pContext, const int iFace );
		static void												mikkTSpaceGetPosition( const SMikkTSpaceContext* pContext, float fvPosOut[], const int iFace, const int iVert );
		static void												mikkTSpaceGetNormal( const SMikkTSpaceContext* pContext, float fvNormOut[], const int iFace, const int iVert );
		static void												mikkTSpaceGetTexCoord( const SMikkTSpaceContext* pContext, float fvTexcOut[], const int iFace, const int iVert );
		static void												mikkTSpaceSetTSpaceBasic( const SMikkTSpaceContext* pContext, const float fvTangent[], const float fSign, const int iFace, const int iVert );
	}


	SampleModel::SampleModel()
	{
	}

	SampleModel::~SampleModel()
	{
		destroy();
	}

	bool SampleModel::createFromFile( SampleDevice& device, SampleFilesystem& filesystem, const char* pFilename, bool wrongNormal /*= false*/ )
	{
		const MemoryBlock data = filesystem.readFile( pFilename );
		if( data.pData == nullptr )
		{
			return false;
		}

		bool hasObject = false;

		SampleModelVertexPositionChunk* pPositionChunk		= new SampleModelVertexPositionChunk();
		SampleModelVertexTexCoordChunk* pTexCoordChunk		= new SampleModelVertexTexCoordChunk();
		SampleModelVertexNormalChunk* pNormalChunk			= new SampleModelVertexNormalChunk();
		SampleModelFaceChunk* pFaceChunk					= new SampleModelFaceChunk();

		SampleModelVertexPositionChunk* pFirstPositionChunk	= pPositionChunk;
		SampleModelVertexTexCoordChunk* pFirstTexCoordChunk	= pTexCoordChunk;
		SampleModelVertexNormalChunk* pFirstNormalChunk		= pNormalChunk;
		SampleModelFaceChunk* pFirstFaceChunk				= pFaceChunk;

		char line[ 256u ];
		size_t lineLength = 0u;
		const char* pData = (const char*)data.pData;
		size_t remainingDataSize = data.size;
		size_t maxVertexCountPerFace = 0u;
		while( SampleModelHelper::readLine( line, sizeof( line ), lineLength, &pData, remainingDataSize ) )
		{
			if( line[ 0u ] == '#' ||
				SampleModelHelper::doesStringStartsWith( line, "mtllib" ) )
			{
				continue;
			}

			if( SampleModelHelper::doesStringStartsWith( line, "o " ) )
			{
				if( hasObject )
				{
					// only read the first object
					break;
				}
				hasObject = true;
			}
			else if( SampleModelHelper::doesStringStartsWith( line, "v " ) )
			{
				SampleModelHelper::readElement( line + 2u, &pPositionChunk, false );
			}
			else if( SampleModelHelper::doesStringStartsWith( line, "vt " ) )
			{
				SampleModelHelper::readElement( line + 3u, &pTexCoordChunk, false );
			}
			else if( SampleModelHelper::doesStringStartsWith( line, "vn " ) )
			{
				SampleModelHelper::readElement( line + 3u, &pNormalChunk, false );
			}
			else if( SampleModelHelper::doesStringStartsWith( line, "f " ) )
			{
				maxVertexCountPerFace = max( maxVertexCountPerFace, SampleModelHelper::readElement( line + 2u, &pFaceChunk, true ) );
			}
		}

		m_elementCount	= 0u;
		m_vertexStride	= sizeof( SampleModelVertex );

		for( SampleModelFaceChunk* pCurrentFaceChunk = pFirstFaceChunk; pCurrentFaceChunk != nullptr; pCurrentFaceChunk = pCurrentFaceChunk->pNext )
		{
			m_elementCount += pCurrentFaceChunk->length;
		}

		SampleModelVertex* pVertices = new SampleModelVertex[ m_elementCount ];
		memset( pVertices, 0, sizeof( SampleModelVertex ) * m_elementCount );

		bool error = false;
		SampleModelFaceChunk* pCurrentFaceChunk = pFirstFaceChunk;
		for( size_t vertexIndex = 0u; vertexIndex < m_elementCount; ++vertexIndex )
		{
			const size_t elementIndex = vertexIndex % ARRAY_COUNT( pCurrentFaceChunk->data );
			if( vertexIndex != 0u && elementIndex == 0u )
			{
				pCurrentFaceChunk = pCurrentFaceChunk->pNext;
				if( pCurrentFaceChunk == nullptr )
				{
					error = true;
					break;
				}
			}

			const SampleModelElement< SampleModelFace, 1u >& faces = pCurrentFaceChunk->data[ elementIndex ];
			const SampleModelFace& face = faces.data[ 0u ];

			const SampleModelElement< float, 3u >* pPosition = SampleModelHelper::findElementForIndex( pFirstPositionChunk, face.positionIndex - 1u );
			const SampleModelElement< float, 2u >* pTexCoord = SampleModelHelper::findElementForIndex( pFirstTexCoordChunk, face.texCoordIndex - 1u );
			const SampleModelElement< float, 3u >* pNormal = SampleModelHelper::findElementForIndex( pFirstNormalChunk, face.normalIndex - 1u );
			if( pPosition == nullptr )
			{
				error = true;
				break;
			}

			SampleModelVertex& targetVertex = pVertices[ vertexIndex ];

			memcpy( &targetVertex.postion, pPosition->data, sizeof( pPosition->data ) );

			if( pTexCoord != nullptr )
			{
				memcpy( &targetVertex.texCoord, pTexCoord->data, sizeof( pTexCoord->data ) );
			}

			if( pNormal != nullptr )
			{
				memcpy( &targetVertex.normal, pNormal->data, sizeof( pNormal->data ) );
			}
		}

		SampleModelHelper::freeChunkedData( pFirstPositionChunk );
		SampleModelHelper::freeChunkedData( pFirstTexCoordChunk );
		SampleModelHelper::freeChunkedData( pFirstNormalChunk );
		SampleModelHelper::freeChunkedData( pFirstFaceChunk );

		if( error )
		{
			device.getWindow().showMessageBox( L"Failed to parse OBJ." );
			delete[] pVertices;
			return false;
		}

		if( maxVertexCountPerFace == 3u &&
			!SampleModelHelper::calculateTangentFlip( pVertices, m_elementCount ) )
		{
			device.getWindow().showMessageBox( L"Failed to calculate Tangents." );
			delete[] pVertices;
			return false;
		}

		D3D11_BUFFER_DESC bufferDesc = {};
		bufferDesc.ByteWidth	= UINT( m_elementCount * sizeof( SampleModelVertex ) );
		bufferDesc.Usage		= D3D11_USAGE_IMMUTABLE;
		bufferDesc.BindFlags	= D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA bufferData = {};
		bufferData.pSysMem = pVertices;

		for( size_t vertexIndex = 0u; vertexIndex< m_elementCount; ++vertexIndex )
		{
			SampleModelVertex& targetVertex = pVertices[ vertexIndex ];

			targetVertex.texCoord.y = 1.0f - targetVertex.texCoord.y;

			if( wrongNormal )
			{
				targetVertex.tangentFlip.w *= -1.0f;
			}
		}

		const HRESULT result = device.getDevice()->CreateBuffer( &bufferDesc, &bufferData, &m_pVertexBuffer );
		delete[] pVertices;

		return SUCCEEDED( result );
	}

	bool SampleModel::createFromMemory( SampleDevice& device, const MemoryBlock& vertexData, const MemoryBlock& indexData, size_t elementCount, size_t vertexStride )
	{
		D3D11_BUFFER_DESC bufferDesc = {};
		bufferDesc.ByteWidth	= UINT( vertexData.size );
		bufferDesc.Usage		= D3D11_USAGE_IMMUTABLE;
		bufferDesc.BindFlags	= D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA bufferData ={};
		bufferData.pSysMem = vertexData.pData;

		HRESULT result = device.getDevice()->CreateBuffer( &bufferDesc, &bufferData, &m_pVertexBuffer );
		if( FAILED( result ) )
		{
			device.getWindow().showMessageBox( L"Failed to create Vertex Buffer." );
			return false;
		}

		if( indexData.pData != nullptr )
		{
			D3D11_BUFFER_DESC bufferDesc = {};
			bufferDesc.ByteWidth	= UINT( indexData.size );
			bufferDesc.Usage		= D3D11_USAGE_IMMUTABLE;
			bufferDesc.BindFlags	= D3D11_BIND_INDEX_BUFFER;

			D3D11_SUBRESOURCE_DATA bufferData ={};
			bufferData.pSysMem = indexData.pData;

			HRESULT result = device.getDevice()->CreateBuffer( &bufferDesc, &bufferData, &m_pIndexBuffer );
			if( FAILED( result ) )
			{
				device.getWindow().showMessageBox( L"Failed to create Index Buffer." );
				return false;
			}
		}

		m_elementCount	= elementCount;
		m_vertexStride	= vertexStride;
		return true;
	}

	void SampleModel::destroy()
	{
		if( m_pIndexBuffer != nullptr )
		{
			m_pIndexBuffer->Release();
			m_pIndexBuffer = nullptr;
		}

		if( m_pVertexBuffer != nullptr )
		{
			m_pVertexBuffer->Release();
			m_pVertexBuffer = nullptr;
		}

		m_elementCount	= 0u;
		m_vertexStride	= 0u;
	}

	void SampleModel::render( ID3D11DeviceContext* pContext ) const
	{
		UINT vertexStride = UINT( m_vertexStride );
		UINT vertexOffset = 0u;
		pContext->IASetVertexBuffers( 0u, 1u, &m_pVertexBuffer, &vertexStride, &vertexOffset );

		if( m_pIndexBuffer != nullptr )
		{
			pContext->IASetIndexBuffer( m_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0u );

			pContext->DrawIndexed( UINT( m_elementCount ), 0u, 0 );
		}
		else
		{
			pContext->Draw( UINT( m_elementCount ), 0u );
		}
	}

	static bool SampleModelHelper::readLine( char* pLineBuffer, size_t lineBufferSize, size_t& lineLength, const char** ppData, size_t& remainingDataSize )
	{
		const char* pData = *ppData;

		lineLength = 0u;

		size_t i = 0u;
		for( ; i < remainingDataSize; ++i )
		{
			if( pData[ i ] == '\n' )
			{
				break;
			}

			pLineBuffer[ lineLength++ ] = pData[ i ];
			if( lineLength == lineBufferSize )
			{
				return false;
			}
		}

		remainingDataSize -= i + 1u;
		pData += i + 1u;
		*ppData = pData;

		pLineBuffer[ lineLength ] = '\0';
		return lineLength > 0;
	}

	static bool SampleModelHelper::doesStringStartsWith( const char* pString, const char* pSearch )
	{
		const size_t stringLength = strlen( pString );
		const size_t searchLength = strlen( pSearch );
		if( searchLength > stringLength )
		{
			return false;
		}

		return memcmp( pString, pSearch, searchLength ) == 0;
	}

	template< typename TStorage, size_t TCount >
	static size_t SampleModelHelper::readElement( const char* pLine, SampleModelChunk< TStorage, TCount >** ppCurrentChunk, bool multiElement )
	{
		size_t elementCount = 0u;
		do
		{
			SampleModelChunk< TStorage, TCount >* pCurrentChunk = *ppCurrentChunk;

			if( pCurrentChunk->length == ARRAY_COUNT( pCurrentChunk->data ) )
			{
				SampleModelChunk< TStorage, TCount >* pLastChunk = pCurrentChunk;
				pCurrentChunk = new SampleModelChunk< TStorage, TCount >();
				pLastChunk->pNext = pCurrentChunk;
				*ppCurrentChunk = pCurrentChunk;
			}

			SampleModelElement< TStorage, TCount >& element = pCurrentChunk->data[ pCurrentChunk->length++ ];
			for( size_t dataIndex = 0u; dataIndex < TCount; ++dataIndex )
			{
				while( *pLine == ' ' )
				{
					pLine++;
				}

				pLine = readElementData( element.data[ dataIndex ], pLine );

				elementCount += TCount;
			}
		}
		while( multiElement && *pLine == ' ' );

		return elementCount;
	}

	template<>
	static const char* SampleModelHelper::readElementData( float& target, const char* pData )
	{
		char* pDataEnd = nullptr;
		target = strtof( pData, &pDataEnd );
		return pDataEnd;
	}

	template<>
	static const char* SampleModelHelper::readElementData( SampleModelFace& target, const char* pData )
	{
		char* pDataEnd = nullptr;

		target.positionIndex = strtol( pData, &pDataEnd, 10 );
		pData = pDataEnd;

		if( pData[ 0u ] == '/' && pData[ 1u ] != '/' )
		{
			pData++;
			target.texCoordIndex = strtol( pData, &pDataEnd, 10 );
			pData = pDataEnd;
		}
		else if( pData[ 0u ] != '\0' && pData[ 1u ] == '/' )
		{
			pData++;
		}

		if( *pData == '/' )
		{
			pData++;
			target.normalIndex = strtol( pData, &pDataEnd, 10 );
		}

		return pDataEnd;
	}

	template< typename TStorage, size_t TCount >
	static const SampleModelElement< TStorage, TCount >* SampleModelHelper::findElementForIndex( const SampleModelChunk< TStorage, TCount >* pChunk, size_t index )
	{
		while( pChunk != nullptr && index >= pChunk->length )
		{
			index -= pChunk->length;
			pChunk = pChunk->pNext;
		}

		if( pChunk == nullptr )
		{
			return nullptr;
		}

		return &pChunk->data[ index ];
	}

	template< typename TChunk >
	static void SampleModelHelper::freeChunkedData( TChunk* pChunk )
	{
		while( pChunk != nullptr )
		{
			TChunk* pNext = pChunk->pNext;
			delete pChunk;
			pChunk = pNext;
		}
	}

	static bool SampleModelHelper::calculateTangentFlip( SampleModelVertex* pVertices, size_t vertexCount )
	{
		SMikkTSpaceInterface mikkInterface = {};
		mikkInterface.m_getNumFaces				= mikkTSpaceGetNumFaces;
		mikkInterface.m_getNumVerticesOfFace	= mikkTSpaceGetNumVerticesOfFace;
		mikkInterface.m_getPosition				= mikkTSpaceGetPosition;
		mikkInterface.m_getNormal				= mikkTSpaceGetNormal;
		mikkInterface.m_getTexCoord				= mikkTSpaceGetTexCoord;
		mikkInterface.m_setTSpaceBasic			= mikkTSpaceSetTSpaceBasic;

		MikkUserData mikkUserData = {};
		mikkUserData.pVertices		= pVertices;
		mikkUserData.vertexCount	= int( vertexCount );

		SMikkTSpaceContext mikkContext = {};
		mikkContext.m_pInterface	= &mikkInterface;
		mikkContext.m_pUserData		= &mikkUserData;

		return genTangSpaceDefault( &mikkContext );
	}

	static int SampleModelHelper::mikkTSpaceGetNumFaces( const SMikkTSpaceContext* pContext )
	{
		const MikkUserData& userdata = *(const MikkUserData*)pContext->m_pUserData;
		return userdata.vertexCount / 3u;
	}

	static int SampleModelHelper::mikkTSpaceGetNumVerticesOfFace( const SMikkTSpaceContext* pContext, const int iFace )
	{
		return 3u;
	}

	static void SampleModelHelper::mikkTSpaceGetPosition( const SMikkTSpaceContext* pContext, float fvPosOut[], const int iFace, const int iVert )
	{
		const MikkUserData& userdata = *(const MikkUserData*)pContext->m_pUserData;
		const SampleModelVertex& vertex = userdata.pVertices[ (iFace * 3u) + iVert ];
		fvPosOut[ 0u ] = vertex.postion.x;
		fvPosOut[ 1u ] = vertex.postion.y;
		fvPosOut[ 2u ] = vertex.postion.z;
	}

	static void SampleModelHelper::mikkTSpaceGetNormal( const SMikkTSpaceContext* pContext, float fvNormOut[], const int iFace, const int iVert )
	{
		const MikkUserData& userdata = *(const MikkUserData*)pContext->m_pUserData;
		const SampleModelVertex& vertex = userdata.pVertices[ (iFace * 3u) + iVert ];
		fvNormOut[ 0u ] = vertex.normal.x;
		fvNormOut[ 1u ] = vertex.normal.y;
		fvNormOut[ 2u ] = vertex.normal.z;
	}

	static void SampleModelHelper::mikkTSpaceGetTexCoord( const SMikkTSpaceContext* pContext, float fvTexcOut[], const int iFace, const int iVert )
	{
		const MikkUserData& userdata = *(const MikkUserData*)pContext->m_pUserData;
		const SampleModelVertex& vertex = userdata.pVertices[ (iFace * 3u) + iVert ];
		fvTexcOut[ 0u ] = vertex.texCoord.x;
		fvTexcOut[ 1u ] = vertex.texCoord.y;
	}

	static void SampleModelHelper::mikkTSpaceSetTSpaceBasic( const SMikkTSpaceContext* pContext, const float fvTangent[], const float fSign, const int iFace, const int iVert )
	{
		MikkUserData& userdata = *(MikkUserData*)pContext->m_pUserData;
		SampleModelVertex& vertex = userdata.pVertices[ (iFace * 3u) + iVert ];
		vertex.tangentFlip.x	= fvTangent[ 0u ];
		vertex.tangentFlip.y	= fvTangent[ 1u ];
		vertex.tangentFlip.z	= fvTangent[ 2u ];
		vertex.tangentFlip.w	= fSign;
	}
}