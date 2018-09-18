//
//  main.c
//  TileEdit
//
//  Created by Thomas Foster on 8/2/18.
//  Copyright © 2018 Thomas Foster. All rights reserved.
//

#include <stdio.h>
#include "sys.h"
#include <unistd.h>

#include "main.h"
#include "project.h"

mapobj_t 	*objects;

// 	array[i][j][k] will be translated to
//	*( array + i*20*30 + j*30 + k )
mapobj_t	**displaygrid;

unsigned 	ticks = 0;
cursor_t	cursor;
sizetype	tilesize;

bool		fillmode;
bool		clearmode;
point_t		fillpoint[2];
bool		gotfillpt;
int 		tag = 0;

#define NUMINFO 4

const char *infonames[NUMINFO] = {
	"/  Toggle Fill Mode",
	"1  Save",
	"22 Revert",
	"00 Quit without Saving"
};

typedef struct
{
	texture_t 	text;
	SDL_Rect	textrect;
	texture_t	image;
	SDL_Rect	imgrect;
} label_t;

#define LISTMAX	30

label_t objlabels[LISTMAX];
label_t infolabels[NUMINFO];

void InitMenu(void)
{
	int i;
	
	// object labels / images
	for (i=0; i<projinfo.listcount; i++) {
		objlabels[i].text = CreateText(projinfo.list[i].name, vgacolor[white]);
		objlabels[i].textrect = (SDL_Rect) { LABELSX, i*TILESIZE, objlabels[i].text.size.width, objlabels[i].text.size.height };
		objlabels[i].image = textures[i];
		objlabels[i].imgrect = (SDL_Rect) { EDITORWIDTH, i*TILESIZE, TILESIZE, TILESIZE};
	}
	
	// info list at bottom
	for (i=0; i<NUMINFO; i++) {
		infolabels[i].text = CreateText(infonames[i], vgacolor[yellow]);
		infolabels[i].textrect = (SDL_Rect) { 0, EDITORHEIGHT+(i*TILESIZE), infolabels[i].text.size.width, infolabels[i].text.size.height};
	}
}

texture_t	settaglabel;
texture_t	currenttaglabel;
texture_t	positionlabel;

SDL_Rect	settagrect;
SDL_Rect	currenttagrect;
SDL_Rect	positionrect;

void UpdateSetTagLabel (void)
{
	char 		text[32];
	int 		tagx;
	
	sprintf(text, "Set Tag: %d", tag);
	settaglabel = CreateText(text, vgacolor[white]);
	
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
	
	sprintf(text, "Current Tag: %d", tag);
	currenttaglabel = CreateText(text, vgacolor[white]);
	
	tagx = 640-currenttaglabel.size.width;
	currenttagrect = SDLRect(tagx, 480+TILESIZE, currenttaglabel.size.width, currenttaglabel.size.height);
}


void UpdatePositionLabel (void)
{
	char 	text[32];
	int 	x;
	
	sprintf(text, "(%d, %d)", cursor.loc.x/TILESIZE, cursor.loc.y/TILESIZE);
	positionlabel = CreateText(text, vgacolor[white]);
	x = EDITORWIDTH-positionlabel.size.width;
	positionrect = SDLRect(x, EDITORHEIGHT+TILESIZE*2, positionlabel.size.width, positionlabel.size.height);
}




void FreeEditor (void)
{
	int i;
	
	for (i=0; i<projinfo.listcount; i++)
	{
		SDL_DestroyTexture(objlabels[i].text.texture);
		SDL_DestroyTexture(objlabels[i].image.texture);
	}
	
	for (i=0; i<NUMINFO; i++)
		SDL_DestroyTexture(infolabels[i].text.texture);
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
	
	if (!gotfillpt) {
		fillpoint[0] = MakePoint(cursor.loc.x, cursor.loc.y);
		gotfillpt = true;
		return;
	}
	
	fillpoint[1] = MakePoint(cursor.loc.x, cursor.loc.y);
	
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
			if (clearmode) {
				displaygrid[starty][x].type = -1;
				displaygrid[starty][x].tag = -1;
			} else {
				displaygrid[starty][x].type = cursor.obj;
			}
		} while (x++ < stopx);
	} while (starty++ < stopy);
	
	gotfillpt = false;
}





typedef enum { up, down, left, right } direction_t;

void MoveCursor(int dist, direction_t dir)
{
	mapobj_t *obj_p;
	
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
	//PrintCursorLocation();
	UpdatePositionLabel();
	
	obj_p = &displaygrid[cursor.loc.y/TILESIZE][cursor.loc.x/TILESIZE];
	UpdateCurrentTagLabel(obj_p->tag);
}



bool quitting;
bool reverting;

