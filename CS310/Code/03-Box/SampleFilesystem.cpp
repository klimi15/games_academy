#include "SampleFilesystem.hpp"

#include <Windows.h>
#include <miniz/miniz.h>
#include <wchar.h>

namespace GamesAcademy
{
	struct SampleMiniZ : public mz_zip_archive {};

	struct SampleFileWatch
	{
		HANDLE		handle;
		FILETIME	lastWriteTime;
		char		filename[ 1u ];
	};

	SampleFilesystem::SampleFilesystem()
	{
	}

	SampleFilesystem::~SampleFilesystem()
	{
		destroy();
	}

	bool SampleFilesystem::create()
	{
		const HINSTANCE hInstance = GetModuleHandle( nullptr );

		WCHAR widePath[ MaxPathLength ];
		WCHAR wideSearchPath[ MaxPathLength ];

		GetModuleFileNameW( hInstance, widePath, ARRAY_COUNT( widePath ) );

		bool foundDir = false;
		bool foundZip = false;
		while( !foundDir && !foundZip )
		{
			const WCHAR* pLastSlash = wcsrchr( widePath, L'\\' );
			const size_t pathPartLength = pLastSlash - widePath;

			wcsncpy_s( wideSearchPath, widePath, pathPartLength );
			if( wcslen( wideSearchPath ) <= 3u )
			{
				break;
			}

			wcscpy_s( wideSearchPath + pathPartLength, ARRAY_COUNT( wideSearchPath ) - pathPartLength, L"\\Content.pak" );
			HANDLE fileHandle = CreateFileW( wideSearchPath, 0u, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr );
			if( fileHandle != INVALID_HANDLE_VALUE )
			{
				BY_HANDLE_FILE_INFORMATION information;
				foundZip |= GetFileInformationByHandle( fileHandle, &information ) ? true : false;
			}

			wcscpy_s( wideSearchPath + pathPartLength, ARRAY_COUNT( wideSearchPath ) - pathPartLength, L"\\Content" );
			fileHandle = CreateFileW( wideSearchPath, 0u, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_BACKUP_SEMANTICS, nullptr );
			if( fileHandle != INVALID_HANDLE_VALUE )
			{
				BY_HANDLE_FILE_INFORMATION information;
				foundDir |= GetFileInformationByHandle( fileHandle, &information ) ? true : false;
				CloseHandle( fileHandle );
			}

			wcsncpy_s( widePath, wideSearchPath, pathPartLength );
		}

		WideCharToMultiByte( CP_UTF8, 0, widePath, -1, m_basePath, ARRAY_COUNT( m_basePath ), nullptr, nullptr );
		strcat_s( m_basePath, "\\" );

		if( foundDir )
		{
			return true;
		}

		if( foundZip )
		{
			m_archiveData = readFile( "Content.pak" );
			if( m_archiveData.pData == nullptr )
			{
				destroy();
				return false;
			}

			// super secret package format
			uint8* pArchiveData = (uint8*)m_archiveData.pData;
			if( pArchiveData[ 0u ] == 'T' &&
				pArchiveData[ 1u ] == 'B' )
			{
				pArchiveData[ 0u ] = 'P';
				pArchiveData[ 1u ] = 'K';

				for( size_t i = 2u; i < m_archiveData.size; ++i )
				{
					pArchiveData[ i ] ^= 0xbbu;
				}
			}

			m_pZip = new SampleMiniZ();
			mz_zip_zero_struct( m_pZip );
			if( !mz_zip_reader_init_mem( m_pZip, m_archiveData.pData, m_archiveData.size, 0u ) )
			{
				destroy();
				return false;
			}

			return true;
		}

		destroy();
		return false;
	}

	void SampleFilesystem::destroy()
	{
		if( m_pZip != nullptr )
		{
			mz_zip_reader_end( m_pZip );
			delete m_pZip;
			m_pZip = nullptr;
		}

		freeFile( m_archiveData );
		m_archiveData = { nullptr, 0u };

		m_basePath[ 0u ] = '\0';
	}

	bool SampleFilesystem::doesFileExist( const char* pFilename )
	{
		if( m_pZip != nullptr )
		{
			mz_uint fileIndex = 0u;
			return mz_zip_reader_locate_file_v2( m_pZip, pFilename, nullptr, 0u, &fileIndex ) != -1;
		}

		char filenameBuffer[ MaxPathLength ];
		sprintf_s( filenameBuffer, sizeof( filenameBuffer ), "%s%s", m_basePath, pFilename );

		WCHAR wideFilenameBuffer[ MaxPathLength ];
		MultiByteToWideChar( CP_UTF8, 0, filenameBuffer, -1, wideFilenameBuffer, ARRAY_COUNT( wideFilenameBuffer ) );

		const HANDLE fileHandle = CreateFileW( wideFilenameBuffer, 0u, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr );
		if( fileHandle == INVALID_HANDLE_VALUE )
		{
			return false;
		}

		BY_HANDLE_FILE_INFORMATION information;
		const bool doesExist = GetFileInformationByHandle( fileHandle, &information ) ? true : false;
		CloseHandle( fileHandle );
		return doesExist;
	}

