/*
 * opna.c - OPNA(YM2608)直接I/Oポート制御の最小実装
 * FM音源部(OPN互換)のみ使用。ADPCM/リズム音源・SSGは使わない。
 */
#include <conio.h>
#include "opna.h"

#define PORT_A_ADDR 0x188
#define PORT_A_DATA 0x18A
#define PORT_B_ADDR 0x18C
#define PORT_B_DATA 0x18E

/* OPNAはアドレス書き込み後、実際にレジスタへ反映されるまで数マイクロ秒
 * かかる(ウェイトステート)。実機ドライバはビジーループで数クロック待つのが
 * 定石。QuuBeeのエミュレーションでも安全のため短いダミーループを挟む。 */
static void opna_wait(void)
{
    volatile int i;
    for (i = 0; i < 8; i++) {
    }
}

void opna_write(int set, unsigned char reg, unsigned char data)
{
    unsigned addr_port = (set == 0) ? PORT_A_ADDR : PORT_B_ADDR;
    unsigned data_port = (set == 0) ? PORT_A_DATA : PORT_B_DATA;
    outp(addr_port, reg);
    opna_wait();
    outp(data_port, data);
    opna_wait();
}

void opna_init(void)
{
    opna_write(0, 0x27, 0x00);   /* タイマ停止・チャンネル3通常モード */
    opna_write(0, 0x29, 0x80);   /* IRQマスク: FM(OPNA)割り込みを許可(念のため) */
}

/* channel(0-5) → レジスタセット(0=A/1=B)とセット内チャンネル番号(0-2) */
static int opna_set(int channel)      { return (channel < 3) ? 0 : 1; }
static int opna_chofs(int channel)    { return channel % 3; }

/* キーオン/オフレジスタ(0x28)向けのチャンネルコード:
 * ch0-2 → 0,1,2 / ch3-5(セットB=物理ch4-6) → 4,5,6 */
static unsigned opna_keycode(int channel)
{
    int ofs = opna_chofs(channel);
    return (channel < 3) ? (unsigned)ofs : (unsigned)(0x04 | ofs);
}

void opna_set_patch(int channel, unsigned char ar_rs, unsigned char dr,
                     unsigned char sr, unsigned char sl_rr, unsigned char tl)
{
    int set = opna_set(channel);
    int ch  = opna_chofs(channel);

    /* アルゴリズム7・フィードバック0: 4オペレータ全てキャリア(加算合成)。
     * スロット1-3は使わず無音化しておき、スロット4だけを鳴らす最小構成。
     * スロット4のDT/MUL=1は固定(キャリアなのでMULを変えると音程自体が
     * ずれてしまうため、音色の違いはエンベロープ(AR/DR/SR/SL/RR)とTLだけで付ける)。 */
    opna_write(set, (unsigned char)(0xB0 + ch), 0x07);

    opna_write(set, (unsigned char)(0x40 + ch), 127); /* slot1 TL: 無音化 */
    opna_write(set, (unsigned char)(0x44 + ch), 127); /* slot2 TL: 無音化 */
    opna_write(set, (unsigned char)(0x48 + ch), 127); /* slot3 TL: 無音化 */

    opna_write(set, (unsigned char)(0x3C + ch), 0x01); /* slot4 DT=0, MUL=1 */
    opna_write(set, (unsigned char)(0x4C + ch), tl);
    opna_write(set, (unsigned char)(0x5C + ch), ar_rs);
    opna_write(set, (unsigned char)(0x6C + ch), dr);
    opna_write(set, (unsigned char)(0x7C + ch), sr);
    opna_write(set, (unsigned char)(0x8C + ch), sl_rr);
    opna_write(set, (unsigned char)(0x9C + ch), 0x00); /* SSG-EGは使わない */
}

void opna_set_tone(int channel)
{
    opna_set_patch(channel, 0x1F, 0x07, 0x02, 0x07, 4);
}

void opna_note_on(int channel, unsigned fnum, unsigned block)
{
    int set = opna_set(channel);
    int ch  = opna_chofs(channel);
    unsigned char fnum_l = (unsigned char)(fnum & 0xFF);
    unsigned char fnum_h = (unsigned char)(((block & 0x07) << 3) | ((fnum >> 8) & 0x07));

    opna_write(set, (unsigned char)(0xA4 + ch), fnum_h); /* Block/FnumHigh 先に書く */
    opna_write(set, (unsigned char)(0xA0 + ch), fnum_l); /* FnumLow */

    opna_write(0, 0x28, (unsigned char)(0xF0 | opna_keycode(channel))); /* 全スロット キーオン */
}

void opna_note_off(int channel)
{
    opna_write(0, 0x28, (unsigned char)(0x00 | opna_keycode(channel))); /* 全スロット キーオフ */
}
