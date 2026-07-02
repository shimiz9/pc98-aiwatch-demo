/*
 * musictest.c - オリジナルBGM(3パート)の検証プログラム(フェーズ6)
 * 4小節ループ(C-G-Am-F)を4周ぶん再生して停止する。
 */
#include <dos.h>
#include "music.h"

/* 1ティック(8分音符)ぶんのウェイト。QuuBee実機での聴感に合わせて要調整。
 * 132BPM相当を狙った暫定値。速すぎ/遅すぎる場合はここを増減する。 */
#define TICK_WAIT 600000UL

static void dprint(const char *s)
{
    bdos(0x09, (unsigned)s, 0);
}

static void wait(unsigned long loops)
{
    volatile unsigned long i;
    for (i = 0; i < loops; i++) {
    }
}

int main(void)
{
    int i;

    dprint("\x1b[2J$");
    dprint("\r\n  Music test: C-G-Am-F loop x4\r\n$");

    music_init();

    for (i = 0; i < 32 * 4; i++) {   /* 1小節8ティック x 4小節 x 4周 */
        music_tick();
        wait(TICK_WAIT);
    }

    dprint("\r\n  done.\r\n$");
    return 0;
}
