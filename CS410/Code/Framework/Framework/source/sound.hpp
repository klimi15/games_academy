#pragma once

#include <cstdint>
#include <cstdio>
#include <xaudio2.h>

namespace GamesAcademy
{
	struct SoundInstance;

	class Sound
	{
	public:

		bool					create();
		void					destroy();

		void*					loadFile( const char* pFilename );
		void					freeFile( void* pFile );

		void*					play( void* pFile );
		void					stop( void* pVoice );
		bool					isPlaying( void* pVoice );

	private:

		IXAudio2*				m_pXAudio2			= nullptr;
		IXAudio2MasteringVoice*	m_pMasteringVoice	= nullptr;

		SoundInstance*			m_pFirstInstance	= nullptr;

		bool					findChunk( FILE* pFile, uint32_t fourcc, size_t& chunkDataSize, size_t& chunkDataPosition );
		bool					readChunk( FILE* pFile, void* pBuffer, size_t bufferSize, size_t chunkDataPosition );
	};
}