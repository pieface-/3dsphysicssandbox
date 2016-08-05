//Copyright (c) 2016 PieFace
//wallobj.h

#ifndef _wallobj_h
#define _wallobj_h

#include "mod_val.h"

typedef struct WALL_OBJ
{
	//0==horiz, 1==vert
	u8 active;
	u8 direction;
	_coord pos;
	u32 length;
} wall_obj;

#endif
