#pragma once

#include "../01-Window/SampleTypes.hpp"

namespace GamesAcademy
{
	struct SampleMiniZ;
	struct SampleFileWatch;

	class SampleFilesystem
	{
	public:

								SampleFilesystem();
								~SampleFilesystem();

		bool					create();
		void					destroy();

		bool					doesFileExist( const char* pFilename );

		MemoryBlock				readFile( const char* pFilename );
		void					freeFile( MemoryBlock data );

		SampleFileWatch*		startWatchFile( const char* pFilename );
		bool					hasWatchedFileChanged( SampleFileWatch* pWatch );
		const char*				getWatchedFileName( SampleFileWatch* pWatch );
		void					stopWatchFile( SampleFileWatch* pWatch );

	private:

		static const size_t		MaxPathLength				= 256u;

		char					m_basePath[ MaxPathLength ]	= { '\0' };

		MemoryBlock				m_archiveData				= { nullptr, 0u };
		SampleMiniZ*			m_pZip						= nullptr;
	};
}
