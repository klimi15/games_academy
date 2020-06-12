#include "Sample02Triangle.hpp"

#include <Windows.h>

int __stdcall WinMain( _In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd )
{
	GamesAcademy::Sample02Triangle sample;
	return sample.run();
}