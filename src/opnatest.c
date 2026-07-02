/*
 * opnatest.c - OPNA直接I/O発音の最小検証プログラム(フェーズ4)
 * channel0でA4(約440Hz)を1秒ほど鳴らして消音するだけ。
 * 市販/フリーのDOSドライバは一切使わず、opna.c(自作)のみで発音する。
 */
#include <dos.h>
#include "opna.h"

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
    dprint("OPNA test: channel0 A4 (approx 440Hz)\r\n$");

    opna_init();
    opna_set_tone(0);

    /* A4 は約440Hz相当: fnum=520, block=4
     * (OPNAクロック7.9872MHzを想定して Fnum = freq*144*2^(20-block)/clock で算出) */
    opna_note_on(0, 520, 4);
    wait(15000000UL);
    opna_note_off(0);

    dprint("done.\r\n$");
    return 0;
}
