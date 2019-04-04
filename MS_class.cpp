#include "MS_class.h"
#include <queue>
#include <stack>

using namespace std;

const int LEVEL_IMG_LENGTH = 100;
const int SPACE_BETWN_LEVEL_IMG = 50;
const int SLCT_LEVEL_IMG_W = 300;
const int SLCT_LEVEL_IMG_H = 100;

const int EASY_BOARD_SIZE = 5;
const int MED_BOARD_SIZE = 9;
const int HARD_BOARD_SIZE = 16;

const int EASY_BOX_LENGTH = 80;
const int MED_BOX_LENGTH = 50;
const int HARD_BOX_LENGTH = 35;

const double EASY_MINE_RATE = 0.15;
const double MED_MINE_RATE = 0.15;
const double HARD_MINE_RATE = 0.15;


MineSweeper::MineSweeper(int screenWidth, int screenHeight, const std::string& windowTitle)
{
	this->screenWidth = screenWidth;
	this->screenHeight = screenHeight;

	initSDL(window, renderer, screenWidth, screenHeight, windowTitle.c_str());
	
	image = loadImage();
}

string MineSweeper::chooseLevel()
{
	const int LEVEL_X = (screenWidth - LEVEL_IMG_LENGTH) / 2;
	const int LEVEL_MED_Y = (screenHeight - LEVEL_IMG_LENGTH) / 2 + 75;
	const int LEVEL_EASY_Y = LEVEL_MED_Y - (LEVEL_IMG_LENGTH + SPACE_BETWN_LEVEL_IMG);
	const int LEVEL_HARD_Y = LEVEL_MED_Y + (LEVEL_IMG_LENGTH + SPACE_BETWN_LEVEL_IMG);
	const int SLCT_LEVEL_X = (screenWidth - SLCT_LEVEL_IMG_W) / 2;
	const int SLCT_LEVEL_Y = LEVEL_EASY_Y - SLCT_LEVEL_IMG_H - SPACE_BETWN_LEVEL_IMG;

	SDL_SetRenderDrawColor(renderer, 229, 204, 255, 0);   // lilac background
	SDL_RenderClear(renderer);
	renderTexture(image[SLCT_LEVEL], renderer, SLCT_LEVEL_X, SLCT_LEVEL_Y, SLCT_LEVEL_IMG_W, SLCT_LEVEL_IMG_H);
	renderTexture(image[EASY], renderer, LEVEL_X, LEVEL_EASY_Y, LEVEL_IMG_LENGTH, LEVEL_IMG_LENGTH);
	renderTexture(image[MED], renderer, LEVEL_X, LEVEL_MED_Y, LEVEL_IMG_LENGTH, LEVEL_IMG_LENGTH);
	renderTexture(image[HARD], renderer, LEVEL_X, LEVEL_HARD_Y, LEVEL_IMG_LENGTH, LEVEL_IMG_LENGTH);
	SDL_RenderPresent(renderer);

	SDL_Event e;

	while (true)
	{
		SDL_Delay(10);
		if (SDL_WaitEvent(&e) == 0) continue;
		if (e.type == SDL_QUIT) return "";
		if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) return "";

		if (e.type == SDL_MOUSEBUTTONDOWN)
		{
			int curX = getMouseX(e);
			int curY = getMouseY(e);

			if (curX > LEVEL_X && (curX < LEVEL_X + LEVEL_IMG_LENGTH) && curY > LEVEL_EASY_Y && (curY < LEVEL_EASY_Y + LEVEL_IMG_LENGTH))
				return "easy";
			else if (curX > LEVEL_X && (curX < LEVEL_X + LEVEL_IMG_LENGTH) && curY > LEVEL_MED_Y && (curY < LEVEL_MED_Y + LEVEL_IMG_LENGTH))
				return "med";
			else if (curX > LEVEL_X && (curX < LEVEL_X + LEVEL_IMG_LENGTH) && curY > LEVEL_HARD_Y && (curY < LEVEL_HARD_Y + LEVEL_IMG_LENGTH))
				return "hard";
		}
	}
}

