/*
 * gvramtest.c - グラフィックVRAM描画の検証(フェーズ10)
 * 画面上部にPC-98標準デジタル8色のカラーバーを表示し、
 * プレーン配置(0xA800=B/0xB000=R/0xB800=G)とビット割当が
 * 想定通りか確認する。あわせてマゼンタの矩形+白十字も描く。
 */
#include <dos.h>
#include "gvram.h"

static void dprint(const char *s)
{
    bdos(0x09, (unsigned)s, 0);
}

static void wait_key(void)
{
    union REGS r;
    r.h.ah = 0x07;
    intdos(&r, &r);
}

int main(void)
{
    int i;

    dprint("\x1b[2J$");
    dprint("\r\n  GVRAM test: 8-color bars + magenta box\r\n$");
    dprint("  color order (left->right): black blue green cyan red magenta yellow white\r\n$");
    dprint("  press any key to exit\r\n$");

    gvram_enable();
    gvram_clear();

    /* 上部に8色バー(色0-7、各80px幅) */
    for (i = 0; i < 8; i++) {
        gvram_fillrect(i * 80, 20, i * 80 + 78, 100, i);
    }

    /* マゼンタ(5)の矩形+白(7)の十字 */
    gvram_fillrect(200, 150, 400, 250, 5);
    gvram_fillrect(295, 150, 305, 250, 7);
    gvram_fillrect(200, 195, 400, 205, 7);

    wait_key();
    return 0;
}
