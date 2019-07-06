#include <stdbool.h>
#include <stdio.h>

#include <SDL_image.h>

#define BG_FILE "space_background_asteroid.png"
#define FG_FILE "beams.png"
#define WIDTH 512
#define HEIGHT 384
#define FG_WIDTH 289
#define FG_HEIGHT 84
#define FG_X 200
#define FG_Y_START (HEIGHT / 4 / 2 - FG_HEIGHT / 2)


SDL_Texture *LoadTexFromFile(SDL_Renderer *renderer, const char *filename)
{
	SDL_Surface *s = NULL;
	SDL_Texture *t = NULL;

	// Load image from file
	s = IMG_Load(filename);
	if (s == NULL)
	{
		printf("Failed to load image %s from file\n", filename);
		goto bail;
	}

	// Convert to texture
	t = SDL_CreateTextureFromSurface(renderer, s);
	if (t == NULL)
	{
		printf("Failed to convert surface: %s\n", SDL_GetError());
		goto bail;
	}

	SDL_FreeSurface(s);
	return t;

bail:
	SDL_DestroyTexture(t);
	SDL_FreeSurface(s);
	return NULL;
}

int RenderFG(
	SDL_Renderer *renderer, SDL_Texture *fg, const SDL_BlendMode mode,
	const int i)
{
	int res = 0;
	res = SDL_SetTextureBlendMode(fg, mode);
	if (res != 0)
	{
		printf("Failed to set blend mode: %s\n", SDL_GetError());
		goto bail;
	}
	SDL_Rect dst = { FG_X, FG_Y_START + i * (HEIGHT / 4), FG_WIDTH, FG_HEIGHT };
	res = SDL_RenderCopy(renderer, fg, NULL, &dst);
	if (res != 0)
	{
		printf("Failed to blit surface: %s\n", SDL_GetError());
		goto bail;
	}

bail:
	return res;
}

int main(int argc, char *argv[])
{
	SDL_Texture *bg = NULL;
	SDL_Texture *fg = NULL;
	SDL_Window *window = NULL;
	SDL_Renderer *renderer = NULL;

	// Init SDL
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		printf("SDL_Init error: %s\n", SDL_GetError());
		goto bail;
	}

	// Create display window
	window = SDL_CreateWindow(
		"SDL2 Blend Modes", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		WIDTH, HEIGHT, 0);
	if (window == NULL)
	{
		printf("Failed to create window: %s\n", SDL_GetError());
		goto bail;
	}
	renderer = SDL_CreateRenderer(window, -1, 0);
	if (renderer == NULL)
	{
		printf("Failed to create renderer: %s\n", SDL_GetError());
		goto bail;
	}
	SDL_RenderClear(renderer);
	SDL_RenderSetLogicalSize(renderer, WIDTH, HEIGHT);

	// Load textures from files
	bg = LoadTexFromFile(renderer, BG_FILE);
	if (bg == NULL)
	{
		goto bail;
	}
	fg = LoadTexFromFile(renderer, FG_FILE);
	if (fg == NULL)
	{
		goto bail;
	}

	// Render
	if (SDL_RenderCopy(renderer, bg, NULL, NULL) != 0)
	{
		printf("Failed to blit texture: %s\n", SDL_GetError());
		goto bail;
	}
	if (RenderFG(renderer, fg, SDL_BLENDMODE_NONE, 0) != 0)
	{
		goto bail;
	}
	if (RenderFG(renderer, fg, SDL_BLENDMODE_BLEND, 1) != 0)
	{
		goto bail;
	}
	if (RenderFG(renderer, fg, SDL_BLENDMODE_ADD, 2) != 0)
	{
		goto bail;
	}
	if (RenderFG(renderer, fg, SDL_BLENDMODE_MOD, 3) != 0)
	{
		goto bail;
	}
	SDL_RenderPresent(renderer);

	bool quit = false;
	while (!quit)
	{
		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			switch (e.type)
			{
				case SDL_QUIT:
					quit = true;
					break;
				default:
					break;
			}
		}
		SDL_Delay(100);
	}

bail:
	SDL_DestroyTexture(bg);
	SDL_DestroyTexture(fg);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
