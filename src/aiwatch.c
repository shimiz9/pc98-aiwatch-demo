/*
 * AIWATCH.EXE - PC-98 FM音源デモ「AI Watch Intro」
 * QuuBee (https://quubee.pages.dev) 向け。DOS INT 21h の範囲のみで完結する
 * 最小構成(v0.1): タイトル表示 -> トピック紹介 -> エンディング。
 * 音源(OPNA直接I/O)・帯状スクロール・画像は後続フェーズで追加する。
 */
#include <dos.h>

/* AH=09h: '$'終端文字列表示 */
static void dprint(const char *s)
{
    bdos(0x09, (unsigned)s, 0);
}

/* AH=0Ah相当は使わず、簡易ソフトウェアウェイト(CPU速度依存の暫定値)。
 * i486DX2-50MHz相当を想定した経験値。後でVSYNC同期に置き換える予定。 */
static void wait(unsigned long loops)
{
    volatile unsigned long i;
    for (i = 0; i < loops; i++) {
    }
}

/* AH=0Ch + AL=01h 経由で「1文字待たずに読み捨てる」= 入力バッファをクリアしてから
 * AH=07h(エコーなし1文字入力・待機)でキー入力を待つ */
static void wait_key(void)
{
    union REGS r;
    r.h.ah = 0x07;
    intdos(&r, &r);
}

static void cls(void)
{
    dprint("\x1b[2J$");   /* ESC[2J: 画面消去 (QuuBee dos_int21.c の CSI 'J' 相当) */
}

int main(void)
{
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
    wait_key();

    cls();
    dprint("\r\n  AI Watch の主要カテゴリ\r\n$");
    dprint("  ======================\r\n\r\n$");

    dprint("  ・生成AI\r\n$");
    wait(3000000UL);
    dprint("      最新の生成AIサービス・モデルのニュースをいち早く\r\n\r\n$");
    wait(3000000UL);

    dprint("  ・開発・インフラ\r\n$");
    wait(3000000UL);
    dprint("      AI開発を支えるツール・クラウド・インフラの動向\r\n\r\n$");
    wait(3000000UL);

    dprint("  ・AI活用\r\n$");
    wait(3000000UL);
    dprint("      ビジネス・現場でのAI活用事例を幅広く紹介\r\n\r\n$");
    wait(3000000UL);

    dprint("\r\n                              -- Press any key --\r\n$");
    wait_key();

    cls();
    dprint("\r\n\r\n\r\n$");
    dprint("                           AI Watch でチェックしよう\r\n\r\n$");
    dprint("                        https://ai.watch.impress.co.jp/\r\n\r\n\r\n$");
    dprint("                                   Thank you!\r\n$");
    wait(8000000UL);

    return 0;
}
