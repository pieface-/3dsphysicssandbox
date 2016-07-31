//Copyright (c) 2016 PieFace
//Screenshot.c

#include <3ds.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "screenshot.h"

//u8* dir = "3DS Physics Sandbox\\";
//u8* root;

void save_screenshot()
{
	//save_screenshot_top();
	save_screenshot_bottom();
}


void save_screenshot_top()
{	
	file_header fh;
	fh.type = 0x4D42;
	fh.size = 14 + 40 + 400*240*3;
	fh.r1 = 0;
	fh.r2 = 0;
	fh.offset = 14 + 40;

	image_header ih;
	ih.size = 40;
	ih.width = 400;
	ih.height = 240;
	ih.planes = 1;
	ih.bit_count = 24;
	ih.compression = 0;
	ih.size_image = 0;
	ih.x_pixels_per_meter = 0;
	ih.y_pixels_per_meter = 0;
	ih.colors_used = 0;
	ih.colors_important = 0;
	

	char* filename = (char*) malloc(64);
	u32 count = 0;
	FILE* f = NULL;
	do
	{
		if(f) fclose(f);		
		count++;
		sprintf(filename, "3dsphysicssandbox_top%d.bmp", count);
		f = fopen(filename,"r");
	} 
	while(f);	
	printf("\x1b[17;0HSaving %s...", filename);

	fclose(f);

	f = fopen(filename,"wb");
	
	write_file_header(&fh, f);
	write_image_header(&ih, f);
	write_framebuffer_RGB565(f, ih.width);

	fclose(f);

	printf("\x1b[18;0HSaved %s            ", filename);
}


void save_screenshot_bottom()
{	
	file_header fh;
	fh.type = 0x4D42;
	fh.size = 14 + 40 + 320*240*3;
	fh.r1 = 0;
	fh.r2 = 0;
	fh.offset = 14 + 40;

	image_header ih;
	ih.size = 40;
	ih.width = 320;
	ih.height = 240;
	ih.planes = 1;
	ih.bit_count = 24;
	ih.compression = 0;
	ih.size_image = 0;
	ih.x_pixels_per_meter = 0;
	ih.y_pixels_per_meter = 0;
	ih.colors_used = 0;
	ih.colors_important = 0;
	
	char* filename = (char*) malloc(64);
	u32 count = 0;
	FILE* f = NULL;
	do
	{
		if(f) fclose(f);		
		count++;
		sprintf(filename, "3dsphysicssandbox_bottom%d.bmp", count);
		f = fopen(filename,"r");
	} 
	while(f);

	fclose(f);

	f = fopen(filename,"wb");

	printf("\x1b[17;0HSaving %s...", filename);
	
	write_file_header(&fh, f);
	write_image_header(&ih, f);
	write_framebuffer_RGB888(f, ih.width);

	printf("\x1b[18;0HSaved %s            ", filename);

	fclose(f);
}

void write_file_header(file_header* fh, FILE* f)
{
	fwrite(&fh->type, sizeof(fh->type), 1, f);
	fwrite(&fh->size, sizeof(fh->size), 1, f);
	fwrite(&fh->r1, sizeof(fh->r1), 1, f);
	fwrite(&fh->r2, sizeof(fh->r2), 1, f);
	fwrite(&fh->offset, sizeof(fh->offset), 1, f);	
}

void write_image_header(image_header* ih, FILE* f)
{
	fwrite(&ih->size, sizeof(ih->size), 1, f);
	fwrite(&ih->width, sizeof(ih->width), 1, f);
	fwrite(&ih->height, sizeof(ih->height), 1, f);
	fwrite(&ih->planes, sizeof(ih->planes), 1, f);
	fwrite(&ih->bit_count, sizeof(ih->bit_count), 1, f);
	fwrite(&ih->compression, sizeof(ih->compression), 1, f);
	fwrite(&ih->size_image, sizeof(ih->size_image), 1, f);
	fwrite(&ih->x_pixels_per_meter, sizeof(ih->x_pixels_per_meter), 1, f);
	fwrite(&ih->y_pixels_per_meter, sizeof(ih->y_pixels_per_meter), 1, f);
	fwrite(&ih->colors_used, sizeof(ih->colors_used), 1, f);
	fwrite(&ih->colors_important, sizeof(ih->colors_important), 1, f);	
}

void write_framebuffer_RGB888(FILE* f, u32 width)
{
	
	u8* bufAdr;
	if(width == 320)	
	{
		bufAdr = gfxGetFramebuffer(GFX_BOTTOM, GFX_LEFT, NULL, NULL);
	}
	else
	{
		bufAdr = gfxGetFramebuffer(GFX_TOP, GFX_RIGHT, NULL, NULL);	
	}

	int i, j;

	//u8* framebuf = (u8*) malloc(3*240*width);

	for(j=0;j<240;j++)
	{

		for(i=0;i<width;i++)
		{
		
			u32 v=(j+i*240)*3;
			//u32 u = (j*width + i)*3;
			fwrite(&bufAdr[v], sizeof(bufAdr[v]), 3, f);
			//printf("\x1b[15;0HInit");
			//memcpy(framebuf + u,bufAdr + v, 3);
			//printf("\x1b[15;0HMem ");
			//fwrite(framebuf, sizeof(framebuf), 1, f);
			//printf("\x1b[15;0HFile");
		}
	}
}

void write_framebuffer_RGB565(FILE* f, u32 width)
{
	
	u8* bufAdr;
	if(width == 320)	
	{
		bufAdr = gfxGetFramebuffer(GFX_BOTTOM, GFX_LEFT, NULL, NULL);
	}
	else
	{
		bufAdr = gfxGetFramebuffer(GFX_TOP, GFX_LEFT, NULL, NULL);	
	}

	
	u8* white = (u8*) malloc(3);			
	white[0] = 0xFF;
	white[1] = 0xFF;
	white[2] = 0xFF;

	u8* black = (u8*) malloc(3);
	black[0] = 0x00;
	black[1] = 0x00;
	black[2] = 0x00;

	int i, j;

	//todo make actually work if colors exist on top screen in future
	for(j=0;j<240;j++)
	{

		for(i=0;i<width;i++)
		{
		
			u32 v=(j+i*240)*2;
	

			if(bufAdr[v])
			{
				fwrite(white, sizeof(white[0]), 3, f);
			}
			else
			{
				fwrite(black, sizeof(black[0]), 3, f);
			}
		}
	}

	free(white);
	free(black);

}