	MemoryBlock SampleFilesystem::readFile( const char* pFilename )
	{
		if( m_pZip != nullptr )
		{
			mz_uint fileIndex = 0u;
			if( mz_zip_reader_locate_file_v2( m_pZip, pFilename, nullptr, 0u, &fileIndex ) == -1 )
			{
				return { nullptr, 0u };
			}

			mz_zip_archive_file_stat fileStats;
			if( !mz_zip_reader_file_stat( m_pZip, fileIndex, &fileStats ) )
			{
				return { nullptr, 0u };
			}

			void* pData = malloc( (size_t)fileStats.m_uncomp_size );
			if( !mz_zip_reader_extract_to_mem_no_alloc( m_pZip, fileIndex, pData, (size_t)fileStats.m_uncomp_size, 0u, nullptr, 0u ) )
			{
				free( pData );
				return { nullptr, 0u };
			}

			return { pData, (size_t)fileStats.m_uncomp_size };
		}

		char filenameBuffer[ MaxPathLength ];
		sprintf_s( filenameBuffer, sizeof( filenameBuffer ), "%s%s", m_basePath, pFilename );

		WCHAR wideFilenameBuffer[ MaxPathLength ];
		MultiByteToWideChar( CP_UTF8, 0, filenameBuffer, -1, wideFilenameBuffer, ARRAY_COUNT( wideFilenameBuffer ) );

		HANDLE fileHandle = CreateFileW( wideFilenameBuffer, FILE_GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0u, nullptr );
		if( fileHandle == INVALID_HANDLE_VALUE )
		{
			if( GetLastError() == ERROR_SHARING_VIOLATION )
			{
				Sleep( 50u );
				fileHandle = CreateFileW( wideFilenameBuffer, FILE_GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0u, nullptr );
			}

			if( fileHandle == INVALID_HANDLE_VALUE )
			{
				return { nullptr, 0u };
			}
		}

		const DWORD fileSize = GetFileSize( fileHandle, nullptr );

		void* pData = malloc( fileSize );

		DWORD bytesRead;
		if( !ReadFile( fileHandle, pData, fileSize, &bytesRead, nullptr ) ||
			bytesRead != fileSize )
		{
			free( pData );
			CloseHandle( fileHandle );
			return { nullptr, 0u };
		}

		CloseHandle( fileHandle );
		return { pData, fileSize };
	}

	void SampleFilesystem::freeFile( MemoryBlock data )
	{
		if( data.pData == nullptr )
		{
			return;
		}

		free( (void*)data.pData );
	}

	SampleFileWatch* SampleFilesystem::startWatchFile( const char* pFilename )
	{
		char filenameBuffer[ MaxPathLength ];
		sprintf_s( filenameBuffer, sizeof( filenameBuffer ), "%s%s", m_basePath, pFilename );

		WCHAR wideFilenameBuffer[ MaxPathLength ];
		MultiByteToWideChar( CP_UTF8, 0, filenameBuffer, -1, wideFilenameBuffer, ARRAY_COUNT( wideFilenameBuffer ) );

		const HANDLE handle = CreateFileW( wideFilenameBuffer, FILE_GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, 0u, nullptr );
		if( handle == INVALID_HANDLE_VALUE )
		{
			return nullptr;
		}

		const size_t filenameLength = strlen( pFilename );

		BY_HANDLE_FILE_INFORMATION information;
		GetFileInformationByHandle( handle, &information );

		SampleFileWatch* pWatch = (SampleFileWatch*)malloc( sizeof( SampleFileWatch ) + filenameLength );
		pWatch->handle			= handle;
		pWatch->lastWriteTime	= information.ftLastWriteTime;

		strcpy_s( pWatch->filename, filenameLength + 1u, pFilename );

		return pWatch;
	}

	bool SampleFilesystem::hasWatchedFileChanged( SampleFileWatch* pWatch )
	{
		if( pWatch == nullptr )
		{
			return false;
		}

		BY_HANDLE_FILE_INFORMATION information;
		GetFileInformationByHandle( pWatch->handle, &information );

		const bool result = memcmp( &information.ftLastWriteTime, &pWatch->lastWriteTime, sizeof( FILETIME ) ) != 0;
		pWatch->lastWriteTime = information.ftLastWriteTime;

		return result;
	}

	const char* SampleFilesystem::getWatchedFileName( SampleFileWatch* pWatch )
	{
		if( pWatch == nullptr )
		{
			return nullptr;
		}

		return pWatch->filename;
	}

	void SampleFilesystem::stopWatchFile( SampleFileWatch* pWatch )
	{
		CloseHandle( pWatch->handle );
		delete pWatch;
	}
}
