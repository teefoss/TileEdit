//
//  loadsave.h
//  TileEdit
//
//  Created by Thomas Foster on 8/6/18.
//  Copyright © 2018 Thomas Foster. All rights reserved.
//

#ifndef loadsave_h
#define loadsave_h

#include <stdio.h>
#include "tffunctions.h"

extern char filename[80];

bool LoadV2File (void);
void SaveV2File (void);

bool LoadV1File (void);
void SaveV1File (void);
#endif /* loadsave_h */
