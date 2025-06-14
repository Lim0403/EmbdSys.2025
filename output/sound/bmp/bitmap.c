#include <stdio.h>
#include <stdlib.h>
#include "bitmap.h"

#pragma pack(push, 1)
typedef struct {
    unsigned short bfType;
    unsigned int bfSize;
    unsigned short bfReserved1;
    unsigned short bfReserved2;
    unsigned int bfOffBits;
} BITMAPFILEHEADER;

typedef struct {
    unsigned int biSize;
    int biWidth;
    int biHeight;
    unsigned short biPlanes;
    unsigned short biBitCount;
    unsigned int biCompression;
    unsigned int biSizeImage;
    int biXPelsPerMeter;
    int biYPelsPerMeter;
    unsigned int biClrUsed;
    unsigned int biClrImportant;
} BITMAPINFOHEADER;
#pragma pack(pop)

int draw_bmp_fullscreen(const char *filename, unsigned char *fbmem, int fb_width, int fb_height, int bpp, int line_length) {
    return draw_bmp_partial(filename, fbmem, 0, 0, fb_width, fb_height, bpp, line_length);
}

int draw_bmp_partial(const char *filename, unsigned char *fbmem, int start_x, int start_y, int fb_width, int fb_height, int bpp, int line_length) {
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        perror("fopen");
        return -1;
    }

    BITMAPFILEHEADER bmpHeader;
    BITMAPINFOHEADER bmpInfo;

    fread(&bmpHeader, sizeof(BITMAPFILEHEADER), 1, fp);
    fread(&bmpInfo, sizeof(BITMAPINFOHEADER), 1, fp);

    if (bmpHeader.bfType != 0x4D42 || bmpInfo.biBitCount != 24) {
        printf("Unsupported BMP format\n");
        fclose(fp);
        return -1;
    }

    fseek(fp, bmpHeader.bfOffBits, SEEK_SET);
    int bmp_width = bmpInfo.biWidth;
    int bmp_height = bmpInfo.biHeight;
    int padding = (4 - (bmp_width * 3) % 4) % 4;

    for (int y = 0; y < bmp_height; y++) {
        for (int x = 0; x < bmp_width; x++) {
            unsigned char bgr[3];
            fread(bgr, 1, 3, fp);

            int fb_x = start_x + x;
            int fb_y = start_y + (bmp_height - 1 - y); // BMP는 bottom-up

            if (fb_x >= fb_width || fb_y >= fb_height) continue;

            int offset = fb_y * line_length + fb_x * (bpp / 8);
            fbmem[offset + 0] = bgr[0]; // Blue
            fbmem[offset + 1] = bgr[1]; // Green
            fbmem[offset + 2] = bgr[2]; // Red
            fbmem[offset + 3] = 0x00;   // Alpha or padding
        }
        fseek(fp, padding, SEEK_CUR);
    }

    fclose(fp);
    return 0;
}
