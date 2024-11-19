#include "Tetris/TetrisGame.h"

/*
	A simple tetris game, that uses GLFW for user input/window creation and modern OpenGL for rendering
*/

#include <vector>
#include <array>


int main()
{
	/*NOTES... Having the concept of an "active block" is LITERALLY UNNECCESSARY AND IT ONLY COMPLICATES THINGS. A LOT.
	Every block that's in a shape should be considered active. It also makes it hard to change colors per shape, so I just gave up on that(It'd be easy if the grid was done differently)
	This is just pretty much a prototype, trying out stuff and getting a basic idea of how the game would work if I decided to actually make it.
	*/

	Debug::InitConsole();
	srand((unsigned int)time(NULL));

	/**/
	bool runAgain = true;
	while (runAgain)
	{
		TetrisGame tetris;
		if (tetris.initGame())
		{
			if (tetris.run() != TetrisGame::STATUS::RERUN)
				runAgain = false;
		}
	}
		

	Debug::StopAndFreeConsole();
}