void MineSweeper::newGame(const string& level)
{
	this->level = level;
	if (level == "easy")
	{
		board_size = EASY_BOARD_SIZE;
		box_length = EASY_BOX_LENGTH;
		mineRate = EASY_MINE_RATE;
	}
	else if (level == "med")
	{
		board_size = MED_BOARD_SIZE;
		box_length = MED_BOX_LENGTH;
		mineRate = MED_MINE_RATE;
	}
	else if (level == "hard")
	{
		board_size = HARD_BOARD_SIZE;
		box_length = HARD_BOX_LENGTH;
		mineRate = HARD_MINE_RATE;
	}			

	board_x = (screenWidth - box_length * board_size) / 2;
	board_y = (screenHeight - box_length * board_size) / 2;

	// FASTER WAY vector<vector<int> > finalState(board_size, vector<int>(board_size))
	// IN GENERAL WITH 2D VECTOR m x n: vector<vector<int> > finalState(m, vector<int>(n, 0)) with 0 is the initialized value for all elements
	// Initialize all elements in the finalState matrix
	finalState.clear();
	if (finalState.size() == 0)
	{
		vector<int> BlankColum;
		for (int i = 0; i < board_size; i++)
			BlankColum.push_back(BLANK);
		for (int i = 0; i < board_size; i++)
			finalState.push_back(BlankColum);
	}

	// Initialize all elements in the state matrix
	curState.clear();
	if (curState.size() == 0)
	{
		vector<int> ClosedColum;
		for (int i = 0; i < board_size; i++)
			ClosedColum.push_back(CLOSED);
		for (int i = 0; i < board_size; i++)
			curState.push_back(ClosedColum);
	}
	
	// Set up mines				// COUNT VARIABLE -> EXACT NUMBER OF MINE WANTED
	const int maxNumMine = mineRate * board_size * board_size;						

	// Random mines' locations
	srand(time(0));
	for (int i = 0; i < maxNumMine; i++)
		finalState[rand() % board_size][rand() % board_size] = MINE;

	nMine = 0;
	curCorrectBoxes = 0;
	// Calculate the actual number of mines in the game
	for (int i = 0; i < finalState.size(); i++)
		for (int j = 0; j < finalState.size(); j++)
			if (finalState[i][j] == MINE)
				nMine++;

	// Calculate hints in the finalState matrix
	calHints();
	
	// Render Game
	render();
}

bool MineSweeper::isInBoard (const SDL_Event & e) const
{
	int curX = getMouseX(e);
	int curY = getMouseY(e);

	if (curX >= board_x && curX <= board_x + board_size * box_length &&
		curY >= board_y && curY <= board_y + board_size * box_length)
		return true;
	else
		return false;
}

void MineSweeper::open(int row, int col)
{
	curState[row][col] = finalState[row][col];
	if (!isMine(row, col))
		curCorrectBoxes++;
}

void MineSweeper::processRightClick(int r, int c)
{
	// Flag event
	if (curState[r][c] == CLOSED)
		curState[r][c] = OPEN_FLAG;

	// Question mark event
	else if (curState[r][c] == OPEN_FLAG)
		curState[r][c] = OPEN_QUEST;

	// Back to closed box event
	else if (curState[r][c] == OPEN_QUEST)
		curState[r][c] = CLOSED;
}

// Write processBlankCell(int row, int col) using stack<pair<int, int> > newBlankCell AND NO NEED TO USE STRUCT CELL
void MineSweeper::processBlankCell(int row, int col)
{
	stack<pair<int, int> > newBlankCell;
	newBlankCell.push(make_pair(row, col));

	while (!newBlankCell.empty())
	{
		pair<int, int> tmpCell = newBlankCell.top();
		newBlankCell.pop();

		int r = tmpCell.first;		
		int c = tmpCell.second;

		for (int i = r - 1; i <= r + 1; i++)
			for (int j = c - 1; j <= c + 1; j++)
				if (isValidCell(i, j) && isClosed(i, j))
				{
					open(i, j);
					if (isBlank(i, j))
						newBlankCell.push(make_pair(i, j));
				}
		
		
	}
}

