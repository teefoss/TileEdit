
#ifndef sdl_h
#define sdl_h

#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#include "type.h"

// Window Options
#define WINDOWNAME		"TileEdit"
#define DPI				1
#define SCREENWIDTH 	832
#define SCREENHEIGHT 	560
//#define FULLSCREEN

// Font Options
#define USEFONTS

#ifdef USEFONTS
#include <SDL2_ttf/SDL_ttf.h>
#define FONTSIZE 16*DPI
#define FONTNAME "fonts/Px437_IBM_VGA9.ttf"
extern TTF_Font	*font;
#endif

typedef struct
{
	SDL_Texture		*texture;
	sizetype			size;
} texture_t;

typedef struct
{
	unsigned char red;
	unsigned char green;
	unsigned char blue;
	unsigned char alpha;
} color_t;

extern SDL_Color		vgacolor[16];

typedef enum
{
	black,			// 0
	blue,			// 1
	green,			// 2
	cyan,			// 3
	red,			// 4
	magenta,		// 5
	brown,			// 6
	white,			// 7
	gray,			// 8
	lightblue,
	lightgreen,
	lightcyan,
	pink,
	lightmagenta,
	yellow,
	brightwhite
} vgacolor_t;


//extern SDL_Window		*window;
extern SDL_Renderer		*renderer;

void 		FatalError(const char *message);
SDL_Texture *LoadSDLTexture(const char *file);
texture_t 	LoadTextureFromFile(const char *file);
texture_t 	CreateText(const char *string, SDL_Color color);
SDL_Rect 	SDLRect(int x, int y, int w, int h);
SDL_Rect 	SDLRectFromPoint(point_t pt, sizetype size);

void StartSDL(void);
void StopSDL(void);

void SetDrawColor(SDL_Color c);

#endif /* sdl_h */
