//Copyright (c) 2016 PieFace
//mod_val.h

#ifndef _mod_val_h
#define _mod_val_h


typedef struct
{
	double x;
	double y;
} _coord;

typedef struct
{
	double x;
	double y;
} _vector;


typedef struct
{
	//Represents the current modifiable value: gravity_y, gravity_x, friction, elasticity
	u8 current;

	//Whether to use data from accelerometer to update gravity or not
	u8 gravity_from_accel;

	//coordinates of where screen is touched
	touchPosition touch;

	//increment - determines how much is added or subtracted to mod_val - is always power of 10 
	double inc;

	//number to determine which physobj touch screen is currently dictating the square's location (-1 for none)
	s32 mov_rect;

	//records where the user tapped on the square e.g. if the top right corner is tapped, it will stay at top right corner
	_coord offset;

	//gravity values - will accelerate square in given direction on every frame - pixels per second^2
	_vector gravity;

	//backup gravity used when toggling between gravity_from_accel
	_vector gravity_b;

	//In a collision, velocity perpendicular to collision is multiplied by elasticity e.g. at 0.5, half of y speed is lost when colliding with floor 
	double elasticity;

	//Friciton - reduces speed parallel to wall if the wall is being touched
	double friction;

	//coefficient of conversion from accel values to gravity values
	double accel_to_gravity;

	//coordinates of camera
	_coord camera;

	//probably temporary-select which physobj's data is viewed
	u8 curr_view; 
	
	//determines whether we are editing global or object info
	u8 scope;
	
	//determines if user is editing wall_obj or phys_obj
	u8 obj_type;

} _mod_val;

char* get_current_string(u8);
char* get_gravity_from_accel_string(u8);
char* get_scope_string(u8);

#endif
