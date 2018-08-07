//
//  tffunctions.c
//  Pong
//
//  Created by Thomas Foster on 7/27/18.
//  Copyright © 2018 Thomas Foster. All rights reserved.
//

#include "tffunctions.h"

point_t MakePoint(int x, int y)
{
	point_t pt;
	
	pt.x = x;
	pt.y = y;
	return pt;
}

sizetype MakeSize(int w, int h)
{
	sizetype s;
	
	s.width = w;
	s.height = h;
	return s;
}

rect_t MakeRect(int x, int y, int w, int h)
{
	rect_t r;
	
	r.origin = MakePoint(x, y);
	r.size = MakeSize(w, h);
	return r;
}

point_t ZeroPoint(void)
{
	point_t pt = MakePoint(0, 0);
	return pt;
}

#pragma mark -

//
// TFRectToSDL
// Converts a TFRect to SDL_Rect
//
void tfRectToSDL(SDL_Rect *dest, rect_t *src)
{
	dest->x = src->origin.x;
	dest->y = src->origin.y;
	dest->w = src->size.width;
	dest->h = src->size.height;
}





void tfMakeBoxFromRect(box_t *destbox, rect_t const *srcrect)
{
	destbox->left = srcrect->origin.x;
	destbox->right = srcrect->origin.x + srcrect->size.width;
	destbox->top = srcrect->origin.y;
	destbox->bottom = srcrect->origin.y + srcrect->size.height;
}


void TFBoxFromSDLRect(box_t *destbox, SDL_Rect const *srcrect)
{
	destbox->left = srcrect->x;
	destbox->right = srcrect->x + srcrect->w;
	destbox->top = srcrect->y;
	destbox->bottom = srcrect->y + srcrect->h;
}




bool tfPointsAreEqual(point_t *pt1, point_t *pt2)
{
	if (pt1->x == pt2->x && pt1->y == pt2->y)
		return true;
	return false;
}




bool tfRectsCollide(rect_t *aRect, rect_t *bRect)
{
	box_t abox, bbox;
	bool xaligned, yaligned;
	
	tfMakeBoxFromRect(&abox, aRect);
	tfMakeBoxFromRect(&bbox, bRect);
	xaligned = !(abox.top > bbox.bottom || abox.bottom < bbox.top);
	yaligned = !(abox.left > bbox.right || abox.right < bbox.left);
	
	if (xaligned && yaligned)
		return true;
	
	return false;
}



//
//	DrawRectOutline
///	Makes an outline around a rect
//
void
DrawRectOutline
( SDL_Renderer *rend,
  SDL_Rect 	*rect,
  float 	linewidth,
  SDL_Color color )
{
	box_t 		box;
	SDL_Rect	rects[4];
	
	TFBoxFromSDLRect(&box, rect);
	rects[0] = (SDL_Rect) { box.left, box.top, rect->w, linewidth }; // top
	rects[1] = (SDL_Rect) { box.left, box.bottom-linewidth, rect->w, linewidth }; // bot
	rects[2] = (SDL_Rect) { box.left, box.top, linewidth, rect->h }; // left
	rects[3] = (SDL_Rect) { box.right-linewidth, box.top, linewidth, rect->h}; // right
	
	SDL_SetRenderDrawColor(rend, color.r, color.g, color.b, color.a);
	SDL_RenderFillRects(rend, rects, 4);
}



#pragma mark -

void ExtractFileName (char *path, char *dest)
{
	char		*src;
	
	src = path + strlen(path) - 1;
	
	// back up until a / or the start
	while (src != path && *(src-1) != '/')
		src--;
	
	strcpy (dest, src);
}


void StripFilename (char *path)
{
	char	*c;
	
	c = path + strlen (path);
	
	// search backwards for a slash
	do
	{
		if (--c == path)
			return;		// no period in path string
	} while (*c != '/');
	
	*c = 0;				// terminate the string there
}