// Write processBlankCell(int row, int col) using queue<Cell> newBlankCell
/*
void MineSweeper::processBlankCell(int row, int col)
{
	queue<Cell> newBlankCell;
	Cell firstBlankCell(row, col);
	newBlankCell.push(firstBlankCell);

	while (!newBlankCell.empty())
	{
		Cell theCell = newBlankCell.front();
	newBlankCell.pop();

if (isValidCell(theCell.r + 1, theCell.c) && isClosed(theCell.r + 1, theCell.c))
{
Cell cell(theCell.r + 1, theCell.c);
open(cell.r, cell.c);			// (2, 1)
if (isBlank(cell.r, cell.c))
newBlankCell.push(cell);
}

if (isValidCell(theCell.r, theCell.c + 1) && isClosed(theCell.r, theCell.c + 1))
{
Cell cell(theCell.r, theCell.c + 1);
open(cell.r, cell.c);		// (1, 2)
if (isBlank(cell.r, cell.c))
newBlankCell.push(cell);
}

if (isValidCell(theCell.r + 1, theCell.c + 1) && isClosed(theCell.r + 1, theCell.c + 1))
{
Cell cell(theCell.r + 1, theCell.c + 1);
open(cell.r, cell.c);				// (2, 2)
if (isBlank(cell.r, cell.c))
newBlankCell.push(cell);
}

if (isValidCell(theCell.r - 1, theCell.c) && isClosed(theCell.r - 1, theCell.c))
{
Cell cell(theCell.r - 1, theCell.c);
open(cell.r, cell.c);		// (0, 1)
if (isBlank(cell.r, cell.c))
newBlankCell.push(cell);
}

if (isValidCell(theCell.r, theCell.c - 1) && isClosed(theCell.r, theCell.c - 1))
{
Cell cell(theCell.r, theCell.c - 1);
open(cell.r, cell.c);			// (1, 0)
if (isBlank(cell.r, cell.c))
newBlankCell.push(cell);
}

if (isValidCell(theCell.r - 1, theCell.c - 1) && isClosed(theCell.r - 1, theCell.c - 1))
{
Cell cell(theCell.r - 1, theCell.c - 1);
open(cell.r, cell.c);			// (0, 0)
if (isBlank(cell.r, cell.c))
newBlankCell.push(cell);
}

if (isValidCell(theCell.r + 1, theCell.c - 1) && isClosed(theCell.r + 1, theCell.c - 1))
{
Cell cell(theCell.r + 1, theCell.c - 1);
open(cell.r, cell.c);				// (2, 0)
if (isBlank(cell.r, cell.c))
newBlankCell.push(cell);
}

if (isValidCell(theCell.r - 1, theCell.c + 1) && isClosed(theCell.r - 1, theCell.c + 1))
{
Cell cell(theCell.r - 1, theCell.c + 1);
open(cell.r, cell.c);			// (0, 2)
if (isBlank(cell.r, cell.c))
newBlankCell.push(cell);
}
}
}
*/