bool ProcessEvents(SDL_Event *ev)
{
	mapobj_t *obj_p;
	int dist;
	
	const unsigned char *state = SDL_GetKeyboardState(NULL);

	while (SDL_PollEvent(ev))
	{
		if (ev->type == SDL_QUIT)
			return false;

		if (ev->type == SDL_KEYDOWN)
		{
			if (ev->key.keysym.scancode >= 4 && ev->key.keysym.scancode < projinfo.listcount+4)
			{
				cursor.obj = ev->key.keysym.scancode-4;
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
					SaveV3File();
					break;
					
				case SDLK_SLASH:
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
					fillmode = false;
					clearmode = false;
					break;
					
				case SDLK_x:
					obj_p = &displaygrid[cursor.loc.y/TILESIZE][cursor.loc.x/TILESIZE];
					if (state[SDL_SCANCODE_LSHIFT]) {
						fillmode = !fillmode;
						clearmode = true;
					}
					else
						obj_p->type = -1;
					break;
					
				case SDLK_SPACE:
					
					obj_p = &displaygrid[cursor.loc.y/TILESIZE][cursor.loc.x/TILESIZE];
					if (!fillmode) {
						if (obj_p->type == cursor.obj) {
							obj_p->type = -1; // remove
						} else {
							obj_p->type = cursor.obj;
							obj_p->tag = tag;
						}
					} else
						Fill();
					
					
					break;
				default:
					break;
			}
			quitting = false;
			reverting = false;
		}
	}
	return true;
}



#pragma mark -

void DisplayMenu(void)
{
	int i;
	SDL_Rect menuarea, infoarea;
	
	menuarea = SDLRect(EDITORWIDTH, 0, SCREENWIDTH-EDITORWIDTH, SCREENHEIGHT);
	infoarea = SDLRect(0, EDITORHEIGHT, SCREENWIDTH, INFOHEIGHT);
	
	// Fill Menu Area
	SDL_SetRenderDrawColor(renderer, 32, 32, 32, 255);
	SDL_RenderFillRect(renderer, &menuarea);
	SDL_RenderFillRect(renderer, &infoarea);

	for (i=0; i<projinfo.listcount; i++)
	{
		SDL_RenderCopy(renderer, objlabels[i].text.texture, NULL, &objlabels[i].textrect);
		SDL_RenderCopy(renderer, objlabels[i].image.texture, NULL, &objlabels[i].imgrect);
	}

	for (i=0; i<NUMINFO; i++)
		SDL_RenderCopy(renderer, infolabels[i].text.texture, NULL, &infolabels[i].textrect);
}



void DisplayMap(void)
{
	SDL_Rect	dest;
	int 		row, col;
	mapobj_t	*obj_p;
	
	for (row=0; row<projinfo.rows; row++) {
		for (col=0; col<projinfo.columns; col++)
		{
			obj_p = &displaygrid[row][col];
			
			if (obj_p->type == -1)
				continue; // empty tile
			
			dest = SDLRect(col*TILESIZE, row*TILESIZE, TILESIZE, TILESIZE);
			SDL_RenderCopy(renderer, textures[obj_p->type].texture, NULL, &dest);
		}
	}
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

	if (!clearmode) {
		for (y=dragorigin.y; y<dragorigin.y+dragsize.height; y+=TILESIZE) {
			for (x=dragorigin.x; x<dragorigin.x+dragsize.width; x+=TILESIZE) {
				dest = SDLRect(x, y, TILESIZE, TILESIZE);
				SDL_RenderCopy(renderer, textures[cursor.obj].texture, NULL, &dest);
			}
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
	SDL_RenderCopy(renderer, positionlabel.texture, NULL, &positionrect);
	
	// CURSOR

	if (fillmode && gotfillpt)
	{
		DisplayFill();
	}
	else // regular cursor
	{
		dest = SDLRect(cursor.loc.x, cursor.loc.y, TILESIZE, TILESIZE);
		if (!clearmode)
			SDL_RenderCopy(renderer, textures[cursor.obj].texture, NULL, &dest);

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


#pragma mark -

// IT BEGINS

int
main
( int		 argc,
  const char *argv[] )
{
	SDL_Event ev;
	
	StartSDL();
	LoadProject(argc, argv);
	InitMenu();

//	chdir("/Users/thomasfoster/Documents/snakedev/maps");
	
	tilesize = MakeSize(projinfo.tilesize, projinfo.tilesize);
	UpdateSetTagLabel();
	UpdateCurrentTagLabel(0);
	UpdatePositionLabel();
	
	// init cursor
	cursor.loc = MakePoint(EDITORWIDTH/2, EDITORHEIGHT/2);
	cursor.obj = 0;
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
	
	free(projinfo.list);
	
	for (int i=0 ; i<projinfo.rows ; i++)
		free(displaygrid[i]); // ??
	free(displaygrid);

	FreeEditor();
	StopSDL();
	return 0;
}
