//Copyright (c) 2016 PieFace
//collision.h

#ifndef _collision_h
#define _collision_h


enum
{
	NO_COLLISION = 0,
	TOP_COLLISION,
	BOT_COLLISION,
	LEFT_COLLISION,
	RIGHT_COLLISION
};

enum
{
	VELOCITY_X,
	VELOCITY_Y
};

u8 detect_collision(phys_obj, phys_obj);
void calculate_collision(phys_obj, phys_obj, _mod_val);
void calculate_velocity(phys_obj, phys_obj, u8, double*, double*);

#endif
