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

#define CONFIG_3D_SLIDERSTATE (*(float *)0x1FF81080)
#define debug

//forward declarations
char* get_current_string(u8);
char* get_gravity_from_accel_string(u8);

int main()
{
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
		0
	};

	s16 deadzone = 25;

	_mod_val mod_val = def;

	phys_obj* objs = (phys_obj*) calloc(8, sizeof(phys_obj));
	wall_obj* walls = (wall_obj*) calloc(8, sizeof(wall_obj));


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

	consoleInit(GFX_TOP, NULL);

	//gfxSetScreenFormat(GFX_TOP, 1);

	printf("\x1b[0;0HPhysics Sandbox v0.3 Alpha by PieFace");
	printf("\x1b[21;0HControls:");
	printf("\x1b[22;0HTap and drag square to move it");
	printf("\x1b[23;0HX - Reset Square");
	printf("\x1b[24;0HDPad L&R - Change between mod_vals");
	printf("\x1b[25;0HDPad U&D - Increment and decrement mod_val");
	printf("\x1b[26;0HL&R - Change increment (pow of 10)");
	printf("\x1b[27;0HY - Toggle gravity from gyroscope");
	printf("\x1b[28;0HSelect - Save screenshot");	
	printf("\x1b[29;0HStart - Exit");
	while (aptMainLoop()) {

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
				mod_val.curr_view ++;
				if(!objs[mod_val.curr_view].active) mod_val.curr_view = 0;
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
				if(mod_val.curr_view == 0)
				{
					for(mod_val.curr_view = 0; objs[mod_val.curr_view + 1].active; 
						mod_val.curr_view ++);
				}
				else
				{
					mod_val.curr_view --;
				}
			}
		}

		//DPAD Left and Right - Change mod_val.current
		if(pressed & KEY_DLEFT)
		{
			mod_val.current--;
			if (mod_val.current > 4) mod_val.current = 4;
		}
		
		if(pressed & KEY_DRIGHT)
		{
			mod_val.current++;
			if (mod_val.current > 4) mod_val.current = 0;
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
			/*
			if(objs[i].pos.x-objs[i].length/2 <=1 || objs[i].pos.x+objs[i].length/2 >= 319)
			{
				objs[i].vel.x = -objs[i].vel.x*mod_val.elasticity;
				//rect_x = (rect_x <=25 ? 0 + 25 : 320 - 25);				
				if(objs[i].pos.x < 1.0 + objs[i].length/2.0) objs[i].pos.x = 1.0 + objs[i].length/2.0;
				if(objs[i].pos.x > 319.0 - objs[i].length/2.0) objs[i].pos.x = 319.0-objs[i].length/2.0;
				if(objs[i].pos.x < 1.0 + objs[i].length/2.0 || objs[i].pos.x > 319.0 - objs[i].length/2.0) 
					objs[i].vel.x -= mod_val.gravity.x;
				
				double friction = mod_val.friction*abs(mod_val.gravity.x)*objs[i].length/50.0*objs[i].length/50.0;
				objs[i].vel.y += (objs[i].vel.y > 0 ? -fmin(friction,objs[i].vel.y) : -fmax(-friction,objs[i].vel.y));
			}
			
			//Collision and friction with top and bottom edge
			if(objs[i].pos.y-objs[i].length/2 <=1 || objs[i].pos.y+objs[i].length/2 >= 239) 
			{
				objs[i].vel.y = -objs[i].vel.y*mod_val.elasticity;
				//rect_y = (rect_y <=25 ? 0 + 25 : 240 - 25);
				if(objs[i].pos.y < 1.0 + objs[i].length/2.0) objs[i].pos.y = 1.0 + objs[i].length/2.0;
				if(objs[i].pos.y > 239.0 - objs[i].length/2.0) objs[i].pos.y = 239-objs[i].length/2.0;

				if(objs[i].pos.y < 1.0 + objs[i].length/2.0 || objs[i].pos.y > 239.0 - objs[i].length/2.0)
					objs[i].vel.y += mod_val.gravity.y;
				
				double friction = mod_val.friction*abs(mod_val.gravity.y)*objs[i].length/50.0*objs[i].length/50.0;
				objs[i].vel.x += (objs[i].vel.x > 0 ? -fmin(friction,objs[i].vel.x) : -fmax(-friction,objs[i].vel.x));
	
			}
			*/
			
			for(u32 j = 0; walls[j].active; j++)
			{	
				if(walls[j].direction)
				{
					if(objs[i].pos.y-objs[i].length/2.0>=walls[j].pos.y && 
				           objs[i].pos.y+objs[i].length/2.0 <= walls[j].pos.y + walls[j].length)
					{
						//collide with left of vert wall
						if(objs[i].pos.x<=walls[j].pos.x && objs[i].pos.x+objs[i].length/2.0 > walls[j].pos.x)
						{
							objs[i].vel.x = -objs[i].vel.x*mod_val.elasticity;
							objs[i].pos.x = walls[j].pos.x-objs[i].length/2.0;
				
							double friction=mod_val.friction*abs(mod_val.gravity.x)*objs[i].length/50.0*objs[i].length/50.0;
							objs[i].vel.y+=(objs[i].vel.y>0?-fmin(friction,objs[i].vel.y):-fmax(-friction,objs[i].vel.y));
							
						}
						//collide with right of vert wall
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
						//collide with top of horiz wall
						if(objs[i].pos.y<=walls[j].pos.y && objs[i].pos.y+objs[i].length/2.0 >= walls[j].pos.y)
						{
							objs[i].vel.y = -objs[i].vel.y*mod_val.elasticity;
							objs[i].pos.y = walls[j].pos.y-objs[i].length/2.0;

							double friction=mod_val.friction*abs(mod_val.gravity.y)*objs[i].length/50.0*objs[i].length/50.0;
							objs[i].vel.x+=(objs[i].vel.x>0?-fmin(friction,objs[i].vel.x):-fmax(-friction,objs[i].vel.x));
							
						}
						//collide with bottom of horiz wall
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
		printf("\x1b[0;41H%s:%5.2f","fps",sf2d_get_fps());		
		printf("\x1b[1;0H%s:%03d,%03d","touch_coord",mod_val.touch.px,mod_val.touch.py);
		printf("\x1b[2;0H%s:%03.0f,%03.0f","rect_coord",objs[mod_val.curr_view].pos.x,objs[mod_val.curr_view].pos.y);
		printf("\x1b[3;0H%s:%05.0f,%05.0f","offset",mod_val.offset.x,mod_val.offset.y);
		printf("\x1b[4;0H%s:%010.3f,%010.3f","velocity",objs[mod_val.curr_view].vel.x,-objs[mod_val.curr_view].vel.y);
		printf("\x1b[5;0H%s:%f,%f","gravity",mod_val.gravity.x,mod_val.gravity.y);
		printf("\x1b[6;0H%s:%f","elasticity",mod_val.elasticity);
		printf("\x1b[7;0H%s:%f","friction",mod_val.friction);
		printf("\x1b[8;0H%s:%f","increment",mod_val.inc);		
		printf("\x1b[12;0H%s:%s","mod_val",get_current_string(mod_val.current));
		printf("\x1b[11;0H%s:%s","gravity_from_accel", get_gravity_from_accel_string(mod_val.gravity_from_accel));
		printf("\x1b[9;0H%s:%02d","curr_view",mod_val.curr_view);
		printf("\x1b[10;0H%s:%02d","mov_rect",mod_val.mov_rect);
		printf("\x1b[13;0H%s:%04d,%04d","circle pad",circle.dx,circle.dy);
		printf("\x1b[14;0H%s:%05.0f,%05.0f","camera",mod_val.camera.x,mod_val.camera.y);	
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

//returns string of mod_val from id
char* get_current_string(u8 id)
{
	if(id==0) return "gravity_y ";
	if(id==1) return "gravity_x ";
	if(id==2) return "elasticity";
	if(id==3) return "friction  ";
	if(id==4) return "curr_view ";
	return "err_bad_id";
}

char* get_gravity_from_accel_string(u8 id)
{
	if(id == 0 ) return "off";
	return "on ";
}
