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

    dprint("\x1b[25;5H$");     /* カーソルを25行目(最下行)5桁目へ (1-based) */
    bdos(0x09, (unsigned)buf, 0);

    s_pos++;
    if (s_pos >= s_msg_len) {
        s_pos = 0;
    }
}
