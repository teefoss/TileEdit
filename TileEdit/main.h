//
//  main.h
//  TileEdit
//
//  Created by Thomas Foster on 8/5/18.
//  Copyright © 2018 Thomas Foster. All rights reserved.
//

#ifndef main_h
#define main_h

#define MAPMULTIPLIER	16
//#define TILESIZE		16*DPI
#define TILESIZE		16

// the tile editing area
#define EDITORWIDTH		640*DPI
#define EDITORHEIGHT	480*DPI
#define INFOHEIGHT		80
#define BLINKSPEED		50

#define MENUX			640*DPI
#define MENUW			192*DPI
#define LABELSX			MENUX+TILESIZE*2

#define ROWS			30	// y!
#define COLS			40	// x!
#define MAPSIZE			COLS*ROWS



typedef enum
{
	EMPTY,
	BLOCK,
	PORTAL,
	START,
	APPLE,
	NUMOBJECTS
} objects_t;


typedef struct
{
	int type;
	int x, y;
	int tag;
} mapobject_t;

extern int numobjects;
extern mapobject_t objects[1200];
extern mapobject_t tiledata[ROWS][COLS];

typedef struct
{
	point_t 	loc;
	objects_t	current;
	int			blink;
} cursor_t;


#endif /* main_h */