// Write processBlankCell(int row, int col) using stack<Cell> newBlankCell
/*
void MineSweeper::processBlankCell(int row, int col)
{
stack<Cell> newBlankCell;
Cell firstBlankCell(row, col);
newBlankCell.push(firstBlankCell);

while (!newBlankCell.empty())
{
Cell theCell = newBlankCell.top();
newBlankCell.pop();

if (isValidCell(theCell.r + 1, theCell.c) && isClosed(theCell.r + 1, theCell.c))
{
Cell cell(theCell.r + 1, theCell.c);
open(cell.r, cell.c);			// (2, 1)
if (isBlank(cell.r, cell.c))
newBlankCell.push(cell);
}

if (isValidCell(theCell.r, theCell.c + 1) && isClosed(theCell.r, theCell.c + 1))
{
Cell cell(theCell.r, theCell.c + 1);
open(cell.r, cell.c);		// (1, 2)
if (isBlank(cell.r, cell.c))
newBlankCell.push(cell);
}

if (isValidCell(theCell.r + 1, theCell.c + 1) && isClosed(theCell.r + 1, theCell.c + 1))
{
Cell cell(theCell.r + 1, theCell.c + 1);
open(cell.r, cell.c);				// (2, 2)
if (isBlank(cell.r, cell.c))
newBlankCell.push(cell);
}

if (isValidCell(theCell.r - 1, theCell.c) && isClosed(theCell.r - 1, theCell.c))
{
Cell cell(theCell.r - 1, theCell.c);
open(cell.r, cell.c);		// (0, 1)
if (isBlank(cell.r, cell.c))
newBlankCell.push(cell);
}

if (isValidCell(theCell.r, theCell.c - 1) && isClosed(theCell.r, theCell.c - 1))
{
Cell cell(theCell.r, theCell.c - 1);
open(cell.r, cell.c);			// (1, 0)
if (isBlank(cell.r, cell.c))
newBlankCell.push(cell);
}

if (isValidCell(theCell.r - 1, theCell.c - 1) && isClosed(theCell.r - 1, theCell.c - 1))
{
Cell cell(theCell.r - 1, theCell.c - 1);
open(cell.r, cell.c);			// (0, 0)
if (isBlank(cell.r, cell.c))
newBlankCell.push(cell);
}

if (isValidCell(theCell.r + 1, theCell.c - 1) && isClosed(theCell.r + 1, theCell.c - 1))
{
Cell cell(theCell.r + 1, theCell.c - 1);
open(cell.r, cell.c);				// (2, 0)
if (isBlank(cell.r, cell.c))
newBlankCell.push(cell);
}

if (isValidCell(theCell.r - 1, theCell.c + 1) && isClosed(theCell.r - 1, theCell.c + 1))
{
Cell cell(theCell.r - 1, theCell.c + 1);
open(cell.r, cell.c);			// (0, 2)
if (isBlank(cell.r, cell.c))
newBlankCell.push(cell);
}
}
}
*/

// Write processBlankCell(int row, int col) using stack<Cell> newBlankCell use 2 while loops
/*
// Write processBlankCell(int row, int col) using stack<Cell> newBlankCell
void MineSweeper::processBlankCell(int row, int col)
{
stack<Cell> newBlankCell;
Cell firstBlankCell(row, col);
newBlankCell.push(firstBlankCell);

while (!newBlankCell.empty())
{
Cell theCell = newBlankCell.top();

while (true)
{
if (isValidCell(theCell.r + 1, theCell.c) && isClosed(theCell.r + 1, theCell.c))
{
Cell cell(theCell.r + 1, theCell.c);
open(cell.r, cell.c);			// (2, 1)
if (isBlank(cell.r, cell.c))
{
newBlankCell.push(cell);
break;
}
}

if (isValidCell(theCell.r, theCell.c + 1) && isClosed(theCell.r, theCell.c + 1))
{
Cell cell(theCell.r, theCell.c + 1);
open(cell.r, cell.c);		// (1, 2)
if (isBlank(cell.r, cell.c))
{
newBlankCell.push(cell);
break;
}
}

if (isValidCell(theCell.r + 1, theCell.c + 1) && isClosed(theCell.r + 1, theCell.c + 1))
{
Cell cell(theCell.r + 1, theCell.c + 1);
open(cell.r, cell.c);				// (2, 2)
if (isBlank(cell.r, cell.c))
{
newBlankCell.push(cell);
break;
}
}

if (isValidCell(theCell.r - 1, theCell.c) && isClosed(theCell.r - 1, theCell.c))
{
Cell cell(theCell.r - 1, theCell.c);
open(cell.r, cell.c);		// (0, 1)
if (isBlank(cell.r, cell.c))
{
newBlankCell.push(cell);
break;
}
}

if (isValidCell(theCell.r, theCell.c - 1) && isClosed(theCell.r, theCell.c - 1))
{
Cell cell(theCell.r, theCell.c - 1);
open(cell.r, cell.c);			// (1, 0)
if (isBlank(cell.r, cell.c))
{
newBlankCell.push(cell);
break;
}
}

if (isValidCell(theCell.r - 1, theCell.c - 1) && isClosed(theCell.r - 1, theCell.c - 1))
{
Cell cell(theCell.r - 1, theCell.c - 1);
open(cell.r, cell.c);			// (0, 0)
if (isBlank(cell.r, cell.c))
{
newBlankCell.push(cell);
break;
}
}

if (isValidCell(theCell.r + 1, theCell.c - 1) && isClosed(theCell.r + 1, theCell.c - 1))
{
Cell cell(theCell.r + 1, theCell.c - 1);
open(cell.r, cell.c);				// (2, 0)
if (isBlank(cell.r, cell.c))
{
newBlankCell.push(cell);
break;
}
}

if (isValidCell(theCell.r - 1, theCell.c + 1) && isClosed(theCell.r - 1, theCell.c + 1))
{
Cell cell(theCell.r - 1, theCell.c + 1);
open(cell.r, cell.c);			// (0, 2)
if (isBlank(cell.r, cell.c))
{
newBlankCell.push(cell);
break;
}
}

newBlankCell.pop();
break;
}
}
}
*/

