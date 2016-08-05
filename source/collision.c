//Copyright (c) 2016 PieFace
//collision.c
#include <3ds.h>
#include "mod_val.h"
#include "physobj.h"
#include <math.h>
#include "collision.h"
#include <stdio.h>

u8 detect_collision(phys_obj o1, phys_obj o2)
{
	double m = fabs((o2.pos.y-o1.pos.y)/(o2.pos.x-o1.pos.x));
	
	
	if(m>1)
	{
		if(o1.pos.x+o1.length/2.0>=o2.pos.x-o2.length/2.0 && o1.pos.x-o1.length/2.0<=o2.pos.x+o2.length/2.0)
		{
			if(o1.pos.y-o1.length/2.0<=o2.pos.y+o2.length/2.0 && o1.pos.y>o2.pos.y) 
			{
				return TOP_COLLISION;
			}
			else if(o2.pos.y-o2.length/2.0<=o1.pos.y+o1.length/2.0 && o2.pos.y>o1.pos.y)
			{
				return BOT_COLLISION;
			}
		}
	}
	else if(m<1)
	{
		if(o1.pos.y+o1.length/2.0>=o2.pos.y-o2.length/2.0 && o1.pos.y-o1.length/2.0<=o2.pos.y+o2.length/2.0)
		{
			if(o1.pos.x-o1.length/2.0<=o2.pos.x+o2.length/2.0 && o1.pos.x>o2.pos.x) 
			{
				return RIGHT_COLLISION;
			}
			else if(o2.pos.x-o2.length/2.0<=o1.pos.x+o1.length/2.0 && o2.pos.x>o1.pos.x)
			{
				return LEFT_COLLISION;
			}
		}
	}
	
	return NO_COLLISION;
}

void calculate_velocity(phys_obj o1, phys_obj o2, u8 vel_dir, double* v1, double* v2)
{
	double m1 = o1.length/50.0*o1.length/50.0;
	double m2 = o2.length/50.0*o2.length/50.0;
	
	if(vel_dir == VELOCITY_X)
	{
		*v1 = (m1-m2)/(m1+m2)*o1.vel.x + (2*m2)/(m1+m2)*o2.vel.x;
		*v2 = (2*m1)/(m1+m2)*o1.vel.x + (m1-m2)/(m1+m2)*o2.vel.x;
	}
	else if(vel_dir == VELOCITY_Y)
	{
		*v1 = (m1-m2)/(m1+m2)*o1.vel.y + (2*m2)/(m1+m2)*o2.vel.y;
		*v2 = (2*m1)/(m1+m2)*o1.vel.y + (m1-m2)/(m1+m2)*o2.vel.y;
	}

}

void calculate_collision(phys_obj o1, phys_obj o2, _mod_val mod_val)
{
	//unused currently	





}
