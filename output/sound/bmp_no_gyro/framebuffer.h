#ifndef __FRAMEBUFFER_H__
#define __FRAMEBUFFER_H__

int init_framebuffer(const char *fb_path, unsigned char **fbmem,
                     int *width, int *height, int *bpp, int *line_length, int *mem_size);

void release_framebuffer(unsigned char *fbmem, int fbfd, int mem_size);

#endif
