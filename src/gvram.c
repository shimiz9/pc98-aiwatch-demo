#include <conio.h>
#include <i86.h>
#include <string.h>
#include "gvram.h"

#define GVRAM_BYTES_PER_LINE 80   /* 640px / 8 */
#define GVRAM_LINES 400

/* グラフィックGDC(スレーブ)のコマンドポート。0xA2へ書くとgdc.s.fifoへ
 * コマンドとして積まれ即実行される(np2kai io/gdc.c gdc_oa2)。
 * START(0x6Bまたは0x0D)でGDCSCRN_ENABLEが立ち、画面合成に反映される。 */
#define GDC_GRAPHIC_CMD_PORT 0xA2
#define GDC_CMD_START 0x6B

void gvram_enable(void)
{
    outp(GDC_GRAPHIC_CMD_PORT, GDC_CMD_START);
}

static void plane_clear(unsigned seg)
{
    unsigned char __far *vram = (unsigned char __far *)MK_FP(seg, 0);
    _fmemset(vram, 0, (size_t)GVRAM_BYTES_PER_LINE * GVRAM_LINES);
}

void gvram_clear(void)
{
    plane_clear(0xA800);
    plane_clear(0xB000);
    plane_clear(0xB800);
}

static void plane_blit(unsigned seg, int x, int y, int w, int h, const unsigned char *src)
{
    unsigned char __far *vram = (unsigned char __far *)MK_FP(seg, 0);
    int row;
    int byte_x = x / 8;
    int byte_w = w / 8;

    for (row = 0; row < h; row++) {
        long off = (long)(y + row) * GVRAM_BYTES_PER_LINE + byte_x;
        _fmemcpy(vram + off, src + (long)row * byte_w, (size_t)byte_w);
    }
}

void gvram_blit(int x, int y, int w, int h,
                const unsigned char *planeB,
                const unsigned char *planeR,
                const unsigned char *planeG)
{
    if (planeB) plane_blit(0xA800, x, y, w, h, planeB);
    if (planeR) plane_blit(0xB000, x, y, w, h, planeR);
    if (planeG) plane_blit(0xB800, x, y, w, h, planeG);
}

void gvram_setpixel(int x, int y, int color)
{
    unsigned char mask = (unsigned char)(0x80 >> (x % 8));
    long off = (long)y * GVRAM_BYTES_PER_LINE + (x / 8);
    unsigned char __far *b = (unsigned char __far *)MK_FP(0xA800, 0);
    unsigned char __far *g = (unsigned char __far *)MK_FP(0xB800, 0);
    unsigned char __far *r = (unsigned char __far *)MK_FP(0xB000, 0);

    if (color & 1) b[off] |= mask; else b[off] &= (unsigned char)~mask;
    if (color & 2) g[off] |= mask; else g[off] &= (unsigned char)~mask;
    if (color & 4) r[off] |= mask; else r[off] &= (unsigned char)~mask;
}

void gvram_fillrect(int x0, int y0, int x1, int y1, int color)
{
    int x, y;
    for (y = y0; y <= y1; y++) {
        for (x = x0; x <= x1; x++) {
            gvram_setpixel(x, y, color);
        }
    }
}
