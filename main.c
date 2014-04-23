/*
  Simple framebuffer testing program
 
  Set the screen to a given color. For use in developing Linux
  display drivers.
  
  Copyright (c) 2014, Jumpnow Technologies, LLC 
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions 
  are met:

  1. Redistributions of source code must retain the above copyright 
     notice, this list of conditions and the following disclaimer.

  2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
  HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
  TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

  Ideas taken from the Yocto Project psplash program.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h> 
#include <sys/mman.h>
#include <linux/fb.h>

struct fb_config
{
	int fd;
	int width;
	int height;
	int bpp;
	int stride;
	int red_offset;
	int red_length;
	int green_offset;
	int green_length;
	int blue_offset;
	int blue_length;
	char *data;
	char *base;
};

void dump_vscreeninfo(struct fb_var_screeninfo *fvsi)
{
	printf("======= FB VAR SCREENINFO =======\n");
	printf("xres: %d\n", fvsi->xres);
	printf("yres: %d\n", fvsi->yres);
	printf("bpp : %d\n", fvsi->bits_per_pixel);
	printf("red bits    :\n");
	printf("    offset   : %d\n", fvsi->red.offset);
	printf("    length   : %d\n", fvsi->red.length);
	printf("    msb_right: %d\n", fvsi->red.msb_right);	
	printf("green bits  :\n");
	printf("    offset   : %d\n", fvsi->green.offset);
	printf("    length   : %d\n", fvsi->green.length);
	printf("    msb_right: %d\n", fvsi->green.msb_right);	
	printf("blue bits   :\n");
	printf("    offset   : %d\n", fvsi->blue.offset);
	printf("    length   : %d\n", fvsi->blue.length);
	printf("    msb_right: %d\n", fvsi->blue.msb_right);	
	printf("transp bits :\n");
	printf("    offset   : %d\n", fvsi->transp.offset);
	printf("    length   : %d\n", fvsi->transp.length);
	printf("    msb_right: %d\n", fvsi->transp.msb_right);	
	
	printf("=================================\n");
}

void dump_fscreeninfo(struct fb_fix_screeninfo *ffsi)
{
	printf("======= FB FIX SCREENINFO =======\n");
	printf("id          : %s\n", ffsi->id);
	printf("smem_start  : 0x%08lX\n", ffsi->smem_start);
	printf("smem_len    : %u\n", ffsi->smem_len);
	printf("line_length : %u\n", ffsi->line_length);
	printf("=================================\n");
}

void plot_pixel(struct fb_config *fb, int x, int y, int r, int g, int b)
{
	int offset = (y * fb->stride) + (x * (fb->bpp >> 3)); 

	*(fb->data + offset) = b;
	*(fb->data + offset + 1) = g;
	*(fb->data + offset + 2) = r;
}

void draw_rect(struct fb_config *fb, int x, int y, int w, int h, int r, int g, int b)
{
	int dx, dy;

	for (dy = 0; dy < h; dy++) {
		for (dx = 0; dx < w; dx++) {
			plot_pixel(fb, x + dx, y + dy, r, g, b);
		}
	}  
}

void clear_screen(struct fb_config *fb, int r, int g, int b)
{
	draw_rect(fb, 0, 0, fb->width, fb->height, r, g, b);
}

int main(int argc, char **argv)
{
	int fd, red, green, blue;
	unsigned long offset;
	struct fb_var_screeninfo fvsi;
	struct fb_fix_screeninfo ffsi;
	struct fb_config fb;

	red = green = blue = 0;

	if (argc > 1) {
		red = 0xff & atoi(argv[1]);	

		if (argc > 2) {
			green = 0xff & atoi(argv[2]);	

			if (argc > 3)
				blue = 0xff & atoi(argv[3]);
		}
	}

	memset(&fb, 0, sizeof(fb));

	if ((fd = open("/dev/fb0", O_RDWR)) < 0) {
		perror("open");
		exit(1);
	}

	if (ioctl(fd, FBIOGET_VSCREENINFO, &fvsi) < 0) {
		perror("ioctl(FBIOGET_VSCREENINFO)");	
		close(fd);
		exit(1);
	}

	// dump_vscreeninfo(&fvsi);

	if (ioctl(fd, FBIOGET_FSCREENINFO, &ffsi) < 0) {
		perror("ioctl(FBIOGET_FSCREENINFO)");
		close(fd);
		exit(1);
	}

	// dump_fscreeninfo(&ffsi);

	fb.fd = fd;
	fb.width = fvsi.xres;
	fb.height = fvsi.yres;
	fb.bpp = fvsi.bits_per_pixel;
	fb.stride = ffsi.line_length;
	fb.red_offset = fvsi.red.offset;
	fb.red_length = fvsi.red.length;
	fb.green_offset = fvsi.green.offset;
	fb.green_length = fvsi.green.length;
	fb.blue_offset = fvsi.blue.offset;
	fb.blue_length = fvsi.blue.length;

	fb.base = (char *)mmap((caddr_t) NULL, fb.stride * fb.height,
			PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	if (fb.base == (char *)-1) {
		perror("mmap");
		close(fd);
		exit(1);
	}

	offset = (unsigned long) ffsi.smem_start % (unsigned long) getpagesize();

	fb.data = fb.base + offset;

	clear_screen(&fb, red, green, blue);

	close(fd);

	return 0;
}

