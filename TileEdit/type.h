
#ifndef type_h
#define type_h

typedef unsigned char byte;
typedef enum { false, true } bool;

typedef struct
{
	int width, height;
} sizetype;

typedef struct
{
	int x,y;
} point_t;

typedef struct
{
	point_t origin;
	sizetype size;
} rect_t;

typedef struct
{
	int right, left, top, bottom;
} box_t;

#endif /* type_h */