// Write processBlankCell(int row, int col) using vector<Cell> newBlankCell
/*
void MineSweeper::processBlankCell(int row, int col)
{
vector<Cell> newBlankCell;
Cell firstBlankCell(row, col);
newBlankCell.push_back(firstBlankCell);

while (newBlankCell.size() != 0)
{
Cell theCell = newBlankCell[0];

if (isValidCell(theCell.r + 1, theCell.c) && isClosed(theCell.r + 1, theCell.c))
{
Cell cell(theCell.r + 1, theCell.c);
open(cell.r, cell.c);			// (2, 1)
if (isBlank(cell.r, cell.c))
newBlankCell.push_back(cell);
}

if (isValidCell(theCell.r, theCell.c + 1) && isClosed(theCell.r, theCell.c + 1))
{
Cell cell(theCell.r, theCell.c + 1);
open(cell.r, cell.c);		// (1, 2)
if (isBlank(cell.r, cell.c))
newBlankCell.push_back(cell);
}

if (isValidCell(theCell.r + 1, theCell.c + 1) && isClosed(theCell.r + 1, theCell.c + 1))
{
Cell cell(theCell.r + 1, theCell.c + 1);
open(cell.r, cell.c);				// (2, 2)
if (isBlank(cell.r, cell.c))
newBlankCell.push_back(cell);
}

if (isValidCell(theCell.r - 1, theCell.c) && isClosed(theCell.r - 1, theCell.c))
{
Cell cell(theCell.r - 1, theCell.c);
open(cell.r, cell.c);		// (0, 1)
if (isBlank(cell.r, cell.c))
newBlankCell.push_back(cell);
}

if (isValidCell(theCell.r, theCell.c - 1) && isClosed(theCell.r, theCell.c - 1))
{
Cell cell(theCell.r, theCell.c - 1);
open(cell.r, cell.c);			// (1, 0)
if (isBlank(cell.r, cell.c))
newBlankCell.push_back(cell);
}

if (isValidCell(theCell.r - 1, theCell.c - 1) && isClosed(theCell.r - 1, theCell.c - 1))
{
Cell cell(theCell.r - 1, theCell.c - 1);
open(cell.r, cell.c);			// (0, 0)
if (isBlank(cell.r, cell.c))
newBlankCell.push_back(cell);
}

if (isValidCell(theCell.r + 1, theCell.c - 1) && isClosed(theCell.r + 1, theCell.c - 1))
{
Cell cell(theCell.r + 1, theCell.c - 1);
open(cell.r, cell.c);				// (2, 0)
if (isBlank(cell.r, cell.c))
newBlankCell.push_back(cell);
}

if (isValidCell(theCell.r - 1, theCell.c + 1) && isClosed(theCell.r - 1, theCell.c + 1))
{
Cell cell(theCell.r - 1, theCell.c + 1);
open(cell.r, cell.c);			// (0, 2)
if (isBlank(cell.r, cell.c))
newBlankCell.push_back(cell);
}

newBlankCell.erase(newBlankCell.begin());
}
}
*/

