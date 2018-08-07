//
//  loadsave.c
//  TileEdit
//
//  Created by Thomas Foster on 8/6/18.
//  Copyright © 2018 Thomas Foster. All rights reserved.
//

#include "loadsave.h"
#include "main.h"

#define VERSION	2

char filename[80];

bool LoadV2File (void)
{
	FILE *stream;
	int ver, i;
	mapobject_t	obj;

	printf("\nLoadV2File: Loading file...\n");
	stream = fopen(filename, "r");
	memset(objects, 0, sizeof(mapobject_t)*1200);
	memset(tiledata, 0, sizeof(mapobject_t)*1200);

	// check version
	if (fscanf(stream, "TileEdit Version %d\n\n", &ver) != 1)
		return false;
	if (ver != 2) {
		printf("Warning! File is v. %d, expected v. %d\n", ver, 2);
		return false;
	}
	
	// get number of objects
	if (fscanf(stream, "numobjects:%d\n", &numobjects) != 1)
		return false;
	
	printf("...loading %d objects...\n", numobjects);
	for (i=0; i<numobjects; i++)
	{
		if (fscanf(stream, "type:%d (%d, %d) tag:%d\n",
			   &obj.type, &obj.x, &obj.y, &obj.tag) != 4)
		{
			printf("...Error! Could not read object data. (line %d)\n", i);
			return false;
		}
		
		tiledata[obj.y][obj.x] = obj;
	}
	
	printf("Success.\n");
	fclose(stream);
	return true;
}

#if 0
bool LoadV1File (void)
{
	FILE *stream;
	int ver;
	int rows,cols;
	int row,col;
	
	stream = fopen(filename, "r");
	memset(&tiledata, 0, sizeof(byte)*ROWS*COLS);
	
	if (fscanf(stream, "TileEdit Version %d\n", &ver) != 1)
		return false;
	if (ver != 1) {
		printf("Readtiledata: Warning! File is v. %d, expected v. %d\n", ver, 1);
		return false;
	}
	
	if (fscanf(stream, "\nrows:%d\ncols:%d\n", &rows, &cols) != 2)
		return false;
	if (rows != ROWS || cols != COLS) {
		printf("Readtiledata: Warning! File has rows: %d, cols: %d. Should be: %d, %d\n",
			   rows,cols,ROWS,COLS);
		return false;
	}
	
	for (row=0; row<ROWS; row++)
		for (col=0; col<COLS; col++)
			if (fscanf(stream, "%hhu\n", &tiledata[row][col]) != 1)
				return false;
	
	printf("Loaded Map File\n");
	fclose(stream);
	
	// Load objects from tiledata;
	memset(objects, 0, sizeof(mapobject_t)*1200);
	int index = 0;
	byte *data;
	for (row=0; row<ROWS; row++) {
		for (col=0; col<COLS; col++)
		{
			data = &tiledata[row][col];
			
			if (!*data)
				continue;
			
			if (*data >= 100)
				objects[index].type = PORTAL;
			else
				objects[index].type = *data;
			
			objects[index].x = col*TILESIZE;
			objects[index].y = row*TILESIZE;
			objects[index++].tag = 0;
		}
	}
	numobjects = index;
	
	int i;
	for (i=0; i<numobjects; i++)
	{
		printf("Obj: %d type: %d (%d, %d) tag: %d\n",
			   i, objects[i].type, objects[i].x, objects[i].y,objects[i].tag);
	}

	return true;
}




//
//  SaveV1File
///	Write out tile data to current file
//
void SaveV1File (void)
{
	FILE *stream;
	int row, col;
	
	stream = fopen(filename, "w");
	
	fprintf(stream, "TileEdit Version %d\n", 1);
	fprintf(stream, "\nrows:%d\ncols:%d\n",ROWS,COLS);
	for (row=0; row<ROWS; row++)
		for (col=0; col<COLS; col++)
			fprintf(stream, "%hhu\n", tiledata[row][col]);
	
	printf("File Saved.\n");
	fclose(stream);
}
#endif


//
//	SaveV2File
/// Write out tile data to current file
//
void SaveV2File (void)
{
	FILE 		*stream;
	int 		row, col;
	mapobject_t *obj_p;
	
	stream = fopen(filename, "w");

	/*
	for (row=0; row<ROWS; row++) {
		for (col=0; col<ROWS; col++) {
			obj_p = &tiledata[row][col];
			
			if (obj_p->type == 0 && )
		}
	}
	 */

	fprintf(stream, "TileEdit Version %d\n\n", 2);
	fprintf(stream, "numobjects:%d\n",numobjects);
	
	for (row=0; row<ROWS; row++) {
		for (col=0; col<ROWS; col++) {
			obj_p = &tiledata[row][col];
			if (obj_p->type || obj_p->tag)
				fprintf(stream, "type:%d (%d, %d) tag:%d\n",
						obj_p->type, col, row, obj_p->tag);
		}
	}
	
//	obj_p = &objects[0];
//	for (i=0; i<numobjects; i++, obj_p++)
//	{
//		fprintf(stream, "type:%d (%d, %d) tag:%d\n",
//			   obj_p->type, obj_p->x, obj_p->y, obj_p->tag);
//	}
	
	printf("V2 File Saved.");
	fclose(stream);
}
