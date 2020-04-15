#include "sound.hpp"

#include <windows.h>

namespace GamesAcademy
{
	static constexpr uint32_t SoundWaveFourccRIFF	= 'FFIR';
	static constexpr uint32_t SoundWaveFourccDATA	= 'atad';
	static constexpr uint32_t SoundWaveFourccFMT	= ' tmf';
	static constexpr uint32_t SoundWaveFourccWAVE	= 'EVAW';
	static constexpr uint32_t SoundWaveFourccXWMA	= 'AMWX';
	static constexpr uint32_t SoundWaveFourccDPDS	= 'sdpd';

	struct SoundFile
	{
		WAVEFORMATEXTENSIBLE	format;
		size_t					dataSize;
		uint8_t					data[ 1u ];
	};

	struct SoundInstance
	{
		SoundInstance*			pNext	= nullptr;
		SoundFile*				pFile	= nullptr;
		IXAudio2SourceVoice*	pVoice	= nullptr;
	};

	bool Sound::create()
	{
		HRESULT result = CoInitializeEx( nullptr, COINIT_MULTITHREADED );
		if( FAILED( result ) )
		{
			MessageBoxW( nullptr, L"Failed to init COM.", L"Framework", MB_ICONSTOP );
			return false;
		}

		UINT32 flags = 0;
		result = XAudio2Create( &m_pXAudio2, flags );
		if( FAILED( result ) )
		{
			MessageBoxW( nullptr, L"Failed to init XAudio2 engine.", L"Framework", MB_ICONSTOP );
			destroy();
			return false;
		}

#if defined( _DEBUG )
		XAUDIO2_DEBUG_CONFIGURATION debug = {};
		debug.TraceMask = XAUDIO2_LOG_ERRORS | XAUDIO2_LOG_WARNINGS;
		debug.BreakMask = XAUDIO2_LOG_ERRORS;
		m_pXAudio2->SetDebugConfiguration( &debug, 0 );
#endif

		result = m_pXAudio2->CreateMasteringVoice( &m_pMasteringVoice );
		if( FAILED( result ) )
		{
			MessageBoxW( nullptr, L"Failed creating mastering voice.", L"Framework", MB_ICONSTOP );
			destroy();
			return false;
		}

		return true;
	}

	void Sound::destroy()
	{
		SoundInstance* pInstance = m_pFirstInstance;
		while( pInstance != nullptr )
		{
			SoundInstance* pNextInstance = pInstance->pNext;

			pInstance->pVoice->DestroyVoice();
			delete pInstance;

			pInstance = pNextInstance;
		}
		m_pFirstInstance = nullptr;

		if( m_pMasteringVoice != nullptr )
		{
			m_pMasteringVoice->DestroyVoice();
			m_pMasteringVoice = nullptr;
		}

		if( m_pXAudio2 != nullptr )
		{
			m_pXAudio2->Release();
			m_pXAudio2 = nullptr;
		}

		CoUninitialize();
	}

	void* Sound::loadFile( const char* pFilename )
	{
		FILE* pFile;
		if( fopen_s( &pFile, pFilename, "rb" ) != 0 )
		{
			return nullptr;
		}

		size_t chunkDataSize;
		size_t chunkDataPosition;
		uint32_t fileType;
		if( !findChunk( pFile, SoundWaveFourccRIFF, chunkDataSize, chunkDataPosition ) ||
			!readChunk( pFile, &fileType, sizeof( fileType ), chunkDataPosition ) )
		{
			MessageBoxW( nullptr, L"Failed to load RIFF chunk from WAV file.", L"Framework", MB_ICONSTOP );
			fclose( pFile );
			return nullptr;
		}

		if( fileType != SoundWaveFourccWAVE )
		{
			MessageBoxW( nullptr, L"Unsupported WAV file format.", L"Framework", MB_ICONSTOP );
			fclose( pFile );
			return nullptr;
		}

		WAVEFORMATEXTENSIBLE format;
		if( !findChunk(pFile, SoundWaveFourccFMT, chunkDataSize, chunkDataPosition ) ||
			!readChunk( pFile, &format, sizeof( format ), chunkDataPosition ) )
		{
			MessageBoxW( nullptr, L"Failed to load Format from WAV file.", L"Framework", MB_ICONSTOP );
			fclose( pFile );
			return nullptr;
		}

		if( !findChunk( pFile, SoundWaveFourccDATA, chunkDataSize, chunkDataPosition ) )
		{
			MessageBoxW( nullptr, L"Failed to find Data in WAV file.", L"Framework", MB_ICONSTOP );
			fclose( pFile );
			return nullptr;
		}

		SoundFile* pSoundFile = (SoundFile*)malloc( sizeof( SoundFile ) + chunkDataSize );
		pSoundFile->format		= format;
		pSoundFile->dataSize	= chunkDataSize;

		if( !readChunk( pFile, pSoundFile->data, chunkDataSize, chunkDataPosition ) )
		{
			MessageBoxW( nullptr, L"Failed to load Data from WAV file.", L"Framework", MB_ICONSTOP );
			fclose( pFile );
			return nullptr;
		}

		return pSoundFile;
	}

