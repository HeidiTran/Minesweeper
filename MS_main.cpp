#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include "MS_class.h"

using namespace std;

const std::string WINDOW_TITLE = "[MINESWEEPER] - 2.1";
const int SCREEN_WIDTH = 1000;
const int SCREEN_HEIGHT = 600;

int main(int argc, char * argv[])
{
	MineSweeper game(SCREEN_WIDTH, SCREEN_HEIGHT, WINDOW_TITLE);
	string level = game.chooseLevel();
	if (!level.empty())
	{
		game.newGame(level);
		game.play();
	}
	game.exit();

	return 0;
}








