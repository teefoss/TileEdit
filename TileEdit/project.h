//
//  project.h
//  TileEdit
//
//  Created by Thomas Foster on 9/15/18.
//  Copyright © 2018 Thomas Foster. All rights reserved.
//

#ifndef project_h
#define project_h

#include "main.h"
#include "sys.h"

typedef struct
{
	int 	type;
	char 	name[9];
	char 	filename[80];
} objinfo_t;

typedef struct
{
	int			version;
	int 		tilesize;
	int 		rows, columns;
	int 		listcount;
	objinfo_t 	*list;
} projectinfo_t;

extern projectinfo_t 	projinfo;
extern texture_t		*textures;

void SaveV3File (void);
void LoadProject (int argc, const char *argv[]);

#endif /* project_h */
