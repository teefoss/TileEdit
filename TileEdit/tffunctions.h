//
//  tffunctions.h
//  Pong
//
//  Created by Thomas Foster on 7/27/18.
//  Copyright © 2018 Thomas Foster. All rights reserved.
//

#ifndef tffunctions_h
#define tffunctions_h

#include <stdio.h>
#include <SDL2/SDL.h>
#include "type.h"

point_t 	MakePoint(int x, int y);
sizetype 	MakeSize(int w, int h);
rect_t 		MakeRect(int x, int y, int w, int h);
point_t 	ZeroPoint(void);

void tfRectToSDL(SDL_Rect *sdlrect, rect_t *tfrect);
void tfMakeBoxFromRect(box_t *destbox, rect_t const *srcrect);
bool tfPointsAreEqual(point_t *pt1, point_t *pt2);

bool tfRectsCollide(rect_t *aRect, rect_t *bRect);

void ExtractFileName (char *path, char *dest);
void StripFilename (char *path);

void DrawRectOutline (SDL_Renderer *rend, SDL_Rect *rect, float linewidth, SDL_Color color);

#endif /* tffunctions_h */
