//
//  main.c
//  TileEdit
//
//  Created by Thomas Foster on 8/2/18.
//  Copyright © 2018 Thomas Foster. All rights reserved.
//

#include <stdio.h>
#include "sdl.h"
#include "tffunctions.h"
#include <unistd.h>
#include "main.h"
#include "loadsave.h"

mapobject_t tiledata[ROWS][COLS] = { };

int numobjects;
mapobject_t objects[1200];


unsigned 	ticks = 0;
cursor_t	cursor;
sizetype	tilesize;

bool		fillmode;
point_t		fillpoint[2];
bool		gotfillpt;
int 		tag = 0;

#pragma mark -

#define NUMINFO 5

SDL_Texture		*images[NUMOBJECTS];
SDL_Rect 		imagerects[NUMOBJECTS];
texture_t 		labels[NUMOBJECTS];
SDL_Rect		labelrects[NUMOBJECTS];
texture_t		infolabels[NUMINFO];
SDL_Rect		inforects[NUMINFO];

const char *labelnames[NUMOBJECTS] = {
	"a Delete",
	"b Block",
	"c Portal",
	"d Player Start",
	"e Apple"
};

const char *infonames[NUMINFO] = {
	"/  Toggle Fill Mode",
	"1  Save",
	"22 Revert",
	"33 Block Fill",
	"00 Quit without Saving"
};



void InitMenu(void)
{
	int i;
	
	images[0] = NULL;
	images[BLOCK] = LoadSDLTexture("textures/block.png");
	images[PORTAL] = LoadSDLTexture("textures/portal.png");
	images[START] = LoadSDLTexture("editor/snakehead.png");
	images[APPLE] = LoadSDLTexture("textures/apple.png");
	
	for (i=0; i<NUMOBJECTS; i++)
	{
		labels[i] = CreateText(labelnames[i], vgacolor[white]);
		imagerects[i] = SDLRect(MENUX, i*TILESIZE, TILESIZE, TILESIZE);
		labelrects[i] = SDLRect(LABELSX, i*TILESIZE, labels[i].size.width, labels[i].size.height);
	}
	
	for (i=0; i<NUMINFO; i++)
	{
		infolabels[i] = CreateText(infonames[i], vgacolor[yellow]);
		inforects[i] = SDLRect(0, EDITORHEIGHT+(i*TILESIZE), infolabels[i].size.width, infolabels[i].size.height);
	}
}

texture_t	settaglabel;
texture_t	currenttaglabel;
SDL_Rect	settagrect;
SDL_Rect	currenttagrect;

void UpdateSetTagLabel (void)
{
	char 		text[32];
	int 		tagx;
	
	sprintf(text, "Set Tag: %d", tag);
	settaglabel = CreateText(text, vgacolor[white]);
	
	//printf("w = %d, h = %d\n", taglabel.size.width, taglabel.size.height);
	
	tagx = 640-settaglabel.size.width;
	settagrect = SDLRect(tagx, 480, settaglabel.size.width, settaglabel.size.height);
}



//
//  UpdateCurrentTag
/// Update the Current Tag Label
/// Call when tag set or hover
//
void UpdateCurrentTagLabel (int tag)
{
	char 		text[32];
	int 		tagx;
	
	if (tag < 100)
		sprintf(text, "Current Tag: -");
	else
		sprintf(text, "Current Tag: %d", tag-100);
	currenttaglabel = CreateText(text, vgacolor[white]);
	
	//printf("w = %d, h = %d\n", taglabel.size.width, taglabel.size.height);
	
	tagx = 640-currenttaglabel.size.width;
	currenttagrect = SDLRect(tagx, 480+TILESIZE, currenttaglabel.size.width, currenttaglabel.size.height);
}




void FreeTextures (void)
{
	int i;
	
	for (i=0; i<NUMOBJECTS; i++)
	{
		if (images[i])
			SDL_DestroyTexture(images[i]);
		SDL_DestroyTexture(labels[i].texture);
	}
	
	for (i=0; i<NUMINFO; i++)
		SDL_DestroyTexture(infolabels[i].texture);
}





//==============================================================
#pragma mark -


