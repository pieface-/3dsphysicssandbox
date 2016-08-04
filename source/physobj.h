//Copyright (c) 2016 PieFace
//physobj.h

#ifndef _physobj_h
#define _physobj_h

#include "mod_val.h"

typedef struct PHYS_OBJ
{
	u8 active;
	_coord pos;
	_vector vel;
	u32 length;
} phys_obj;

#endif
