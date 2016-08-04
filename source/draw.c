//Copyright (c) 2016 PieFace
//draw.c

#include <math.h>
#include <sf2d.h>
#include "physobj.h"
#include "wallobj.h"
#include "mod_val.h"

void draw_phys_objs(phys_obj* objs, _coord camera)
{	
	for(u32 i = 0; objs[i].active; i++)
	{
		sf2d_draw_rectangle((s32)round(objs[i].pos.x-objs[i].length/2.0-camera.x), (s32)round(objs[i].pos.y-objs[i].length/2.0-camera.y), 
			objs[i].length, objs[i].length, RGBA8(0xFF, 0x00, 0xFF, 0xFF));

		sf2d_draw_rectangle((s32)round(objs[i].pos.x-objs[i].length/2.0-camera.x), (s32)round(objs[i].pos.y-objs[i].length/2.0-camera.y), 
			1, objs[i].length, RGBA8(0x00, 0x00, 0x00, 0xFF));

		sf2d_draw_rectangle((s32)round(objs[i].pos.x-objs[i].length/2.0-camera.x), (s32)round(objs[i].pos.y-objs[i].length/2.0-camera.y), 
			objs[i].length, 1, RGBA8(0x00, 0x00, 0x00, 0xFF));

		sf2d_draw_rectangle((s32)round(objs[i].pos.x-objs[i].length/2.0-camera.x), (s32)round(objs[i].pos.y+objs[i].length/2.0-camera.y)-1, 
			objs[i].length, 1, RGBA8(0x00, 0x00, 0x00, 0xFF));

		sf2d_draw_rectangle((s32)round(objs[i].pos.x+objs[i].length/2.0-camera.x)-1, (s32)round(objs[i].pos.y-objs[i].length/2.0-camera.y), 
			1, objs[i].length, RGBA8(0x00, 0x00, 0x00, 0xFF));
	}

}

void draw_wall_objs(wall_obj* walls, _coord camera)
{
	for(u32 i = 0; walls[i].active; i++)
	{
		if(walls[i].direction)
		{
			sf2d_draw_rectangle((s32)round(walls[i].pos.x-camera.x), (s32)round(walls[i].pos.y-camera.y), 
				1, walls[i].length, RGBA8(0x33, 0x33, 0x33, 0xFF));
		}
		else
		{
			sf2d_draw_rectangle((s32)round(walls[i].pos.x-camera.x), (s32)round(walls[i].pos.y-camera.y), 
				walls[i].length, 1, RGBA8(0x33, 0x33, 0x33, 0xFF));
		}

	}

}

