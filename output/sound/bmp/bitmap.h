#ifndef __BITMAP_H__
#define __BITMAP_H__

int draw_bmp_fullscreen(const char *filename, unsigned char *fbmem, int fb_width, int fb_height, int bpp, int line_length);
int draw_bmp_partial(const char *filename, unsigned char *fbmem, int start_x, int start_y, int fb_width, int fb_height, int bpp, int line_length);

#endif
