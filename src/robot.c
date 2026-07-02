/*
 * robot.c - トピック画面を左上から右下へ「Z」の軌跡で横切るロボット。
 * Y座標はgvram.h同様、実座標の半分(表示側で縦2倍に引き伸ばされる前提)。
 *
 * ロボットは(x,y)を「頭の左上」を原点とした相対座標で組み立てる。
 * アンテナが原点より上(負のy)にはみ出すので、消去(erase)は
 * バウンディングボックス全体をまとめて黒で塗りつぶす方式にしている。
 */
#include "gvram.h"
#include "robot.h"

#define BOUND_X0   0
#define BOUND_Y0  -4    /* アンテナの先端(玉)ぶん、原点より上にはみ出す */
#define BOUND_X1  31
#define BOUND_Y1  14
#define BOUND_W  (BOUND_X1 - BOUND_X0 + 1)
#define BOUND_H  (BOUND_Y1 - BOUND_Y0 + 1)

#define SEG_STEPS 30
#define TOTAL_STEPS (SEG_STEPS * 3)

#define ROBOT_COLOR 3   /* シアン */
#define EYE_COLOR   0   /* 黒(目) */

typedef struct { int x, y; } POINT;

/* Zの4頂点: 左上 -> 右上 -> 左下 -> 右下(ロボット本体の幅ぶん右端に余裕を持たせる) */
static const POINT P0 = { 20,   8 };
static const POINT P1 = { 588,  8 };
static const POINT P2 = { 20, 176 };
static const POINT P3 = { 588, 176 };

static int s_step;
static int s_last_x = -1;
static int s_last_y = -1;

static void lerp(POINT a, POINT b, int t, int steps, int *ox, int *oy)
{
    *ox = a.x + (b.x - a.x) * t / steps;
    *oy = a.y + (b.y - a.y) * t / steps;
}

static void erase_robot(int x, int y)
{
    gvram_fillrect(x + BOUND_X0, y + BOUND_Y0, x + BOUND_X1, y + BOUND_Y1, 0);
}

static void draw_robot(int x, int y)
{
    /* アンテナ: 支柱+先端の玉 */
    gvram_fillrect(x + 15, y - 3, x + 16, y - 2, ROBOT_COLOR);
    gvram_fillrect(x + 14, y - 4, x + 17, y - 4, ROBOT_COLOR);

    /* 頭 */
    gvram_fillrect(x + 8, y - 1, x + 23, y + 3, ROBOT_COLOR);
    /* 目(頭の色をくり抜くように黒で) */
    gvram_fillrect(x + 11, y,     x + 12, y + 1, EYE_COLOR);
    gvram_fillrect(x + 19, y,     x + 20, y + 1, EYE_COLOR);

    /* 胴体 */
    gvram_fillrect(x + 4, y + 4, x + 27, y + 10, ROBOT_COLOR);

    /* 腕(左右) */
    gvram_fillrect(x + 0, y + 5, x + 3,  y + 9, ROBOT_COLOR);
    gvram_fillrect(x + 28, y + 5, x + 31, y + 9, ROBOT_COLOR);

    /* 脚(左右) */
    gvram_fillrect(x + 8,  y + 11, x + 13, y + 14, ROBOT_COLOR);
    gvram_fillrect(x + 18, y + 11, x + 23, y + 14, ROBOT_COLOR);
}

void robot_init(void)
{
    s_step = 0;
    s_last_x = -1;
    s_last_y = -1;
}

void robot_step(void)
{
    int x, y;
    int t = s_step;

    if (t > TOTAL_STEPS) {
        t = TOTAL_STEPS;
    }

    if (t < SEG_STEPS) {
        lerp(P0, P1, t, SEG_STEPS, &x, &y);
    } else if (t < SEG_STEPS * 2) {
        lerp(P1, P2, t - SEG_STEPS, SEG_STEPS, &x, &y);
    } else {
        lerp(P2, P3, t - SEG_STEPS * 2, SEG_STEPS, &x, &y);
    }

    if (s_last_x >= 0) {
        erase_robot(s_last_x, s_last_y);
    }
    draw_robot(x, y);
    s_last_x = x;
    s_last_y = y;

    if (s_step <= TOTAL_STEPS) {
        s_step++;
    }
}