// Write processBlankCell(int row, int col) using recursion
/*
void MineSweeper::processBlankCell(int row, int col)
{

if (isValidCell(row + 1, col) && isClosed(row + 1, col))
{
open(row + 1, col);			// (2, 1)
if (isBlank(row + 1, col))
processBlankCell(row + 1, col);
}

if (isValidCell(row, col + 1) && isClosed(row, col + 1))
{
open(row, col + 1);			// (1, 2)
if (isBlank(row, col + 1))
processBlankCell(row, col + 1);
}

if (isValidCell(row + 1, col + 1) && isClosed(row + 1, col + 1))
{
open(row + 1, col + 1);				// (2, 2)
if (isBlank(row + 1, col + 1))
processBlankCell(row + 1, col + 1);
}

if (isValidCell(row - 1, col) && isClosed(row - 1, col))
{
open(row - 1, col);			// (0, 1)
if (isBlank(row - 1, col))
processBlankCell(row - 1, col);
}

if (isValidCell(row, col - 1) && isClosed(row, col - 1))
{
open(row, col - 1);			// (1, 0)
if (isBlank(row, col - 1))
processBlankCell(row, col - 1);
}

if (isValidCell(row - 1, col - 1) && isClosed(row - 1, col - 1))
{
open(row - 1, col - 1);				// (0, 0)
if (isBlank(row - 1, col - 1))
processBlankCell(row - 1, col - 1);
}

if (isValidCell(row + 1, col - 1) && isClosed(row + 1, col - 1))
{
open(row + 1, col - 1);				// (2, 0)
if (isBlank(row + 1, col - 1))
processBlankCell(row + 1, col - 1);
}

if (isValidCell(row - 1, col + 1) && isClosed(row - 1, col + 1))
{
open(row - 1, col + 1);				// (0, 2)
if (isBlank(row - 1, col + 1))
processBlankCell(row - 1, col + 1);
}
}
*/

// Write processBlankCell(int row, int col) using while(true)
/*
void MineSweeper::processBlankCell(const Cell& c)
{
while (true)
{
bool isNewCellOpened = false;
for (int i = 0; i < board_size; i++)
{
for (int j = 0; j < board_size; j++)
{
if (curState[i][j] == BLANK)
{
if (i < board_size - 1 && curState[i + 1][j] == CLOSED) {
open(i + 1, j);			// (2, 1)
if (isBlank(i + 1, j))
isNewCellOpened = true;
}
if (j < board_size - 1 && curState[i][j + 1] == CLOSED) {
// (1, 2)
open(i, j + 1);
if (isBlank(i, j + 1))
isNewCellOpened = true;
}
if (i < board_size - 1 && j < board_size - 1 && curState[i + 1][j + 1] == CLOSED) {
Cell cell(i + 1, j + 1);		// (2, 2)
open(cell);
if (isBlank(cell))
isNewCellOpened = true;
}
if (i >= 1 && curState[i - 1][j] == CLOSED) {
Cell cell(i - 1, j);			// (0, 1)
open(cell);
if (isBlank(cell))
isNewCellOpened = true;
}
if (j >= 1 && curState[i][j - 1] == CLOSED) {
Cell cell(i, j - 1);			// (1, 0)
open(cell);
if (isBlank(cell))
isNewCellOpened = true;
}
if (i >= 1 && j >= 1 && curState[i - 1][j - 1] == CLOSED) {
Cell cell(i - 1, j - 1);		// (0, 0)
open(cell);
if (isBlank(cell))
isNewCellOpened = true;
}
if (i < board_size - 1 && j >= 1 && curState[i + 1][j - 1] == CLOSED) {
Cell cell(i + 1, j - 1);		// (2, 0)
open(cell);
if (isBlank(cell))
isNewCellOpened = true;
}
if (j < board_size - 1 && i >= 1 && curState[i - 1][j + 1] == CLOSED) {
Cell cell(i - 1, j + 1);		// (0, 2)
open(cell);
if (isBlank(cell))
isNewCellOpened = true;
}
}
}
}
if (isNewCellOpened == false)
break;
}
}
*/


