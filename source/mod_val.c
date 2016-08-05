//Copyright (c) 2016 PieFace
//mod_val.h

#include <3ds.h>

//returns string of mod_val from id
char* get_current_string(u8 id)
{
	if(id==0) return "gravity.y ";
	if(id==1) return "gravity.x ";
	if(id==2) return "elasticity";
	if(id==3) return "friction  ";
	if(id==4) return "scope     ";
	if(id==5) return "obj_type  ";
	if(id==6) return "curr_view ";
	if(id==7) return "active    ";
	if(id==8) return "coord.x   ";
	if(id==9) return "coord.y   ";
	if(id==10) return "direction ";
	if(id==11) return "vel.x     ";
	if(id==12) return "vel.y     ";
	if(id==13) return "length    ";
	return "err_bad_id";
}

char* get_gravity_from_accel_string(u8 id)
{
	if(id == 1 ) return "on ";
	return "off";
}

char* get_scope_string(u8 id)
{
	if(id == 1) return "object";
	return "global";
}
