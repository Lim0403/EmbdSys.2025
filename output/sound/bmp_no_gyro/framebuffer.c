#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/fb.h>
#include <unistd.h>
#include "framebuffer.h"

int init_framebuffer(const char *fb_path, unsigned char **fbmem,
                     int *width, int *height, int *bpp, int *line_length, int *mem_size) {
    int fbfd = open(fb_path, O_RDWR);
    if (fbfd < 0) {
        perror("open fb");
        return -1;
    }

    struct fb_var_screeninfo vinfo;
    struct fb_fix_screeninfo finfo;

    ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo);
    ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo);

    *width = vinfo.xres;
    *height = vinfo.yres;
    *bpp = vinfo.bits_per_pixel;
    *line_length = finfo.line_length;
    *mem_size = finfo.smem_len;

    *fbmem = (unsigned char *)mmap(0, *mem_size, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
    if (*fbmem == MAP_FAILED) {
        perror("mmap");
        close(fbfd);
        return -1;
    }

    return fbfd;
}

void release_framebuffer(unsigned char *fbmem, int fbfd, int mem_size) {
    munmap(fbmem, mem_size);
    close(fbfd);
}
