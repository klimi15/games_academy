#include "Sample01Window.hpp"

#include <Windows.h>

int __stdcall WinMain( _In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd )
{
	GamesAcademy::Sample01Window sample;
	return sample.run();
}