#include <string.h>
#include <dos.h>
#include "scroll.h"

#define SCROLL_WIDTH 70   /* 80桁中70桁を使い、左右に少し余白を残す */

static const char *s_msg;
static int s_msg_len;
static int s_pos;

static void dprint(const char *s)
{
    bdos(0x09, (unsigned)s, 0);
}

void scroll_init(const char *msg)
{
    s_msg = msg;
    s_msg_len = (int)strlen(msg);
    s_pos = 0;
}

void scroll_step(void)
{
    char buf[SCROLL_WIDTH + 1];
    int i;

    for (i = 0; i < SCROLL_WIDTH; i++) {
        buf[i] = s_msg[(s_pos + i) % s_msg_len];
    }
    buf[SCROLL_WIDTH] = '$';   /* AH=09h の終端記号。NUL終端は不要 */

    /* ESC[s で呼び出し元のカーソル位置を保存してから25行目(最下行)へ移動して描画し、
     * ESC[u で元の位置に戻す。これをしないと、スクロール描画後にカーソルが25行目に
     * 残ったまま呼び出し元がテキストを続けて出力し、25行目末尾で改行するたびに画面が
     * 上へスクロールしてしまう(過去のスクロール行が画面上に残像として積み上がるバグ)。 */
    dprint("\x1b[s\x1b[25;5H$");
    bdos(0x09, (unsigned)buf, 0);
    dprint("\x1b[u$");

    s_pos++;
    if (s_pos >= s_msg_len) {
        s_pos = 0;
    }
}
