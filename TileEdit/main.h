//
//  main.h
//  TileEdit
//
//  Created by Thomas Foster on 8/5/18.
//  Copyright © 2018 Thomas Foster. All rights reserved.
//

#ifndef main_h
#define main_h

#include "tffunctions.h"

#define MAPMULTIPLIER	16
#define TILESIZE		16

// the tile editing area
#define EDITORWIDTH		640
#define EDITORHEIGHT	480
#define INFOHEIGHT		80
#define BLINKSPEED		50

#define LABELSX			EDITORWIDTH+(TILESIZE*2)

//#define ROWS			30	// y!
//#define COLS			40	// x!
//#define MAPSIZE			COLS*ROWS

typedef struct
{
	int x, y;
	int type;
	int tag;
	int action;
} mapobj_t;

// old, remove
#if 0
typedef struct
{
	int type;
	int x, y;
	int tag;
} maptile_t;
#endif

extern mapobj_t *objects;
extern mapobj_t	**displaygrid;
typedef struct
{
	point_t 	loc;
	int			obj;
	int			blink;
} cursor_t;


#endif /* main_h */
