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
	
	printf("\x1b[15;0H%f",m);
	
	if(m>1)
	{
		printf("\x1b[16;0Htest%d",1);
		if(o1.pos.x+o1.length/2.0>=o2.pos.x-o2.length/2.0 && o1.pos.x-o1.length/2.0<=o2.pos.x+o2.length/2.0)
		{
			printf("\x1b[16;0Htest%d",2);
			if(o1.pos.y-o1.length/2.0<=o2.pos.y+o2.length/2.0 && o1.pos.y>o2.pos.y) 
			{
				printf("\x1b[16;0Htest%d",3);
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
		printf("\x1b[16;0Htest%d",4);
		if(o1.pos.y+o1.length/2.0>=o2.pos.y-o2.length/2.0 && o1.pos.y-o1.length/2.0<=o2.pos.y+o2.length/2.0)
		{
			printf("\x1b[16;0Htest%d",5);
			if(o1.pos.x-o1.length/2.0<=o2.pos.x+o2.length/2.0 && o1.pos.x>o2.pos.x) 
			{
				printf("\x1b[16;0Htest%d",6);
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

void calculate_collision(phys_obj o1, phys_obj o2, _mod_val mod_val)
{
	





}
