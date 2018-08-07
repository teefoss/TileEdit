//
//  sdl.c
//
//  Created by Thomas Foster on 4/15/18.
//  Copyright © 2018 Thomas Foster. All rights reserved.
//
//  starting/stopping SDL
//


#include "sdl.h"
#include "tffunctions.h"
//#include <SDL2_mixer/SDL_mixer.h>

SDL_Window		*window = NULL;
SDL_Surface		*screensurface = NULL;
SDL_Renderer	*renderer = NULL;

#ifdef USEFONTS
TTF_Font		*font = NULL;
#endif


void FatalError(const char *message)
{
	printf("%s SDL Error: %s\n", message, SDL_GetError());
	SDL_Quit();
	exit(1);
}


SDL_Texture *LoadSDLTexture(const char *file)
{
	SDL_Surface *s;
	SDL_Texture *t;
	
	s = IMG_Load(file);
	if (!s) {
		printf("LoadTexture: Could not load '%s'!\n", file);
		FatalError("");
	}
	
	t = SDL_CreateTextureFromSurface(renderer, s);
	
	SDL_FreeSurface(s);
	return t;
}

//
// LoadTextureFromFile
/** Returns a texture_t with texture and size loaded */
texture_t LoadTextureFromFile(const char *file)
{
	SDL_Surface *s;
	texture_t t;
	
	s = IMG_Load(file);
	if (!s) {
		printf("LoadTexture: Could not load '%s'!\n", file);
		FatalError("");
	}
	
	t.texture = SDL_CreateTextureFromSurface(renderer, s);
	t.size = MakeSize(s->w, s->h);

	SDL_FreeSurface(s);
	return t;
}


texture_t CreateText(const char *string, SDL_Color color)
{
	SDL_Surface *s;
	texture_t	t;
	
	s = TTF_RenderText_Solid(font, string, color);
	if (!s) {
		printf("CreateText: could not load text! '%s'\n",string);
		FatalError("");
	}
	t.texture = SDL_CreateTextureFromSurface(renderer, s);
	t.size = MakeSize(s->w, s->h);
	
	SDL_FreeSurface(s);
	return t;
}


SDL_Rect SDLRect(int x, int y, int w, int h)
{
	SDL_Rect r = { x,y,w,h };
	return r;
}

SDL_Rect SDLRectFromPoint(point_t pt, sizetype size)
{
	SDL_Rect r = { pt.x, pt.y, size.width, size.height };
	return r;
}


void StartSDL(void)
{
	printf("loading SDL...\n");
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
		FatalError("StartSDL: SDL_Init failed!");
	
	
	// create window
	window = SDL_CreateWindow(WINDOWNAME,
							  SDL_WINDOWPOS_UNDEFINED,
							  SDL_WINDOWPOS_UNDEFINED,
							  SCREENWIDTH,
							  SCREENHEIGHT,
							  SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI);
	if (!window)
		FatalError("StartSDL: Could not create window!");
#ifdef FULLSCREEN
		SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
#endif
	
	// create renderer
	renderer = SDL_CreateRenderer(window,-1, 0);
	if (!renderer)
		FatalError("InitRenderer: Could not create renderer!");

	SDL_RenderSetScale(renderer, 2, 2);
	// create mixer
//	Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 512);
	
#ifdef USEFONTS
	if (TTF_Init() == -1)
		FatalError("StartSDL: Could not init TTF!");
	font = TTF_OpenFont(FONTNAME, FONTSIZE);
	if (!font)
		FatalError("StartSDL: Could not load font!");
#endif
	screensurface = SDL_GetWindowSurface(window);
	
	printf("SDL loaded\n");
}




void StopSDL()
{
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

#ifdef USEFONTS
	TTF_CloseFont(font);
	TTF_Quit();
	font = NULL;
#endif
	//Mix_CloseAudio();
	SDL_Quit();
	printf("SDL Quit\n\n");
}


#pragma mark - RENDER

void SetDrawColor(SDL_Color c)
{
	SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
}

#pragma mark - COLOR

//http://answers.google.com/answers/threadview/id/126097.html
SDL_Color
vgacolor[16] = {
	{0, 0, 0}, 			// 0
	{ 0, 0, 170 }, 		// 1
	{ 0, 170, 0 }, 		// 2
	{ 0, 170, 170 }, 	// 3
	{ 170, 0, 0 }, 		// 4
	{ 170, 0, 170 }, 	// 5
	{ 170, 85, 0 }, 	// 6
	{ 170, 170, 170}, 	// 7
	{ 85, 85, 85 }, 	// 8
	{ 85, 85, 255 }, 	// 9
	{ 85, 255, 85 }, 	// 10
	{ 85, 255, 255 }, 	// 11
	{ 255, 85, 85 }, 	// 12
	{ 255, 85, 255 }, 	// 13
	{ 255, 255, 85 }, 	// 14
	{ 255, 255, 255}, 	// 15
};


SDL_Color MakeColor(unsigned char r, unsigned char g, unsigned char b)
{
	SDL_Color c = { r,g,b,255 };
	return c;
}


