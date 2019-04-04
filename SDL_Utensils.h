#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <string>
#include <ostream>
#include <iostream>
#include <cstring>

void initSDL(SDL_Window* &window, SDL_Renderer* &renderer,
	int screenWidth, int screenHeight, const char* windowTitle);
void logSDLError(std::ostream& os, const std::string &msg, bool fatal = false);
void waitUntilKeyPressed();
void quitSDL(SDL_Window* window, SDL_Renderer* renderer);

SDL_Texture* loadTexture(const std::string &file, SDL_Renderer *ren);
void renderTexture(SDL_Texture *tex, SDL_Renderer *ren, int x, int y);
void renderTexture(SDL_Texture *tex, SDL_Renderer *ren, int x, int y, int w, int h);

Mix_Chunk* loadChunk(const std::string &file);

int getMouseX(const SDL_Event& e);
int getMouseY(const SDL_Event& e);
