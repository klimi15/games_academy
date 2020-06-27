#include "server.h"

#include <unistd.h>

int main(int argc, char *argv[])
{
	GamesAcademy::Server server;
	server.create();

	while( true )
	{
		server.update();
		usleep( 30 * 1000 );
	}

	server.destroy();
	return 0;
}