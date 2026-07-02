/*
 * scrolltest.c - 帯状スクロールの検証プログラム(フェーズ5)
 * 半角カナ+半角英数のキャッチコピーを画面最下行で横スクロール表示する。
 */
#include <dos.h>
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

int main(void)
{
    int i;

    dprint("\x1b[2J$");
    dprint("\r\n  Scroll test\r\n$");

    /* 半角カナのみで構成: "AIノサイシンドウコウオトドケルソウゴウジョウホウサイト。AI Watch" */
    scroll_init("               "
                "AI ﾉ ｻｲｼﾝ ﾄﾞｳｺｳ ｦ ﾄﾄﾞｹﾙ ｿｳｺﾞｳ ｼﾞｮｳﾎｳ ｻｲﾄ｡"
                "  AI Watch  https://ai.watch.impress.co.jp/     ");

    for (i = 0; i < 400; i++) {
        scroll_step();
        wait(400000UL);
    }

    dprint("\x1b[2J$");
    dprint("\r\n  done.\r\n$");
    return 0;
}
