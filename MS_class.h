#pragma once
#include <vector>
#include <iomanip>
#include "SDL_Utensils.h"
#include "MS_Recources.h"

struct Cell
{
	Cell(int row, int col);
	int r;
	int c;
};

class MineSweeper
{
public:
	MineSweeper(int screenWidth, int screenHeight, const std::string& windowTitle);

	std::string chooseLevel();

	void newGame(const std::string& level);
	
	bool isInBoard(const SDL_Event & e) const;

	int getRow(const SDL_Event& e) { return (getMouseY(e) - board_y) / box_length; }

	int getCol(const SDL_Event& e) { return (getMouseX(e) - board_x) / box_length; }

	bool isRightClick(const SDL_Event & e) const { return e.button.button == SDL_BUTTON_RIGHT; }

	bool isMine(int r, int c) const { return finalState[r][c] == MINE; }

	bool isBlank(int r, int c) const { return finalState[r][c] == BLANK; }

	void open(int r, int c);

	void processRightClick(int r, int c);

	void processBlankCell(int row, int col);

	void render() ;

	void play();

	bool allNoMineCellOpened() { return curCorrectBoxes == board_size * board_size - nMine; }

	void win();

	void lose();

	bool isReplay(const SDL_Event& e);

	void exit();

private:
	std::vector<SDL_Texture*> loadImage();
	void calHints();
	bool isClosed(int r, int c) { return curState[r][c] == CLOSED; }
	bool isValidCell(int row, int col);
	void openAllBoxes();
	
private:
	SDL_Window * window;
	SDL_Renderer* renderer;
	int screenWidth, screenHeight;
	std::vector<SDL_Texture*> image;

	int board_x, board_y, board_size, box_length;
	std::vector<std::vector<int> > finalState;
	std::vector<std::vector<int> > curState;
	int nMine, curCorrectBoxes;
	double mineRate;
	std::string level;

	int REPLAY_X, REPLAY_Y, REPLAY_W, REPLAY_H;
	int EASY_X, EASY_Y, MED_X, MED_Y, HARD_X, HARD_Y;
};




