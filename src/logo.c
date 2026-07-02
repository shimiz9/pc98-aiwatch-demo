#include "gvram.h"
#include "logo.h"

/* Y座標はすべて半分の値で指定している(QuuBee上のグラフィック画面は
 * 縦200ラインを2倍に引き伸ばして400ライン相当として表示するため)。 */
void logo_draw(void)
{
    gvram_fillrect(144, 30, 495, 56, 5); /* マゼンタのバッジ背景 */

    gvram_fillrect(170, 36, 187, 38, 7); /* 白文字 "AI WATCH" */
    gvram_fillrect(200, 36, 229, 38, 7); /* 白文字 "AI WATCH" */
    gvram_fillrect(254, 36, 259, 38, 7); /* 白文字 "AI WATCH" */
    gvram_fillrect(290, 36, 295, 38, 7); /* 白文字 "AI WATCH" */
    gvram_fillrect(308, 36, 325, 38, 7); /* 白文字 "AI WATCH" */
    gvram_fillrect(338, 36, 379, 38, 7); /* 白文字 "AI WATCH" */
    gvram_fillrect(392, 36, 421, 38, 7); /* 白文字 "AI WATCH" */
    gvram_fillrect(434, 36, 439, 38, 7); /* 白文字 "AI WATCH" */
    gvram_fillrect(470, 36, 475, 38, 7); /* 白文字 "AI WATCH" */
    gvram_fillrect(164, 39, 169, 41, 7); /* 白文字 "AI WATCH" */
    gvram_fillrect(188, 39, 193, 41, 7); /* 白文字 "AI WATCH" */
    gvram_fillrect(212, 39, 217, 41, 7); /* 白文字 "AI WATCH" */
    gvram_fillrect(254, 39, 259, 41, 7); /* 白文字 "AI WATCH" */
    gvram_fillrect(290, 39, 295, 41, 7); /* 白文字 "AI WATCH" */
    gvram_fillrect(302, 39, 307, 41, 7); /* 白文字 "AI WATCH" */
    gvram_fillrect(326, 39, 331, 41, 7); /* 白文字 "AI WATCH" */
    gvram_fillrect(356, 39, 361, 41, 7); /* 白文字 "AI WATCH" */
    gvram_fillrect(386, 39, 391, 41, 7); /* 白文字 "AI WATCH" */
    gvram_fillrect(434, 39, 439, 41, 7); /* 白文字 "AI WATCH" */
    gvram_fillrect(470, 39, 475, 41, 7); /* 白文字 "AI WATCH" */
    gvram_fillrect(164, 42, 193, 44, 7); /* 白文字 "AI WATCH" */
    gvram_fillrect(212, 42, 217, 44, 7); /* 白文字 "AI WATCH" */
    gvram_fillrect(254, 42, 259, 44, 7); /* 白文字 "AI WATCH" */
    gvram_fillrect(272, 42, 277, 44, 7); /* 白文字 "AI WATCH" */
    gvram_fillrect(290, 42, 295, 44, 7); /* 白文字 "AI WATCH" */
    gvram_fillrect(302, 42, 331, 44, 7); /* 白文字 "AI WATCH" */
    gvram_fillrect(356, 42, 361, 44, 7); /* 白文字 "AI WATCH" */
    gvram_fillrect(386, 42, 391, 44, 7); /* 白文字 "AI WATCH" */
    gvram_fillrect(434, 42, 475, 44, 7); /* 白文字 "AI WATCH" */
    gvram_fillrect(164, 45, 169, 47, 7); /* 白文字 "AI WATCH" */
    gvram_fillrect(188, 45, 193, 47, 7); /* 白文字 "AI WATCH" */
    gvram_fillrect(212, 45, 217, 47, 7); /* 白文字 "AI WATCH" */
    gvram_fillrect(254, 45, 259, 47, 7); /* 白文字 "AI WATCH" */
    gvram_fillrect(266, 45, 271, 47, 7); /* 白文字 "AI WATCH" */
    gvram_fillrect(278, 45, 283, 47, 7); /* 白文字 "AI WATCH" */
    gvram_fillrect(290, 45, 295, 47, 7); /* 白文字 "AI WATCH" */
    gvram_fillrect(302, 45, 307, 47, 7); /* 白文字 "AI WATCH" */
    gvram_fillrect(326, 45, 331, 47, 7); /* 白文字 "AI WATCH" */
    gvram_fillrect(356, 45, 361, 47, 7); /* 白文字 "AI WATCH" */
    gvram_fillrect(386, 45, 391, 47, 7); /* 白文字 "AI WATCH" */
    gvram_fillrect(434, 45, 439, 47, 7); /* 白文字 "AI WATCH" */
    gvram_fillrect(470, 45, 475, 47, 7); /* 白文字 "AI WATCH" */
    gvram_fillrect(164, 48, 169, 50, 7); /* 白文字 "AI WATCH" */
    gvram_fillrect(188, 48, 193, 50, 7); /* 白文字 "AI WATCH" */
    gvram_fillrect(200, 48, 229, 50, 7); /* 白文字 "AI WATCH" */
    gvram_fillrect(254, 48, 259, 50, 7); /* 白文字 "AI WATCH" */
    gvram_fillrect(290, 48, 295, 50, 7); /* 白文字 "AI WATCH" */
    gvram_fillrect(302, 48, 307, 50, 7); /* 白文字 "AI WATCH" */
    gvram_fillrect(326, 48, 331, 50, 7); /* 白文字 "AI WATCH" */
    gvram_fillrect(356, 48, 361, 50, 7); /* 白文字 "AI WATCH" */
    gvram_fillrect(392, 48, 421, 50, 7); /* 白文字 "AI WATCH" */
    gvram_fillrect(434, 48, 439, 50, 7); /* 白文字 "AI WATCH" */
    gvram_fillrect(470, 48, 475, 50, 7); /* 白文字 "AI WATCH" */
}
/* badge bottom(実座標系)= 56 -> 表示上は約112px相当 */
