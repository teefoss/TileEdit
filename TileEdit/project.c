//
//  project.c
//  TileEdit
//
//  Created by Thomas Foster on 9/15/18.
//  Copyright © 2018 Thomas Foster. All rights reserved.
//

#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "project.h"


#define FILEVERSION	4

char			filename[80];

texture_t		*textures;
projectinfo_t 	projinfo;


bool LoadV3File (char *filename)
{
	FILE *stream;
	int ver, i;
	int numobjects;
	mapobj_t obj;
	
	printf("\nLoadV#File: Loading file...\n");
	stream = fopen(filename, "r");
	
	
	// check version
	if (fscanf(stream, "TileEdit Version %d\n\n", &ver) != 1)
		return false;
	if (ver != FILEVERSION) {
		printf("Warning! File is v. %d, expected v. %d\n", ver, FILEVERSION);
		return false;
	}
	
	// get number of objects
	if (fscanf(stream, "numobjects:%d\n", &numobjects) != 1)
		return false;
	
	memset(objects, 0, sizeof(mapobj_t)*numobjects);
	
	printf("...loading %d objects...\n", numobjects);
	for (i=0; i<numobjects; i++)
	{
		if (fscanf(stream, "type:%d (%d, %d) tag:%d\n",
				   &obj.type, &obj.x, &obj.y, &obj.tag) != 4)
		{
			printf("...Error! Could not read object data. (line %d)\n", i);
			return false;
		}
		displaygrid[obj.y][obj.x] = obj; // load up the grid
	}
	
	printf("Success.\n\n");
	fclose(stream);
	return true;
}



//
//	SaveV2File
/// Write out tile data to current file
//
void SaveV3File (void)
{
	FILE 		*stream;
	int 		row, col;
	int			num;
	mapobj_t 	*obj_p;
	
	stream = fopen(filename, "w");
	
	// find number of objects
	num = 0;
#if 0
	obj_p = &displaygrid[0][0];
	for (i=0,num=0; i<projinfo.rows*projinfo.columns; i++, obj_p++) {
		if (obj_p->type != -1)
			num++;
	}
#endif
	for (row=0; row<projinfo.rows; row++)
		for (col=0 ; col<projinfo.columns; col++)
		{
			if (displaygrid[row][col].type != -1)
				num++;
		}
	
	fprintf(stream, "TileEdit Version %d\n\n", FILEVERSION);
	fprintf(stream, "numobjects:%d\n",num);
	
	for (row=0; row<projinfo.rows; row++)
	{
		for (col=0; col<projinfo.columns; col++)
		{
			obj_p = &displaygrid[row][col];
			//printf("(%d, %d) type is %d\n", row, col, obj_p->type);
			if (obj_p->type != -1)
				fprintf(stream, "type:%d (%d, %d) tag:%d action:%d\n",
						obj_p->type, col, row, obj_p->tag, obj_p->action);
		}
	}
	
	printf("V%d File Saved. (%d objects)\n",FILEVERSION, num);
	fclose(stream);
}





void
LoadProject
( int 		 argc,
  const char *argv[] )
{
	char 	projfilepath[80];
	char	workingdir[80];
	FILE	*stream;
	int		i, lcount, row, col;
	
	if (argc != 4)
	{
		puts("tedit [proj file] [-new or -load] [filename]\n");
		exit(1);
	}
	
	//printf("argv 1: %s", argv[1]);

	strcpy(projfilepath, argv[1]);
	strcpy(filename, argv[3]);
	
	printf("project file: %s\n", projfilepath);
	printf("tedit file: %s\n", filename);
	
//
// 	read in project info from file
//
	stream = fopen(projfilepath, "r");
	if (!stream)
	{
		printf("could not open project file!\n");
		exit(1);
	}
	
	
	if (fscanf(stream, "TileEdit Project File Ver. %d\n\n", &projinfo.version) != 1) {
		puts("LoadProject: Error! Could not read project file.\n");
		exit(1);
	}
	printf("Loading project file v %d...\n", projinfo.version);
	
	memset(&projinfo, 0, sizeof(projectinfo_t));
	
	if (fscanf(stream, "tilesize:%d rows:%d columns:%d\n\n",
			   &projinfo.tilesize, &projinfo.rows, &projinfo.columns) != 3)
	{
		puts("LoadProject: Error! Could not read project info.\n");
		exit(1);
	}
	
	// allocate display grid with number of rows and columns
	
	// disaster?
	displaygrid = malloc( sizeof(mapobj_t *) * projinfo.rows );
	for (i=0 ; i<projinfo.rows ; i++)
		displaygrid[i] = malloc( sizeof(mapobj_t) * projinfo.columns );
	
	for (row=0 ; row<projinfo.rows ; row++)
	for (col=0 ; col<projinfo.columns ; col++)
	{
		memset(&displaygrid[row][col], 0, sizeof(mapobj_t));
		displaygrid[row][col].type = -1; // -1 means empty tile
	}
	
	
	if (fscanf(stream, "project: %s\n", workingdir) != 1) {
		puts("LoadProject: Error! Could not read project path.\n");
		exit(1);
	}
	
	printf("project directory: %s\n", workingdir);
	chdir(workingdir);
	
	if (fscanf(stream, "numobjects: %d", &lcount) != 1)	{
		puts("LoadProject: Error! Could not read number of objects.\n");
		exit(1);
	}
	
	projinfo.listcount = lcount;
	projinfo.list = malloc(sizeof(objinfo_t)*lcount);
	
	for (i=0 ; i<lcount ; i++)
	{
		if (fscanf(stream, "%d : %s : %s\n",
			&projinfo.list[i].type, projinfo.list[i].name, projinfo.list[i].filename) != 3)
		{
			printf("LoadProject: Error! Could not object list item %d.\n",i);
			exit(1);
		}
	}
	fclose(stream);
	
//
//	Load textures
//
	//memset(textures, 0, sizeof(texture_t)*lcount);
	textures = malloc(sizeof(texture_t)*lcount);
	for (i=0 ; i<lcount; i++)
		textures[i] = LoadTextureFromFile(projinfo.list[i].filename);
	
//
// 	handle params
//
	if (!strcmp(argv[2], "-load"))
	{
		if (!LoadV3File(filename)) {
			puts("Couldn't open V3 file!\n");
			exit(1);
		}
	} else if (!strcmp(argv[2], "-new")) {
		printf("creating new file: %s...\n", filename);
		SaveV3File();
	}

	
}