void MineSweeper::render() 
{
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);   // white background
	SDL_RenderClear(renderer);

	// Draw the physical board
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);	// Black lines

	// Horizontal lines
	for (int i = 0; i < board_size + 1; i++) 
		SDL_RenderDrawLine(renderer, board_x, board_y + box_length * i,
			board_x + board_size * box_length, board_y + box_length * i);

	// Vertical lines
	for (int i = 0; i < board_size + 1; i++) 
		SDL_RenderDrawLine(renderer, board_x + box_length * i, board_y,
			board_x + box_length * i, board_y + board_size * box_length);

	// Draw board cells
	for (int i = 0; i < curState.size(); i++)
		for (int j = 0; j < curState.size(); j++)
			if (curState[i][j] != CLOSED)
				renderTexture(image[curState[i][j]], renderer, j * box_length + board_x, i * box_length + board_y, box_length, box_length);

	REPLAY_X = 0;
	REPLAY_Y = 0;
	REPLAY_W = board_x;
	REPLAY_H = 100;

	EASY_X = MED_X = HARD_X = (board_x - LEVEL_IMG_LENGTH) / 2;
	EASY_Y = REPLAY_Y + REPLAY_H;
	MED_Y = EASY_Y + LEVEL_IMG_LENGTH + SPACE_BETWN_LEVEL_IMG;
	HARD_Y = MED_Y + LEVEL_IMG_LENGTH + SPACE_BETWN_LEVEL_IMG;

	// Replay 
	renderTexture(image[PLAY_AGAIN], renderer, REPLAY_X, REPLAY_Y, REPLAY_W, REPLAY_H);
	renderTexture(image[EASY], renderer, EASY_X, EASY_Y, LEVEL_IMG_LENGTH, LEVEL_IMG_LENGTH);
	renderTexture(image[MED], renderer, MED_X, MED_Y, LEVEL_IMG_LENGTH, LEVEL_IMG_LENGTH);
	renderTexture(image[HARD], renderer, HARD_X, HARD_Y, LEVEL_IMG_LENGTH, LEVEL_IMG_LENGTH);


	SDL_RenderPresent(renderer);
}

void MineSweeper::play()
{
	SDL_Event e;

	while (true)
	{
		SDL_Delay(10);
		if (SDL_WaitEvent(&e) == 0) continue;
		if (e.type == SDL_QUIT) return;
		if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) return;

		if (e.type == SDL_MOUSEBUTTONDOWN && isInBoard(e))
		{
			int row = getRow(e);
			int col = getCol(e);

			if (isRightClick(e)) 
			{
				processRightClick(row, col);
				render();
			}	
			else
			{
				open(row, col);
				render();

				if (isMine(row, col))
					lose();
				else if (isBlank(row, col)) 
				{
					processBlankCell(row, col);
					render();
				}		
			}

		}

		if (allNoMineCellOpened())
			win();

		if (e.type == SDL_MOUSEBUTTONDOWN && isReplay(e))
		{
			newGame(level);
			render();
		}
	}

}

void MineSweeper::openAllBoxes()
{
	for (int i = 0; i < board_size; i++)
		for (int j = 0; j < board_size; j++) {
			open(i, j);
			if (!isMine(i, j))
				curCorrectBoxes--;
		}
	render();
}

