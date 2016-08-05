//Copyright (c) 2016 PieFace
//main.c

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <3ds.h>
#include <sf2d.h>
#include "screenshot.h"
#include "mod_val.h"
#include "draw.h"
#include "physobj.h"
#include "wallobj.h"
#include "collision.h"

#define CONFIG_3D_SLIDERSTATE (*(float *)0x1FF81080)
#define debug

//forward declarations
char* get_current_string(u8);
char* get_gravity_from_accel_string(u8);

FS_Archive sdmc;

int main()
{
	sdmc = (FS_Archive){ARCHIVE_SDMC, (FS_Path){PATH_EMPTY, 1, (u8*)""}};
	FSUSER_OpenArchive(&sdmc);

	// Set the random seed based on the time
	srand(time(NULL));

	//init screen background to white
	sf2d_init();
	sf2d_set_clear_color(RGBA8(0xFF, 0xFF, 0xFF, 0xFF));
	sf2d_set_3D(0);
	sf2d_set_vblank_wait(1);


	_mod_val def = 

	{
		0,
		0,
		{320/2, 240/2},
		1.0,
		-1,
		{0, 0},
		{0.0, -10.0},
		{0.0, -10.0},
		.5,
		.5,
		25.0,
		{0, 0},
		0,
		0,
		0
	};

	s16 deadzone = 25;

	_mod_val mod_val = def;

	phys_obj* objs = (phys_obj*) calloc(32, sizeof(phys_obj));
	wall_obj* walls = (wall_obj*) calloc(32, sizeof(wall_obj));
	
	wall_obj wall_null = {};
	phys_obj obj_null = {};


	phys_obj obj0 = {1, {100,100}, {0,0}, 50};

	phys_obj obj1 = {1, {200, 100}, {0,-250}, 20};

	phys_obj obj2 = {1, {150, 50}, {00,0}, 80};


	wall_obj wobj0 = {1, 0, {0,0}, 320};
	wall_obj wobj1 = {1, 1, {0,0}, 240};
	wall_obj wobj2 = {1, 0, {0,239}, 320};
	wall_obj wobj3 = {1, 1, {319,0}, 240};
	
	walls[2] = wobj0;
	walls[1] = wobj1;
	walls[0] = wobj2;
	walls[3] = wobj3;

	objs[0] = obj0;
	objs[1] = obj1;
	objs[2] = obj2;

	//HID variables
	touchPosition touch;
	circlePosition circle;
	accelVector accel;
	u32 held;
	u32 pressed;
	u32 released;

	HIDUSER_EnableAccelerometer();
	
	PrintConsole top, global_info, obj_info, controls;
	
	consoleInit(GFX_TOP, &top);
	consoleInit(GFX_TOP, &global_info);
	consoleInit(GFX_TOP, &obj_info);
	consoleInit(GFX_TOP, &controls);
	
	consoleSetWindow(&global_info, 0, 3, 25, 12);
	consoleSetWindow(&obj_info, 25, 3, 25, 12);
	consoleSetWindow(&controls, 0, 20, 50, 10);

	//gfxSetScreenFormat(GFX_TOP, 1);

	consoleSelect(&top);
	printf("Physics Sandbox v0.3 by PieFace\n\n");
	
	consoleSelect(&controls);
	printf("Controls:\n");
	printf("Tap and drag square to move it\n");
	printf("X - Reset to default\n");
	printf("B - Teleport selected square to center\n");
	printf("DPad L&R - Change between mod_vals\n");
	printf("DPad U&D - Increment and decrement mod_val\n");
	printf("L&R - Change increment (pow of 10)\n");
	printf("Y - Toggle gravity from gyroscope\n");
	printf("Select - Save screenshot\n");	
	printf("Start - Exit");
	
	consoleSelect(&top);
	printf("Global Info              Object Info");
	
	
	while (aptMainLoop()) {
	
		consoleSelect(&top);

		//gather inputs
		hidScanInput();
		hidCircleRead(&circle);
		held = hidKeysHeld();
		pressed = hidKeysDown();
		released = hidKeysUp();

		hidAccelRead(&accel);

		//move camera
		if(abs(circle.dx)>= deadzone)
		{
			mod_val.camera.x+=circle.dx/60.0;
			if(mod_val.mov_rect!=-1) mod_val.offset.x+=circle.dx/60.0;
		}

		if(abs(circle.dy)>= deadzone)
		{
			mod_val.camera.y-=circle.dy/60.0;
			if(mod_val.mov_rect!=-1) mod_val.offset.y-=circle.dy/60.0;
		}


		//check button presses

		//Press Start to Exit
		if (held & KEY_START) 
		{
			break;
		}
		
		if (pressed & KEY_SELECT)
		{
			save_screenshot();
		}

		//Read touch values if the screen is pressed
		if (held & KEY_TOUCH) 
		{
			hidTouchRead(&mod_val.touch);
			if(mod_val.mov_rect!=-1)
			{
				objs[mod_val.mov_rect].vel.x = ((double)mod_val.touch.px + mod_val.offset.x - objs[mod_val.mov_rect].pos.x)*60.0;
				objs[mod_val.mov_rect].vel.y = ((double)mod_val.touch.py + mod_val.offset.y - objs[mod_val.mov_rect].pos.y)*60.0;
			}
		}
		
		if(pressed & KEY_B)
		{
			if(!mod_val.obj_type)
			{
				objs[mod_val.curr_view].pos = (_coord){160,120};
				objs[mod_val.curr_view].vel = (_vector){0,0};
			}		
		}

		if(pressed & KEY_Y)
		{
			if(mod_val.gravity_from_accel==0)
			{
				mod_val.gravity_from_accel = 1;
				mod_val.gravity_b = mod_val.gravity;
			}			
			else
			{
				mod_val.gravity_from_accel = 0;
				mod_val.gravity = mod_val.gravity_b;
			}		
		}

		//DPAD up - Increment current mod_val
		if(pressed & KEY_DUP)
		{
			if(mod_val.current == 0)
			{
				if(mod_val.gravity_from_accel)
				{
					mod_val.gravity_b.y += mod_val.inc;
				}
				else
				{
					mod_val.gravity.y += mod_val.inc;
				}
			}
			
			if(mod_val.current == 1)
			{
				if(mod_val.gravity_from_accel)
				{
					mod_val.gravity_b.x += mod_val.inc;
				}
				else
				{
					mod_val.gravity.x += mod_val.inc;
				}
			}

			if(mod_val.current == 2)
			{
				mod_val.elasticity += mod_val.inc;
				if(mod_val.elasticity > 1) mod_val.elasticity = 1;
			}

			if(mod_val.current == 3)
			{
				mod_val.friction += mod_val.inc;
				if(mod_val.friction > 1) mod_val.friction = 1;
			}
			
			if(mod_val.current == 4)
			{
				mod_val.scope++;
				if(mod_val.scope>1) mod_val.scope = 0;
			}
			
			if(mod_val.current == 5)
			{
				mod_val.obj_type ++;
				if(mod_val.obj_type>1) mod_val.obj_type = 0;
				
				consoleSelect(&obj_info);
				consoleClear();
				consoleSelect(&top);
			}

			if(mod_val.current == 6)
			{
				mod_val.curr_view ++;
				if(mod_val.curr_view > 31) mod_val.curr_view = 0;
			}
			
			if(mod_val.current == 7)
			{
				if(mod_val.obj_type)
				{
					walls[mod_val.curr_view].active++;
					if(walls[mod_val.curr_view].active>1) walls[mod_val.curr_view].active=0;
				}
				else
				{
					objs[mod_val.curr_view].active++;
					if(objs[mod_val.curr_view].active>1) objs[mod_val.curr_view].active=0;
				}
			}
			
			if(mod_val.current == 8)
			{
				if(mod_val.obj_type)
				{
					walls[mod_val.curr_view].pos.x += mod_val.inc;
				}
				else
				{
					objs[mod_val.curr_view].pos.x += mod_val.inc;
				}			
			
			}
			
			if(mod_val.current == 9)
			{
				if(mod_val.obj_type)
				{
					walls[mod_val.curr_view].pos.y += mod_val.inc;
				}
				else
				{
					objs[mod_val.curr_view].pos.y += mod_val.inc;
				}			
			
			}
			
			if(mod_val.current == 10)
			{
				walls[mod_val.curr_view].direction++;
				if(walls[mod_val.curr_view].direction > 1) walls[mod_val.curr_view].direction = 0;
			
			}
			
			if(mod_val.current == 11)
			{
				objs[mod_val.curr_view].vel.x += mod_val.inc;
			}
			
			if(mod_val.current == 12)
			{
				objs[mod_val.curr_view].vel.y += mod_val.inc;
			}
			
			if(mod_val.current == 13)
			{
				if(mod_val.obj_type)
				{
					walls[mod_val.curr_view].length += mod_val.inc;
				}
				else
				{
					objs[mod_val.curr_view].length += mod_val.inc;
				}
			}
		}
		
		//DPAD down - decrement current mod_val.current
		if(pressed & KEY_DDOWN)
		{
			if(mod_val.current == 0)
			{
				if(mod_val.gravity_from_accel)
				{
					mod_val.gravity_b.y -= mod_val.inc;
				}
				else
				{
					mod_val.gravity.y -= mod_val.inc;
				}
			}
			
			if(mod_val.current == 1)
			{
				if(mod_val.gravity_from_accel)
				{
					mod_val.gravity_b.x -= mod_val.inc;
				}
				else
				{
					mod_val.gravity.x -= mod_val.inc;
				}
			}

			if(mod_val.current == 2)
			{
				mod_val.elasticity -= mod_val.inc;
				if(mod_val.elasticity < 0) mod_val.elasticity = 0;
			}

			if(mod_val.current == 3)
			{
				mod_val.friction -= mod_val.inc;
				if(mod_val.friction < 0) mod_val.friction = 0;
			}

			if(mod_val.current == 4)
			{
				mod_val.scope--;
				if(mod_val.scope>1) mod_val.scope = 1;
			}
			
			if(mod_val.current == 5)
			{
				mod_val.obj_type --;
				if(mod_val.obj_type>1) mod_val.obj_type = 1;
				
				consoleSelect(&obj_info);
				consoleClear();
				consoleSelect(&top);
			}

			if(mod_val.current == 6)
			{
					mod_val.curr_view --;
					if(mod_val.curr_view > 31) mod_val.curr_view = 31;
			}
			
			if(mod_val.current == 7)
			{
				if(mod_val.obj_type)
				{
					walls[mod_val.curr_view].active--;
					if(walls[mod_val.curr_view].active>1) walls[mod_val.curr_view].active=1;
				}
				else
				{
					objs[mod_val.curr_view].active--;
					if(objs[mod_val.curr_view].active>1) objs[mod_val.curr_view].active=1;
				}
			}
			
			if(mod_val.current == 8)
			{
				if(mod_val.obj_type)
				{
					walls[mod_val.curr_view].pos.x -= mod_val.inc;
				}
				else
				{
					objs[mod_val.curr_view].pos.x -= mod_val.inc;
				}			
			
			}
			
			if(mod_val.current == 9)
			{
				if(mod_val.obj_type)
				{
					walls[mod_val.curr_view].pos.y -= mod_val.inc;
				}
				else
				{
					objs[mod_val.curr_view].pos.y -= mod_val.inc;
				}			
			
			}
			
			if(mod_val.current == 10)
			{
				walls[mod_val.curr_view].direction--;
				if(walls[mod_val.curr_view].direction > 1) walls[mod_val.curr_view].direction = 1;
			
			}
			
			if(mod_val.current == 11)
			{
				objs[mod_val.curr_view].vel.x -= mod_val.inc;
			}
			
			if(mod_val.current == 12)
			{
				objs[mod_val.curr_view].vel.y -= mod_val.inc;
			}
			
			if(mod_val.current == 13)
			{
				if(mod_val.obj_type)
				{
					walls[mod_val.curr_view].length -= mod_val.inc;
				}
				else
				{
					objs[mod_val.curr_view].length -= mod_val.inc;
				}
			}
		}

		//DPAD Left and Right - Change mod_val.current
		if(pressed & KEY_DLEFT)
		{
			mod_val.current--;
			if(mod_val.scope)
			{
				if (mod_val.current < 4) mod_val.current = 13;
			}
			else
			{
				if (mod_val.current > 4) mod_val.current = 4;
			}
			
			if(mod_val.current==10 && mod_val.obj_type==0) mod_val.current = 9;
			if(mod_val.current==12 && mod_val.obj_type==1) mod_val.current = 10;
		}
		
		if(pressed & KEY_DRIGHT)
		{
			mod_val.current++;
			if(mod_val.scope)
			{
				if (mod_val.current > 13) mod_val.current = 4;
			}
			else
			{
				if (mod_val.current > 4) mod_val.current = 0;
			}
			if(mod_val.current==10 && mod_val.obj_type==0) mod_val.current = 11;
			if(mod_val.current==11 && mod_val.obj_type==1) mod_val.current = 13;
		}
		

		//L and R - Multiply and divide increment by 10
		if(pressed & KEY_L) mod_val.inc *= 10;
		if(pressed & KEY_R) mod_val.inc /= 10;		


		//Collisions and Friction
		//Detects collision with edge of the screen
		//Inverts velocity perpendicular to collision and gradually reduces velocity parallel to collision

		//Collision and friction with left or right edge

		for(u32 i = 0; objs[i].active; i++)
		{		
			
			
			for(u32 j = i; objs[j].active; j++)
			{
				if(i!=j)
				{
	
					if(detect_collision(objs[i], objs[j])==TOP_COLLISION)
					{
						calculate_velocity(objs[i], objs[j], VELOCITY_Y, &objs[i].vel.y, &objs[j].vel.y);
					
						objs[i].vel.y = objs[i].vel.y*mod_val.elasticity;
						

						
						double friction=mod_val.friction*abs(mod_val.gravity.y)*objs[i].length/50.0*objs[i].length/50.0;
						objs[i].vel.x+=(objs[i].vel.x>0?-fmin(friction,objs[i].vel.x):-fmax(-friction,objs[i].vel.x));
						
						objs[j].vel.y = objs[j].vel.y*mod_val.elasticity;
						objs[j].pos.y = objs[i].pos.y-objs[i].length/2.0-objs[j].length/2.0;
						
						double friction1=mod_val.friction*abs(mod_val.gravity.y)*objs[j].length/50.0*objs[j].length/50.0;
						objs[j].vel.x+=(objs[j].vel.x>0?-fmin(friction1,objs[j].vel.x):-fmax(-friction1,objs[j].vel.x));
					}
					//mostly working properly
					else if(detect_collision(objs[i], objs[j])==BOT_COLLISION)
					{

						
						//objs[j].pos.y = objs[i].pos.y-objs[i].length/2.0-objs[j].length/2.0;
						
						calculate_velocity(objs[i], objs[j], VELOCITY_Y, &objs[i].vel.y, &objs[j].vel.y);
						
						objs[j].vel.y = objs[j].vel.y*mod_val.elasticity;
						
						double friction=mod_val.friction*abs(mod_val.gravity.y)*objs[i].length/50.0*objs[j].length/50.0;
						objs[j].vel.x+=(objs[j].vel.x>0?-fmin(friction,objs[j].vel.x):-fmax(-friction,objs[j].vel.x));
						
						objs[i].vel.y = objs[i].vel.y*mod_val.elasticity;
						objs[i].pos.y = objs[j].pos.y-objs[j].length/2.0-objs[i].length/2.0;
						
						double friction1=mod_val.friction*abs(mod_val.gravity.y)*objs[j].length/50.0*objs[i].length/50.0;
						objs[i].vel.x+=(objs[i].vel.x>0?-fmin(friction1,objs[i].vel.x):-fmax(-friction1,objs[i].vel.x));
					}
					else if(detect_collision(objs[i], objs[j])==RIGHT_COLLISION)
					{
						
						calculate_velocity(objs[i], objs[j], VELOCITY_X, &objs[i].vel.x, &objs[j].vel.x);
						
						objs[i].vel.x = objs[i].vel.x*mod_val.elasticity;
						objs[i].pos.x = objs[j].pos.x+objs[j].length/2.0+objs[i].length/2.0;
						
						double friction=mod_val.friction*abs(mod_val.gravity.x)*objs[i].length/50.0*objs[i].length/50.0;
						objs[i].vel.y+=(objs[i].vel.y>0?-fmin(friction,objs[i].vel.y):-fmax(-friction,objs[i].vel.y));
						
						objs[j].vel.x = objs[j].vel.x*mod_val.elasticity;
						//objs[j].pos.y = objs[i].pos.y+objs[i].length/2.0+objs[j].length/2.0;
						
						double friction1=mod_val.friction*abs(mod_val.gravity.x)*objs[j].length/50.0*objs[j].length/50.0;
						objs[j].vel.y+=(objs[j].vel.y>0?-fmin(friction1,objs[j].vel.y):-fmax(-friction1,objs[j].vel.y));
					}
					else if(detect_collision(objs[i], objs[j])==LEFT_COLLISION)
					{
						
						calculate_velocity(objs[i], objs[j], VELOCITY_X, &objs[i].vel.x, &objs[j].vel.x);
						
						objs[j].vel.x = objs[j].vel.x*mod_val.elasticity;
						//objs[j].pos.y = objs[i].pos.y-objs[i].length/2.0-objs[j].length/2.0;
						
						double friction=mod_val.friction*abs(mod_val.gravity.x)*objs[i].length/50.0*objs[j].length/50.0;
						objs[j].vel.y+=(objs[j].vel.y>0?-fmin(friction,objs[j].vel.y):-fmax(-friction,objs[j].vel.y));
						
						objs[i].vel.x = objs[i].vel.x*mod_val.elasticity;
						objs[i].pos.x = objs[j].pos.x-objs[j].length/2.0-objs[i].length/2.0;
						
						double friction1=mod_val.friction*abs(mod_val.gravity.x)*objs[j].length/50.0*objs[i].length/50.0;
						objs[i].vel.y+=(objs[i].vel.y>0?-fmin(friction1,objs[i].vel.y):-fmax(-friction1,objs[i].vel.y));
					}
					
				}
			}
			
			
			for(u32 j = 0; walls[j].active; j++)
			{	
				if(walls[j].direction)
				{
					if(objs[i].pos.y-objs[i].length/2.0>=walls[j].pos.y && 
				           objs[i].pos.y+objs[i].length/2.0 <= walls[j].pos.y + walls[j].length)
					{
						//collide with left of vert wall j
						if(objs[i].pos.x<=walls[j].pos.x && objs[i].pos.x+objs[i].length/2.0 > walls[j].pos.x)
						{
							objs[i].vel.x = -objs[i].vel.x*mod_val.elasticity;
							objs[i].pos.x = walls[j].pos.x-objs[i].length/2.0;
				
							double friction=mod_val.friction*abs(mod_val.gravity.x)*objs[i].length/50.0*objs[i].length/50.0;
							objs[i].vel.y+=(objs[i].vel.y>0?-fmin(friction,objs[i].vel.y):-fmax(-friction,objs[i].vel.y));
							
						}
						//collide with right of vert wall j
						else if(objs[i].pos.x>=walls[j].pos.x && objs[i].pos.x-objs[i].length/2.0 < walls[j].pos.x + 1)
						{
							objs[i].vel.x = -objs[i].vel.x*mod_val.elasticity;
							objs[i].pos.x = walls[j].pos.x+objs[i].length/2.0+1;

							double friction=mod_val.friction*abs(mod_val.gravity.x)*objs[i].length/50.0*objs[i].length/50.0;
							objs[i].vel.y+=(objs[i].vel.y>0?-fmin(friction,objs[i].vel.y):-fmax(-friction,objs[i].vel.y));
						}
					}
				}
				else
				{
					if(objs[i].pos.x+objs[i].length/2.0>=walls[j].pos.x && 
					   objs[i].pos.x-objs[i].length/2.0 <= walls[j].pos.x + walls[j].length)
					{
						//collide with top of horiz wall j
						if(objs[i].pos.y<=walls[j].pos.y && objs[i].pos.y+objs[i].length/2.0 >= walls[j].pos.y)
						{
							objs[i].vel.y = -objs[i].vel.y*mod_val.elasticity;
							objs[i].pos.y = walls[j].pos.y-objs[i].length/2.0;

							double friction=mod_val.friction*abs(mod_val.gravity.y)*objs[i].length/50.0*objs[i].length/50.0;
							objs[i].vel.x+=(objs[i].vel.x>0?-fmin(friction,objs[i].vel.x):-fmax(-friction,objs[i].vel.x));
							
						}
						//collide with bottom of horiz wall j
						else if(objs[i].pos.y>=walls[j].pos.y && objs[i].pos.y-objs[i].length/2.0 <= walls[j].pos.y + 1)
						{
							objs[i].vel.y = -objs[i].vel.y*mod_val.elasticity;
							objs[i].pos.y = walls[j].pos.y+objs[i].length/2.0+1;

							double friction=mod_val.friction*abs(mod_val.gravity.y)*objs[i].length/50.0*objs[i].length/50.0;
							objs[i].vel.x+=(objs[i].vel.x>0?-fmin(friction,objs[i].vel.x):-fmax(-friction,objs[i].vel.x));
						}
					}
				}	
			}
			
		}

		//If the coordinates of the screen tap are inside the square, allow movement of the square
		if (pressed & KEY_TOUCH)
		{

			for(u32 i = 0; objs[i].active; i++)
			{

				if ((mod_val.touch.px >= objs[i].pos.x-objs[i].length/2-mod_val.camera.x) && 
					(mod_val.touch.px <= objs[i].pos.x+objs[i].length/2-mod_val.camera.x) && 
					(mod_val.touch.py >= objs[i].pos.y-objs[i].length/2-mod_val.camera.y) && 
					(mod_val.touch.py <= objs[i].pos.y+objs[i].length/2-mod_val.camera.y))
				{
					mod_val.mov_rect = i;
					mod_val.offset.x = objs[i].pos.x - mod_val.touch.px;
					mod_val.offset.y = objs[i].pos.y - mod_val.touch.py;
					break;
				}
			}		
		}
		
		//If the screen is let go, stop allowing screen to control square
		if ((released & KEY_TOUCH))
		{
			mod_val.mov_rect = -1;
			mod_val.offset.x = 0;
			mod_val.offset.y = 0;
		}



		//Press x to reset all values
		if(pressed & KEY_X)
		{
			mod_val = def;
			objs[0] = obj0;
			objs[1] = obj1;
			objs[2] = obj2;
			for(u8 i = 3; i < 32; i ++) 
			{
				objs[i] = obj_null;
			}
			walls[0] = wobj0;
			walls[1] = wobj1;
			walls[2] = wobj2;
			walls[3] = wobj3;
			for(u8 i = 4; i < 32; i ++) 
			{
				walls[4] = wall_null;
			}
			
			consoleSelect(&obj_info);
			consoleClear();
			consoleSelect(&global_info);
			consoleClear();
			consoleSelect(&top);
		}

		if(mod_val.gravity_from_accel)
		{
			mod_val.gravity.x = accel.x / -mod_val.accel_to_gravity;
			mod_val.gravity.y = accel.z / mod_val.accel_to_gravity;
		}

		//Determines how to move the square
		//If the touch screen is not dictating the square's location, apply gravity and velocity
		//Else move square to where screen is touched		
		
		for(u32 i = 0; objs[i].active; i++)
		{

			if(mod_val.mov_rect==i)
			{
				objs[i].pos.x = mod_val.touch.px + mod_val.offset.x;
				objs[i].pos.y = mod_val.touch.py + mod_val.offset.y;
			}
			else
			{	
				objs[i].vel.y -= mod_val.gravity.y;
				objs[i].vel.x += mod_val.gravity.x;
		
				objs[i].pos.x += objs[i].vel.x/60.0f;
				objs[i].pos.y += objs[i].vel.y/60.0f;
			}
		}

		//Live printing of important values
		#ifdef debug
		printf("\x1b[29;41H%s:%5.2f","fps",sf2d_get_fps());
		
		consoleSelect(&global_info);		
		printf("\x1b[0;0H%s:%03d,%03d","touch_coord",mod_val.touch.px,mod_val.touch.py);
		printf("\x1b[1;0H%s:%05.0f,%05.0f","offset",mod_val.offset.x,mod_val.offset.y);
		printf("\x1b[2;0H%s:%6.2f,%6.2f","gravity",mod_val.gravity.x,mod_val.gravity.y);
		printf("\x1b[3;0H%s:%5.5f","elasticity",mod_val.elasticity);
		printf("\x1b[4;0H%s:%5.5f","friction",mod_val.friction);
		printf("\x1b[5;0H%s:%f","increment",mod_val.inc);		
		printf("\x1b[10;0H%s:%s","mod_val",get_current_string(mod_val.current));
		printf("\x1b[6;0H%s:%s","gravity_from_accel", get_gravity_from_accel_string(mod_val.gravity_from_accel));
		printf("\x1b[7;0H%s:%03d","mov_rect",mod_val.mov_rect);
		printf("\x1b[8;0H%s:%05.0f,%05.0f","camera",mod_val.camera.x,mod_val.camera.y);	
		printf("\x1b[9;0H%s:%s","scope",get_scope_string(mod_val.scope));
		
		consoleSelect(&obj_info);
		
		if(mod_val.obj_type)
		{
			printf("\x1b[3;0H%s:%d","direction",walls[mod_val.curr_view].direction);
			printf("\x1b[2;0H%s:%03.0f,%03.0f","coord",walls[mod_val.curr_view].pos.x,walls[mod_val.curr_view].pos.y);
			printf("\x1b[0;0H%s:w%02d","curr_view",mod_val.curr_view);
			printf("\x1b[1;0H%s:%d","active",walls[mod_val.curr_view].active);
			printf("\x1b[4;0H%s:%03d","length",walls[mod_val.curr_view].length);
		}
		else
		{
			printf("\x1b[2;0H%s:%03.0f,%03.0f","coord",objs[mod_val.curr_view].pos.x,objs[mod_val.curr_view].pos.y);
			printf("\x1b[3;0H%s:%07.1f,%07.1f","velocity",objs[mod_val.curr_view].vel.x,-objs[mod_val.curr_view].vel.y);
			printf("\x1b[0;0H%s:o%02d","curr_view",mod_val.curr_view);
			printf("\x1b[1;0H%s:%d","active",objs[mod_val.curr_view].active);
			printf("\x1b[4;0H%s:%03d","length",objs[mod_val.curr_view].length);
		}
		#endif

		//draw frame
		sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);

		draw_phys_objs(objs, mod_val.camera);

		draw_wall_objs(walls, mod_val.camera);
		
		sf2d_end_frame();

		sf2d_swapbuffers();
	}

	
	sf2d_fini();
	return 0;
}