void PrintCursorLocation(void)
{
#if 0
	printf("cursor: (%d, %d)\n", cursor.loc.x, cursor.loc.y);
	printf("cursor at map (%d, %d)\n",cursor.loc.x/16, cursor.loc.y/16);
#endif
}




//
//  Fill
/// Called to begin dragging a fill box
//
void Fill(void)
{
	int x;
	int starty, startx;
	int stopy, stopx;

	if (!fillmode)
		printf("Error! Fill called when not in fillmode");
	
	if (!gotfillpt) {
		fillpoint[0] = MakePoint(cursor.loc.x, cursor.loc.y);
		gotfillpt = true;
		return;
	}
	
	fillpoint[1] = MakePoint(cursor.loc.x, cursor.loc.y);

	// TODO check both points not -1
	
	if (fillpoint[0].x <= fillpoint[1].x) {
		startx = fillpoint[0].x/TILESIZE;
		stopx = fillpoint[1].x/TILESIZE;
	} else {
		startx = fillpoint[1].x/TILESIZE;
		stopx = fillpoint[0].x/TILESIZE;
	}
	
	if (fillpoint[0].y <= fillpoint[1].y) {
		starty = fillpoint[0].y/TILESIZE;
		stopy = fillpoint[1].y/TILESIZE;
	} else {
		starty = fillpoint[1].y/TILESIZE;
		stopy = fillpoint[0].y/TILESIZE;
	}
	
	printf("start: (%d, %d), stop (%d, %d)\n", startx,starty,stopx,stopy);

	do {
		x = startx;
		do {
//			tiledata[starty][x] = cursor.current;
		} while (x++ < stopx);
	} while (starty++ < stopy);
	
	gotfillpt = false;
}





typedef enum { up, down, left, right } direction_t;

void MoveCursor(int dist, direction_t dir)
{
	byte *data;
	
	switch (dir) {
		case up:
			if (cursor.loc.y > 0)
				cursor.loc.y -= dist;
			if (cursor.loc.y < 0)
				cursor.loc.y = 0;
			break;
		case down:
			if (cursor.loc.y < EDITORHEIGHT-TILESIZE)
				cursor.loc.y += dist;
			if (cursor.loc.y > EDITORHEIGHT-TILESIZE)
				cursor.loc.y = EDITORHEIGHT-TILESIZE;
			break;
		case left:
			if (cursor.loc.x > 0)
				cursor.loc.x -= dist;
			if (cursor.loc.x < 0)
				cursor.loc.x = 0;
			break;
		case right:
			if (cursor.loc.x < EDITORWIDTH-TILESIZE)
				cursor.loc.x += dist;
			if (cursor.loc.x > EDITORWIDTH-TILESIZE)
				cursor.loc.x = EDITORWIDTH-TILESIZE;
			break;

		default:
			break;
	}
	cursor.blink = 1;
	PrintCursorLocation();

	data = &tiledata[cursor.loc.y/TILESIZE][cursor.loc.x/TILESIZE];
	UpdateCurrentTagLabel(*data);

}



void ReplaceObjectAt (int x, int y)
{
	
}

/// Add a new object at the end of the array
void NewObjectAt(int x, int y)
{
	mapobject_t obj;
	
	numobjects++;
	obj.type = cursor.current;
	obj.x = x;
	obj.y = y;
	obj.tag = tag;
	objects[numobjects-1] = obj;
}

void InsertObjectAt(int index)
{
	mapobject_t obj;
	
	obj.type = cursor.current;
}

void RemoveObjectAt(int index)
{
	objects[index].type = -1;
}

void PlaceObjectAt (int x, int y)
{
	int i;
	
	for (i=0; i<numobjects; i++)
	{
		if (objects[i].x == x && objects[i].y == y) {
			if (objects[i].type == -1)
				InsertObjectAt(i);
			if (objects[i].type == cursor.current)
				RemoveObjectAt(i);
			
		} else {
			NewObjectAt(x, y);
		}
		
	}
}

bool quitting;
bool reverting;
bool blockfilling;

