#ifndef LOGO_H
#define LOGO_H

/* グラフィックVRAMに"AI WATCH"ロゴを描画する。badge_color(0-7)でバッジ背景の
 * 色を指定できる(文字は常に白固定)。同じ位置に再描画すれば色の変更になる。 */
void logo_draw(int badge_color);

#endif
