#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <3ds.h>
#include <sf2d.h>


#define CONFIG_3D_SLIDERSTATE (*(float *)0x1FF81080)
#define debug

char* get_mod_val_id(u8);

int main()
{
	// Set the random seed based on the time
	srand(time(NULL));

	sf2d_init();
	sf2d_set_clear_color(RGBA8(0xFF, 0xFF, 0xFF, 0xFF));
	sf2d_set_3D(0);
	sf2d_set_vblank_wait(1);


	u8 mod_val = 0;
	u16 touch_x = 320/2;
	u16 touch_y = 240/2;
	float rect_x = 320/2;
	float rect_y = 240/2;
	float inc = 1.0f;
	u8 mov_rect = 0.0f;
	s8 offset_x = 0.0f;
	s8 offset_y = 0.0f;
	float vel_x = 0.0f;
	float vel_y = 0.0f;
	float elasticity = .5f;
	float gravity_y = -10.0f;
	float gravity_x = 0.0f;	
	float friction = .5f;
	touchPosition touch;
	circlePosition circle;
	u32 held;
	u32 pressed;
	u32 released;

	consoleInit(GFX_TOP, NULL);

	printf("\x1b[0;0HPhysics Sandbox v0.1 by PieFace");
	printf("\x1b[23;0HControls:");
	printf("\x1b[24;0HTap and drag square to move it");
	printf("\x1b[25;0HX - Reset Square");
	printf("\x1b[26;0HDPad L&R - Change between mod_vals");
	printf("\x1b[27;0HDPad U&D - Increment and decrement mod_val");
	printf("\x1b[28;0HL&R - Change increment (pow of 10)");	
	printf("\x1b[29;0HStart - Exit");
	while (aptMainLoop()) {

		hidScanInput();
		hidCircleRead(&circle);
		held = hidKeysHeld();
		pressed = hidKeysDown();
		released = hidKeysUp();

		if (held & KEY_START) 
		{
			break;
		}
		
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

		if(pressed & KEY_DDOWN)
		{
			if(mod_val == 0)
			{
				gravity_y -= inc;
			}
			
			if(mod_val == 1)
			{
				gravity_x -= inc;
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

		if(pressed & KEY_L) inc *= 10;
		if(pressed & KEY_R) inc /= 10;		

		if(rect_x-25 <=0 || rect_x+25 >= 319)
		{
			vel_x = -vel_x*elasticity;
			rect_x = (rect_x <=25 ? 0 + 25 : 320 - 25);
			vel_y += (vel_y > 0 ? -fmin(friction,vel_y) : -fmax(-friction,vel_y));
		}

		if(rect_y-25 <=0 || rect_y+25 >= 239) 
		{
			vel_y = -vel_y*elasticity;
			rect_y = (rect_y <=25 ? 0 + 25 : 240 - 25);
			vel_x += (vel_x > 0 ? -fmin(friction,vel_x) : -fmax(-friction,vel_x));

		}

		if ((pressed & KEY_TOUCH)&&(touch_x > rect_x-25)&&(touch_x < rect_x+25)&&(touch_y > rect_y-25)&&(touch_y < rect_y+25))
		{
			mov_rect = 1;
			offset_x = rect_x - touch_x;
			offset_y = rect_y - touch_y;
		}

		if ((released & KEY_TOUCH))
		{
			mov_rect = 0;
			offset_x = 0;
			offset_y = 0;
		}

		if(pressed & KEY_X)
		{
			mod_val = 0;
			touch_x = 320/2;
			touch_y = 240/2;
			rect_x = 320/2;
			rect_y = 240/2;
			inc = 1.0f;
			mov_rect = 0.0f;
			offset_x = 0.0f;
			offset_y = 0.0f;
			vel_x = 0.0f;
			vel_y = 0.0f;
			elasticity = .5f;
			gravity_y = -10.0f;
			gravity_x = 0.0f;	
			friction = .5f;
		}

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

		#ifdef debug
		printf("\x1b[0;41H%s:%5.2f","fps",sf2d_get_fps());
		printf("\x1b[1;0H%s:%03d,%03d","touch_coord",touch_x,touch_y);
		printf("\x1b[2;0H%s:%03.0f,%03.0f","rect_coord",rect_x,rect_y);
		printf("\x1b[3;0H%s:%03d,%03d","offset",offset_x,offset_y);
		printf("\x1b[4;0H%s:%010.3f,%010.3f","velocity",vel_x,vel_y);
		printf("\x1b[5;0H%s:%f,%f","gravity",gravity_x,gravity_y);
		printf("\x1b[6;0H%s:%f","elasticity",elasticity);
		printf("\x1b[7;0H%s:%f","friction",friction);
		printf("\x1b[8;0H%s:%f","increment",inc);		
		printf("\x1b[9;0H%s:%s","mod_val",get_mod_val_id(mod_val));		
		#endif

		sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);

		sf2d_draw_rectangle((int)rect_x-25, (int)rect_y-25, 50, 50, RGBA8(0xFF, 0x00, 0xFF, 0xFF));
		
		sf2d_end_frame();

		sf2d_swapbuffers();
	}

	
	sf2d_fini();
	return 0;
}

char* get_mod_val_id(u8 id)
{
	if(id==0) return "gravity_y ";
	if(id==1) return "gravity_x ";
	if(id==2) return "elasticity";
	if(id==3) return "friction  ";
	return "err_bad_id";
}
