#include "SDL_Utensils.h"

using namespace std;

void initSDL(SDL_Window* &window, SDL_Renderer* &renderer,
	int screenWidth, int screenHeight, const char* windowTitle)
{
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
		logSDLError(std::cout, "SDL_Init", true);

	window = SDL_CreateWindow(windowTitle, SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED, screenWidth, screenHeight, SDL_WINDOW_SHOWN);
	// Shown window fullscreen dextop
	/*
	window = SDL_CreateWindow(WINDOW_TITLE.c_str(), SDL_WINDOWPOS_CENTERED,
	SDL_WINDOWPOS_CENTERED, screenWidth, screenHeight, SDL_WINDOW_FULLSCREEN_DESKTOP);
	if (window == nullptr) logSDLError(std::cout, "CreateWindow", true);
	*/
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
	SDL_RenderSetLogicalSize(renderer, screenWidth, screenHeight);
}

void logSDLError(ostream & os, const string & msg, bool fatal)
{
	os << msg << "Error: " << SDL_GetError() << endl;
	if (fatal)
	{
		SDL_Quit();
		exit(1);
	}
}

void waitUntilKeyPressed()
{
	SDL_Event e;
	while (true)
	{
		if (SDL_WaitEvent(&e) != 0 && (e.type == SDL_KEYDOWN || e.type == SDL_QUIT))
			return;
		SDL_Delay(100);
	}
}

void quitSDL(SDL_Window* window, SDL_Renderer* renderer)
{
	// free memory
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

/**
* Loads an image into a texture on the rendering device
* @param file The image file to load
* @param ren The renderer to load the texture onto
* @return the loaded texture, or nullptr if something went wrong.
*/
SDL_Texture* loadTexture(const std::string &file, SDL_Renderer *ren) {
	SDL_Texture *texture = IMG_LoadTexture(ren, file.c_str());
	if (texture == nullptr) {
		logSDLError(std::cout, "LoadTexture");
	}
	return texture;
}

/**
* Loads a BMP image into a texture on the rendering device
* @param file The BMP image file to load
* @param ren The renderer to load the texture onto
* @return the loaded texture, or nullptr if something went wrong.
*/
/*
SDL_Texture* loadTexture(const std::string &file, SDL_Renderer *ren)
{
	//Kh?i t?o là nullptr ?? tránh l?i 'dangling pointer'
	SDL_Texture *texture = nullptr;
	//N?p ?nh t? tên file (v?i ???ng d?n)
	SDL_Surface *loadedImage = SDL_LoadBMP(file.c_str());
	//N?u không có l?i, chuy?n ??i v? d?ng texture and và tr? v?
	if (loadedImage != nullptr) {
		texture = SDL_CreateTextureFromSurface(ren, loadedImage);
		SDL_FreeSurface(loadedImage);
		//??m b?o vi?c chuy?n ??i không có l?i
		if (texture == nullptr) {
			logSDLError(std::cout, "CreateTextureFromSurface");
		}
	}
	else {
		logSDLError(std::cout, "LoadBMP");
	}
	return texture;
}
*/

/**
* Draw an SDL_Texture to an SDL_Renderer at position x, y, preserving
* the texture's width and height
* @param tex The source texture we want to draw
* @param ren The renderer we want to draw to
* @param x The x coordinate to draw to
* @param y The y coordinate to draw to
*/
void renderTexture(SDL_Texture *tex, SDL_Renderer *ren, int x, int y)
{
	//Thi?t l?p hình ch? nh?t ?ích mà chúng ta mu?n v? ?nh vào trong
	SDL_Rect dst;
	dst.x = x;
	dst.y = y;
	//Truy v?n texture ?? l?y chi?u r?ng và cao (vào chi?u r?ng và cao t??ng ?ng c?a hình ch? nh?t ?ích)
	SDL_QueryTexture(tex, NULL, NULL, &dst.w, &dst.h);
	//??a toàn b? ?nh trong texture vào hình ch? nh?t ?ích
	SDL_RenderCopy(ren, tex, NULL, &dst);
}

/**
* Draw an SDL_Texture to an SDL_Renderer at position x, y, with some desired
* width and height
* @param tex The source texture we want to draw
* @param ren The renderer we want to draw to
* @param x The x coordinate to draw to
* @param y The y coordinate to draw to
* @param w The width of the texture to draw
* @param h The height of the texture to draw
*/
void renderTexture(SDL_Texture *tex, SDL_Renderer *ren, int x, int y, int w, int h)
{
	//Setup the destination rectangle to be at the position we want
	SDL_Rect dst;
	dst.x = x;
	dst.y = y;
	dst.w = w;
	dst.h = h;

	SDL_RenderCopy(ren, tex, NULL, &dst);
}

Mix_Chunk* loadChunk(const std::string &file)
{
	Mix_Chunk *chunk = Mix_LoadWAV(file.c_str());
	if (chunk == NULL)
	{
		cout << "SDL_mixer Error: " << Mix_GetError() << endl;
		Mix_FreeChunk(chunk);
	}
	return chunk;
}

int getMouseX(const SDL_Event& e) { return e.button.x; }

int getMouseY(const SDL_Event& e) { return e.button.y; }