bool ProcessEvents(SDL_Event *ev)
{
	mapobject_t *obj_p;
	int dist;
	
	const unsigned char *state = SDL_GetKeyboardState(NULL);

	while (SDL_PollEvent(ev))
	{
		if (ev->type == SDL_QUIT)
			return false;

		if (ev->type == SDL_KEYDOWN)
		{
			if (ev->key.keysym.scancode >= 4 && ev->key.keysym.scancode < NUMOBJECTS+4) {
				if (!fillmode || !(objects_t)ev->key.keysym.scancode-4 == PORTAL)
					cursor.current = (objects_t)ev->key.keysym.scancode-4;
				return true;
			}
			
			if (state[SDL_SCANCODE_LSHIFT])
				dist = TILESIZE*8;
			else
				dist = TILESIZE;

			switch (ev->key.keysym.sym) {
					
				case SDLK_UP:
					MoveCursor(dist, up);
					break;
				case SDLK_DOWN:
					MoveCursor(dist, down);
					break;
				case SDLK_LEFT:
					MoveCursor(dist, left);
					break;
				case SDLK_RIGHT:
					MoveCursor(dist, right);
					break;
					
				case SDLK_1:
					SaveV2File();
					break;
					
				case SDLK_SLASH:
					if (cursor.current != PORTAL)
						fillmode = !fillmode;
					break;
					
				case SDLK_0:
					if (quitting)
						return false;
					else {
						quitting = true;
						return true;
					}
					
				case SDLK_2:
					if (reverting) {
//						LoadV1File();
						reverting = false;
					} else {
						reverting = true;
						return  true;
					}
					break;
					
				case SDLK_3:
					if (blockfilling) {
						obj_p = &tiledata[0][0];
						for (int i=0; i<ROWS*COLS; i++,obj_p++)
							obj_p->type = BLOCK;
					} else {
						blockfilling = true;
						return true;
					}
					break;
					
				case SDLK_MINUS:
					if (tag != 0)
						tag--;
					UpdateSetTagLabel();
					break;
					
				case SDLK_EQUALS:
					tag++;
					UpdateSetTagLabel();
					break;
					
					
				case SDLK_ESCAPE:
					gotfillpt = false;
					break;
					
				case SDLK_SPACE:
					
					obj_p = &tiledata[cursor.loc.y][cursor.loc.x];
					if (!fillmode) {
						
						if (cursor.current == EMPTY) {
							if
						} else {
							
						}
						
						UpdateCurrentTagLabel(tag);
					} else
						Fill();
					
					
					break;
				default:
					break;
			}
			quitting = false;
			reverting = false;
			blockfilling = false;
		}
	}
	return true;
}



void DisplayMenu(void)
{
	int i;
	SDL_Rect menuarea, infoarea;
	
	menuarea = SDLRect(MENUX, 0, MENUW, SCREENHEIGHT*DPI);
	infoarea = SDLRect(0, EDITORHEIGHT, SCREENWIDTH, INFOHEIGHT);
	
	// Fill Menu Area
	SDL_SetRenderDrawColor(renderer, 17, 17, 17, 255);
	SDL_RenderFillRect(renderer, &menuarea);
	SDL_RenderFillRect(renderer, &infoarea);

	for (i=0; i<NUMOBJECTS; i++) {
		if (i != 0)
			SDL_RenderCopy(renderer, images[i], NULL, &imagerects[i]);
		SDL_RenderCopy(renderer, labels[i].texture, NULL, &labelrects[i]);
	}

	for (i=0; i<NUMINFO; i++)
		SDL_RenderCopy(renderer, infolabels[i].texture, NULL, &inforects[i]);
}



void DisplayMap(void)
{
	SDL_Rect	dest;
	int 		row, col, i;
	mapobject_t		*data;
	mapobject_t	*obj_p;
	
	for (row=0; row<ROWS; row++) {
		for (col=0; col<COLS; col++) {
			data = &tiledata[row][col];
			dest = SDLRect(col*TILESIZE, row*TILESIZE, TILESIZE, TILESIZE);
			SDL_RenderCopy(renderer, images[data->type], NULL, &dest);
		}
	}
	
	
//	obj_p = &objects[0];
//	for (i=0; i<numobjects; i++, obj_p++)
//	{
//		dest = SDLRect(obj_p->x, obj_p->y, TILESIZE, TILESIZE);
//		SDL_RenderCopy(renderer, images[obj_p->type], NULL, &dest);
//	}
}



