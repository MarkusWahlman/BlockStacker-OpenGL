#include "debug.h"

void Debug::InitConsole()
{
#ifdef _DEBUG
	if (AllocConsole())
	{
		/*Redirect std output to new console*/
		freopen("CONIN$", "r", stdin);
		freopen("CONOUT$", "w", stdout);
		freopen("CONOUT$", "w", stderr);
	}
#endif
}

void Debug::StopAndFreeConsole()
{
#ifdef _DEBUG
	char c;
	std::cin >> c;
	FreeConsole();
#endif
}

void Debug::Message()
{
#ifdef _DEBUG
	/*This will be called after last log call, add a new line*/
	std::cout << '\n';
#endif
}
