#include "Game.h"

#include <windows.h>

int WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd )
{
	GamesAcademy::Game game;
	return game.run();
}