	void Sound::freeFile( void* pFile )
	{
		free( pFile );
	}

	void Sound::play( void* pFile )
	{
		if( pFile == nullptr )
		{
			return;
		}

		SoundInstance* pInstance = new SoundInstance();
		pInstance->pFile = (SoundFile*)pFile;

		HRESULT result = m_pXAudio2->CreateSourceVoice( &pInstance->pVoice, (WAVEFORMATEX*)&pInstance->pFile->format );
		if( FAILED( result ) )
		{
			MessageBoxW( nullptr, L"Failed to create Voice.", L"Framework", MB_ICONSTOP );
			delete pInstance;
			return;
		}

		XAUDIO2_BUFFER buffer = {};
		buffer.AudioBytes	= (UINT32)pInstance->pFile->dataSize;
		buffer.pAudioData	= pInstance->pFile->data;
		buffer.Flags		= XAUDIO2_END_OF_STREAM;

		result = pInstance->pVoice->SubmitSourceBuffer( &buffer );
		if( FAILED( result ) )
		{
			MessageBoxW( nullptr, L"Failed to submit Sound Buffer.", L"Framework", MB_ICONSTOP );
			pInstance->pVoice->DestroyVoice();
			delete pInstance;
			return;
		}

		result = pInstance->pVoice->Start();
		if( FAILED( result ) )
		{
			MessageBoxW( nullptr, L"Failed to start Sound.", L"Framework", MB_ICONSTOP );
			pInstance->pVoice->DestroyVoice();
			delete pInstance;
			return;
		}

		pInstance->pNext = m_pFirstInstance;
		m_pFirstInstance = pInstance;
	}

	bool Sound::findChunk( FILE* pFile, uint32_t fourcc, size_t& chunkDataSize, size_t& chunkDataPosition )
	{
		if( fseek( pFile, 0u, SEEK_SET ) != 0 )
		{
			return false;
		}

		uint32_t currentChunkType;
		uint32_t currentChunkDataSize;
		uint32_t offset = 0;
		while( true )
		{
			if( fread( &currentChunkType, sizeof( currentChunkType ), 1u, pFile ) != 1 )
			{
				return false;
			}

			if( fread( &currentChunkDataSize, sizeof( currentChunkDataSize ), 1u, pFile ) != 1 )
			{
				return false;
			}

			switch( currentChunkType )
			{
			case SoundWaveFourccRIFF:
				{
					currentChunkDataSize = 4;

					uint32_t fileType;
					if( fread( &fileType, sizeof( fileType ), 1u, pFile ) != 1 )
					{
						return false;
					}
				}
				break;

			default:
				{
					if( fseek( pFile, currentChunkDataSize, SEEK_CUR ) != 0 )
					{
						return false;
					}
				}
				break;
			}

			offset += sizeof( uint32_t ) * 2u;

			if( currentChunkType == fourcc )
			{
				chunkDataSize		= currentChunkDataSize;
				chunkDataPosition	= offset;
				return true;
			}

			offset += currentChunkDataSize;
		}

		return false;
	}

	bool Sound::readChunk( FILE* pFile, void* pBuffer, size_t bufferSize, size_t chunkDataPosition )
	{
		if( fseek( pFile, (long)chunkDataPosition, SEEK_SET ) != 0 )
		{
			return false;
		}

		return fread( pBuffer, bufferSize, 1u, pFile ) == 1;
	}

}