void DisplayFill(void)
{
	point_t 	fixedpt, dragpt;
	point_t 	dragorigin;
	sizetype	dragsize;
	SDL_Rect	dragrect, dest;
	int 		y,x;
	
	if (!fillmode && !gotfillpt)
		return;
	
	fixedpt = fillpoint[0];
	dragpt = MakePoint(cursor.loc.x, cursor.loc.y);

	dragorigin.x = (fixedpt.x <= dragpt.x) ? fixedpt.x : dragpt.x;
	dragorigin.y = (fixedpt.y <= dragpt.y) ? fixedpt.y : dragpt.y;
	dragsize.width = abs(dragpt.x-fixedpt.x)+TILESIZE;
	dragsize.height = abs(dragpt.y-fixedpt.y)+TILESIZE;
	dragrect = SDLRectFromPoint(dragorigin, dragsize);
	
//	for (y=dragorigin.y; y<dragsize.height-TILESIZE; y+=TILESIZE) {
//		for (x=dragorigin.x; x<dragsize.width-TILESIZE; x+=TILESIZE) {
//			dest = SDLRect(x, y, TILESIZE, TILESIZE);
//			SDL_RenderCopy(renderer, images[cursor.current], NULL, &dest);
//		}
//	}
	
	for (y=dragorigin.y; y<dragorigin.y+dragsize.height; y+=TILESIZE) {
		for (x=dragorigin.x; x<dragorigin.x+dragsize.width; x+=TILESIZE) {
			dest = SDLRect(x, y, TILESIZE, TILESIZE);
			SDL_RenderCopy(renderer, images[cursor.current], NULL, &dest);
		}
	}
	
	if (cursor.blink)
		DrawRectOutline(renderer, &dragrect, 2.0, vgacolor[red]);
	
}


void Display(void)
{
	SDL_Rect dest;
	
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
	
	DisplayMenu();
	DisplayMap();

	SDL_RenderCopy(renderer, settaglabel.texture, NULL, &settagrect);
	SDL_RenderCopy(renderer, currenttaglabel.texture, NULL, &currenttagrect);
	
	// display cursor

	if (fillmode && gotfillpt)
	{
		DisplayFill();
	}
	else // regular cursor
	{
		dest = SDLRect(cursor.loc.x, cursor.loc.y, TILESIZE, TILESIZE);
		if (cursor.current != 0)
			SDL_RenderCopy(renderer, images[cursor.current], NULL, &dest);

		if (cursor.blink) {
			if (fillmode) {
				DrawRectOutline(renderer, &dest, 2.0, vgacolor[red]);
			} else {
				DrawRectOutline(renderer, &dest, 2.0, vgacolor[brightwhite]);
			}
		}
	}
	
	
	SDL_RenderPresent(renderer);
}




int main(int argc, const char * argv[])
{
	bool newfile;
	SDL_Event ev;
	
	StartSDL();
	InitMenu();

	chdir("/Users/thomasfoster/Documents/snakedev/maps");
	
	if (argc == 3)
	{
		if (strcmp(argv[1], "-new") == 0) {
			strcpy(filename, argv[2]);
//			filepath = (char *)argv[2];
			newfile = true;
		} else if (strcmp(argv[1], "-open") == 0) {
//			filepath = (char *)argv[2];
			strcpy(filename, argv[2]);
			newfile = false;
		} else {
			printf("tileedit [-new or -open] filename\n");
		}
	}
	else
	{
		exit(1);
	}
	
	if (!newfile)
		if (!LoadV2File())
			printf("Readtiledata: Error! Could not read data from file\n");

	printf("editing %s\n",filename);

	tilesize = MakeSize(TILESIZE, TILESIZE);
	UpdateSetTagLabel();
	UpdateCurrentTagLabel(0);
	
	// init cursor
	cursor.loc = ZeroPoint();
	cursor.current = BLOCK;
	cursor.blink = 1;
	
	while (1)
	{
		ticks++;
		if (!ProcessEvents(&ev))
			break;
		
		if (ticks % BLINKSPEED == 0)
			cursor.blink ^= 1;
		
		Display();
		SDL_Delay(10);
	}
	
	FreeTextures();
	StopSDL();
	return 0;
}
