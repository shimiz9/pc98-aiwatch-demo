/*
 * AIWATCH.EXE - PC-98 FM音源デモ「AI Watch Intro」
 * QuuBee (https://quubee.pages.dev) 向け。DOS INT 21h の範囲のみで完結する。
 *
 * 構成: タイトル(ロゴ+サブタイトル) -> トピック紹介 -> エンディング。
 * BGM(OPNA直接I/O・オリジナル3パート曲)と帯状スクロールは全画面を通して
 * バックグラウンドで鳴り続ける/流れ続ける。市販/フリーのDOSドライバは
 * 一切使わず、音源・画像変換ツールもすべて自作(tools/以下)。
 */
#include <dos.h>
#include "music.h"
#include "scroll.h"

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

/* BGMとスクロールを1ティックぶん進める。画面上の「待ち」は必ずこれ経由にして、
 * 音・スクロールが止まらないようにする。 */
static void frame_wait(void)
{
    music_tick();
    scroll_step();
    wait(MUSIC_TICK_WAIT);
}

static void wait_ticks(int n)
{
    int i;
    for (i = 0; i < n; i++) {
        frame_wait();
    }
}

/* AH=0Bh: 入力バッファにキーがあれば AL=0xFF (非ブロッキング) */
static int key_pressed(void)
{
    union REGS r;
    r.h.ah = 0x0B;
    intdos(&r, &r);
    return r.h.al != 0;
}

/* キーが押されるまでBGM/スクロールを進めながら待つ */
static void wait_key_bg(void)
{
    union REGS r;
    while (!key_pressed()) {
        frame_wait();
    }
    r.h.ah = 0x07;   /* バッファ済みのキーを読み捨てる(ノンブロッキングで即返る) */
    intdos(&r, &r);
}

static void cls(void)
{
    dprint("\x1b[2J$");   /* ESC[2J: 画面消去 (QuuBee dos_int21.c の CSI 'J' 相当) */
}

int main(void)
{
    music_init();
    scroll_init("               "
                "AI ﾉ ｻｲｼﾝ ﾄﾞｳｺｳ ｦ ﾄﾄﾞｹﾙ ｿｳｺﾞｳ ｼﾞｮｳﾎｳ ｻｲﾄ｡"
                "  AI Watch  https://ai.watch.impress.co.jp/     ");

    /* ---- タイトル画面 ---- */
    cls();
    dprint("\r\n\r\n$");
    dprint("               ###  #####   #     #  ###  #######  #####  #     #\r\n$");
    dprint("              #   #   #     #     # #   #    #    #       #     #\r\n$");
    dprint("              #####   #     #  #  # #####    #    #       #######\r\n$");
    dprint("              #   #   #     # # # # #   #    #    #       #     #\r\n$");
    dprint("              #   # #####   #     # #   #    #     #####  #     #\r\n$");
    dprint("\r\n$");
    dprint("                     生成AI・最新テクノロジー総合情報サイト\r\n$");
    dprint("\r\n\r\n$");
    dprint("                         Presented by pc98-aiwatch-demo\r\n$");
    dprint("\r\n\r\n                              -- Press any key --\r\n$");
    wait_key_bg();

    /* ---- トピック紹介画面 ---- */
    cls();
    dprint("\r\n  AI Watch の主要カテゴリ\r\n$");
    dprint("  ======================\r\n\r\n$");

    dprint("  ・生成AI\r\n$");
    wait_ticks(12);
    dprint("      最新の生成AIサービス・モデルのニュースをいち早く\r\n\r\n$");
    wait_ticks(12);

    dprint("  ・開発・インフラ\r\n$");
    wait_ticks(12);
    dprint("      AI開発を支えるツール・クラウド・インフラの動向\r\n\r\n$");
    wait_ticks(12);

    dprint("  ・AI活用\r\n$");
    wait_ticks(12);
    dprint("      ビジネス・現場でのAI活用事例を幅広く紹介\r\n\r\n$");
    wait_ticks(12);

    dprint("\r\n                              -- Press any key --\r\n$");
    wait_key_bg();

    /* ---- エンディング画面 ---- */
    cls();
    dprint("\r\n\r\n\r\n$");
    dprint("                           AI Watch でチェックしよう\r\n\r\n$");
    dprint("                        https://ai.watch.impress.co.jp/\r\n\r\n\r\n$");
    dprint("                                   Thank you!\r\n$");
    wait_ticks(32);

    return 0;
}
