//Copyright (c) 2016 PieFace
//Screenshot.h

#ifndef _screenshot_h
#define _screenshot_h

typedef struct FILE_HEADER 
{
	u16 type;
	u32 size;
	u16 r2;
	u16 r1;
	u32 offset;
} file_header;

typedef struct IMAGE_HEADER 
{
	u32 size;
	u32 width;
	u32 height;
	u16 planes;
	u16 bit_count;
	u32 compression;
	u32 size_image;
	u32 x_pixels_per_meter;
	u32 y_pixels_per_meter;
	u32 colors_used;
	u32 colors_important;
} image_header;

void save_screenshot();
void save_screenshot_bottom();
void save_screenshot_top();
void write_file_header(file_header*, FILE*);
void write_image_header(image_header*, FILE*);
void write_framebuffer_RGB888(FILE*,u32);
void write_framebuffer_RGB565(FILE*,u32);

#endif