bool MineSweeper::isReplay(const SDL_Event & e)
{
	int curX = getMouseX(e);
	int curY = getMouseY(e);

	if (curX > EASY_X && (curX < EASY_X + LEVEL_IMG_LENGTH) && curY > EASY_Y && (curY < EASY_Y + LEVEL_IMG_LENGTH))
	{
		level = "easy";
		return true;
	}
	else if (curX > MED_X && (curX < MED_X + LEVEL_IMG_LENGTH) && curY > MED_Y && (curY < MED_Y + LEVEL_IMG_LENGTH))
	{
		level = "med";
		return true;
	}
	else if (curX > HARD_X && (curX < HARD_X + LEVEL_IMG_LENGTH) && curY > HARD_Y && (curY < HARD_Y + LEVEL_IMG_LENGTH))
	{
		level = "hard";
		return true;
	}
	else
		return false;
}

void MineSweeper::win()
{
	SDL_Delay(500);

	openAllBoxes();
	renderTexture(image[WIN_IMAGE], renderer, board_x + box_length * board_size + 1, board_y, board_x, box_length*board_size * 3 / 4);
	SDL_RenderPresent(renderer);
}

void MineSweeper::lose()
{
	// Show mine image
	render();

	SDL_Delay(500);

	openAllBoxes();
	renderTexture(image[LOSE_IMAGE], renderer, board_x + box_length * board_size + 1, board_y, board_x, box_length*board_size * 3 / 4);
	SDL_RenderPresent(renderer);
}

void MineSweeper::exit()		
{
	for (int i = 0; i < image.size(); i++)
		SDL_DestroyTexture(image[i]);
	quitSDL(window, renderer);
}

vector<SDL_Texture*> MineSweeper::loadImage()
{
	const size_t NUM_OF_IMAGES = sizeof(lib) / sizeof(*lib);
	vector<SDL_Texture*> image;
	for (int i = 0; i < NUM_OF_IMAGES; i++) {
		image.push_back(loadTexture(lib[i], renderer));
		//If error in loading, free loaded texture, quit SDL
		if (image[i] == nullptr) {
			for (int j = 0; j < i + 1; j++)
				SDL_DestroyTexture(image[j]);
			quitSDL(window, renderer);
		}
	}
	return image;
}

void MineSweeper::calHints()
{
	for (int i = 0; i < finalState.size(); i++)
		for (int j = 0; j < finalState.size(); j++)
			if (finalState[i][j] == MINE)
			{
				if (i < board_size - 1 && finalState[i + 1][j] != MINE)
					finalState[i + 1][j]++;				// (2, 1)
				if (j < board_size - 1 && finalState[i][j + 1] != MINE)
					finalState[i][j + 1]++;				// (1, 2)
				if (i < board_size - 1 && j < board_size - 1 && finalState[i + 1][j + 1] != MINE)
					finalState[i + 1][j + 1]++;			// (2, 2)
				if (i >= 1 && finalState[i - 1][j] != MINE)
					finalState[i - 1][j]++;				// (0, 1)
				if (j >= 1 && finalState[i][j - 1] != MINE)
					finalState[i][j - 1]++;				// (1, 0)
				if (i >= 1 && j >= 1 && finalState[i - 1][j - 1] != MINE)
					finalState[i - 1][j - 1]++;			// (0, 0)
				if (i < board_size - 1 && j >= 1 && finalState[i + 1][j - 1] != MINE)
					finalState[i + 1][j - 1]++;			// (2, 0)
				if (j < board_size - 1 && i >= 1 && finalState[i - 1][j + 1] != MINE)
					finalState[i - 1][j + 1]++;			// (0, 2)
			}
}

bool MineSweeper::isValidCell(int row, int col)
{
	if (0 <= row && row < board_size && 0 <= col && col < board_size)
		return true;
	else
		return false;
}

Cell::Cell(int row, int col)
{
	r = row;
	c = col;
}
