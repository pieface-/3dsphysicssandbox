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


#define CONFIG_3D_SLIDERSTATE (*(float *)0x1FF81080)
#define debug

//forward declarations
char* get_mod_val_id(u8);
char* get_gravity_from_accel_id(u8);

int main()
{
	// Set the random seed based on the time
	srand(time(NULL));

	//init screen background to white
	sf2d_init();
	sf2d_set_clear_color(RGBA8(0xFF, 0xFF, 0xFF, 0xFF));
	sf2d_set_3D(0);
	sf2d_set_vblank_wait(1);


	//Variables

	//Represents the current modifiable value: gravity_y, gravity_x, friction, elasticity
	u8 mod_val = 0;
	u8 gravity_from_accel = 0;

	//coordinates of screen touch
	u16 touch_x = 320/2;
	u16 touch_y = 240/2;

	//coordinates of rectangle (double to allow small velocities - casted to int when rendering)
	double rect_x = 320/2;
	double rect_y = 240/2;

	//increment - determines how much is added or subtracted to mod_val - is always power of 10 
	double inc = 1.0;
	
	//boolean to determine if touch screen is currently dictating the square's location
	u8 mov_rect = 0.0;

	//records where the user tapped on the square e.g. if the top right corner is tapped, it will stay at top right corner
	s8 offset_x = 0.0;
	s8 offset_y = 0.0;

	//x and y velocity of the square in pixels per second
	double vel_x = 0.0;
	double vel_y = 0.0;

	//In a collision, velocity perpendicular to collision is multiplied by elasticity e.g. at 0.5, half of y speed is lost when colliding with floor 
	double elasticity = .5;

	//gravity values - will accelerate square in given direction on every frame - pixels per second^2
	double gravity_y = -10.0;
	double gravity_x = 0.0;
	double gravity_y_b = -10.0;
	double gravity_x_b = 0.0;
	double accel_to_gravity = 25.0;

	//Friciton - reduces speed parallel to wall if the wall is being touched
	//currently not implemented well, needs to reflect gravity values	
	double friction = .5;

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

	printf("\x1b[0;0HPhysics Sandbox v0.2 by PieFace");
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
			hidTouchRead(&touch);
			touch_x = touch.px;
			touch_y = touch.py;
			if(mov_rect)
			{
				vel_x = (touch_x + offset_x - rect_x)*60.0f;
				vel_y = (touch_y + offset_y - rect_y)*60.0f;
			}
		}

		if(pressed & KEY_Y)
		{
			if(gravity_from_accel==0)
			{
				gravity_from_accel = 1;
				gravity_y_b = gravity_y;
				gravity_x_b = gravity_x;
			}			
			else
			{
				gravity_from_accel = 0;
				gravity_y = gravity_y_b;
				gravity_x = gravity_x_b;
			}		
		}

		//DPAD up - Increment current mod_val
		if(pressed & KEY_DUP)
		{
			if(mod_val == 0)
			{
				gravity_y += inc;
			}
			
			if(mod_val == 1)
			{
				gravity_x += inc;
			}

			if(mod_val == 2)
			{
				elasticity += inc;
				if(elasticity > 1) elasticity = 1;
			}

			if(mod_val == 3)
			{
				friction += inc;
				if(friction > 1) friction = 1;
			}
		}
		
		//DPAD down - decrement current mod_val
		if(pressed & KEY_DDOWN)
		{
			if(mod_val == 0)
			{
				if(gravity_from_accel)
				{
					gravity_y_b -= inc;
				}
				else
				{
					gravity_y -= inc;
				}
			}
			
			if(mod_val == 1)
			{
				if(gravity_from_accel)
				{
					gravity_x_b -= inc;
				}
				else
				{
					gravity_x -= inc;
				}
			}

			if(mod_val == 2)
			{
				elasticity -= inc;
				if(elasticity < 0) elasticity = 0;
			}

			if(mod_val == 3)
			{
				friction -= inc;
				if(friction < 0) friction = 0;
			}
		}

		//DPAD Left and Right - Change mod_val
		if(pressed & KEY_DLEFT)
		{
			mod_val--;
			if (mod_val > 3) mod_val = 3;
		}
		
		if(pressed & KEY_DRIGHT)
		{
			mod_val++;
			if (mod_val > 3) mod_val = 0;
		}
		

		//L and R - Multiply and divide increment by 10
		if(pressed & KEY_L) inc *= 10;
		if(pressed & KEY_R) inc /= 10;		


		//Collisions and Friction
		//Detects collision with edge of the screen
		//Inverts velocity perpendicular to collision and gradually reduces velocity parallel to collision

		//Collision and friction with left or right edge
		if(rect_x-25 <=1 || rect_x+25 >= 319)
		{
			vel_x = -vel_x*elasticity;
			//rect_x = (rect_x <=25 ? 0 + 25 : 320 - 25);
			if(rect_x < 25) rect_x = 25;
			if(rect_x > 320 - 25) rect_x = 320-25;
			vel_y += (vel_y > 0 ? -fmin(friction*abs(gravity_x),vel_y) : -fmax(-friction*abs(gravity_x),vel_y));
		}
		
		//Collision and friction with top and bottom edge
		if(rect_y-25 <=1 || rect_y+25 >= 239) 
		{
			vel_y = -vel_y*elasticity;
			//rect_y = (rect_y <=25 ? 0 + 25 : 240 - 25);
			if(rect_y < 25) rect_y = 25;
			if(rect_y > 240 - 25) rect_y = 240-25;
			vel_x += (vel_x > 0 ? -fmin(friction*abs(gravity_y),vel_x) : -fmax(-friction*abs(gravity_y),vel_x));

		}

		//If the coordinates of the screen tap are inside the square, allow movement of the square
		if ((pressed & KEY_TOUCH)&&(touch_x > rect_x-25)&&(touch_x < rect_x+25)&&(touch_y > rect_y-25)&&(touch_y < rect_y+25))
		{
			mov_rect = 1;
			offset_x = rect_x - touch_x;
			offset_y = rect_y - touch_y;
		}
		
		//If the screen is let go, stop allowing screen to control square
		if ((released & KEY_TOUCH))
		{
			mov_rect = 0;
			offset_x = 0;
			offset_y = 0;
		}

		//Press x to reset all values
		if(pressed & KEY_X)
		{
			mod_val = 0;
			touch_x = 320/2;
			touch_y = 240/2;
			rect_x = 320/2;
			rect_y = 240/2;
			inc = 1.0;
			mov_rect = 0.0;
			offset_x = 0.0;
			offset_y = 0.0;
			vel_x = 0.0;
			vel_y = 0.0;
			elasticity = .5;
			gravity_y = -10.0;
			gravity_x = 0.0;
			gravity_y_b = -10.0;
			gravity_x_b = 0;	
			friction = .5;
			gravity_from_accel = 0;
		}

		if(gravity_from_accel)
		{
			gravity_x = accel.x / -accel_to_gravity;
			gravity_y = accel.z / accel_to_gravity;
		}

		//Determines how to move the square
		//If the touch screen is not dictating the square's location, apply gravity and velocity
		//Else move square to where screen is touched		
		if(!mov_rect)
		{
			vel_y -= gravity_y;
			vel_x += gravity_x;
	
			rect_x += vel_x/60.0f;
			rect_y += vel_y/60.0f;
		}
		else
		{
			rect_x = touch_x + offset_x;
			rect_y = touch_y + offset_y;
		}

		//Live printing of important values
		#ifdef debug
		printf("\x1b[0;41H%s:%5.2f","fps",sf2d_get_fps());		
		printf("\x1b[1;0H%s:%03d,%03d","touch_coord",touch_x,touch_y);
		printf("\x1b[2;0H%s:%03.0f,%03.0f","rect_coord",rect_x,rect_y);
		printf("\x1b[3;0H%s:%03d,%03d","offset",offset_x,offset_y);
		printf("\x1b[4;0H%s:%010.3f,%010.3f","velocity",vel_x,-vel_y);
		printf("\x1b[5;0H%s:%f,%f","gravity",gravity_x,gravity_y);
		printf("\x1b[6;0H%s:%f","elasticity",elasticity);
		printf("\x1b[7;0H%s:%f","friction",friction);
		printf("\x1b[8;0H%s:%f","increment",inc);		
		printf("\x1b[9;0H%s:%s","mod_val",get_mod_val_id(mod_val));
		printf("\x1b[10;0H%s:%s","gravity_from_accel", get_gravity_from_accel_id(gravity_from_accel));
		//printf("\x1b[12;0H%s x:%d y:%d z:%d                      ","accel", accel.x, accel.y, accel.z);	
		#endif

		//draw frame
		sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);

		sf2d_draw_rectangle((int)rect_x-25, (int)rect_y-25, 50, 50, RGBA8(0xFF, 0x00, 0xFF, 0xFF));
		
		sf2d_end_frame();

		sf2d_swapbuffers();
	}

	
	sf2d_fini();
	return 0;
}

//returns string of mod_val from id
char* get_mod_val_id(u8 id)
{
	if(id==0) return "gravity_y ";
	if(id==1) return "gravity_x ";
	if(id==2) return "elasticity";
	if(id==3) return "friction  ";
	return "err_bad_id";
}

char* get_gravity_from_accel_id(u8 id)
{
	if(id == 0 ) return "off";
	return "on ";
}
