#pragma once
#include <string>

const std::string lib[] = 
{ 
	"MS_Images/number_0.bmp",			// lib[0] -> BLANK = 0
	"MS_Images/number_1.bmp", 
	"MS_Images/number_2.bmp",
	"MS_Images/number_3.bmp", 
	"MS_Images/number_4.bmp",
	"MS_Images/number_5.bmp",
	"MS_Images/number_6.bmp",
	"MS_Images/number_7.bmp",
	"MS_Images/number_8.bmp",
	"MS_Images/mine.bmp",				// lib[9] -> MINE = 9 -> if changed order of pic, change const int MINE = 9 
	"MS_Images/flag.bmp",				// lib[10] -> OPEN_FLAG = 10 -> if changed order of pic, change const int OPEN_FLAG = 10 
	"MS_Images/quest_mark.bmp",			// lib[11] -> OPEN_QUEST = 11 -> if changed order of pic, change const int OPEN_QUEST = 11 
	"MS_Images/win.bmp",				// lib[12] -> image for winning = 12 -> if changed order of pic, change const int WIN_IMAGE
	"MS_Images/lose_meme.bmp",			// lib[13] -> image for lose = 13 -> if changed order of pic, change const int LOSE_IMAGE
	"MS_Images/play_again.png",			// lib[14]
	"MS_Images/select_level.png",
	"MS_Images/easy.png",
	"MS_Images/medium.png",
	"MS_Images/hard.png"
};

// To understand these number -> go to string lib[]
const int CLOSED = 100;				// 100 so it's different from all index number of images in string lib[]	
const int BLANK = 0;
const int MINE = 9;
const int OPEN_FLAG = 10;
const int OPEN_QUEST = 11;
const int WIN_IMAGE = 12;
const int LOSE_IMAGE = 13;
const int PLAY_AGAIN = 14;
const int SLCT_LEVEL = 15;
const int EASY = 16;
const int MED = 17;
const int HARD = 18;



