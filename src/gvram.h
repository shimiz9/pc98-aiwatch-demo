/*
 * gvram.h - PC-98グラフィックVRAMへの直接描画(BIOS/DOSドライバ非依存)
 *
 * PC-98の標準デジタル8色モードを使用(パレット設定不要、機種標準の固定8色):
 *   セグメント 0xA800 = B(青)プレーン, 0xB000 = R(赤)プレーン, 0xB800 = G(緑)プレーン
 *   (どのプレーンも 1bpp、1行80バイト=640px、Bit7が左端ピクセル)
 * 色は各プレーンのbit ON/OFFの組み合わせで決まる:
 *   000=黒 001=青 010=緑 011=シアン 100=赤 101=マゼンタ 110=黄 111=白
 * 拡張の輝度(I)プレーン(0xE000)は16色モード有効化が別途必要になるため使わない。
 */
#ifndef GVRAM_H
#define GVRAM_H

/* グラフィック画面の表示を有効化する(GDCへSTARTコマンドを送る)。
 * PC-98は電源投入時の状態ではグラフィック画面がGDC的に"停止"扱いになっており、
 * 実機では起動時にBIOS/DOSが自動でSTARTコマンドを送るが、QuuBeeのHLE-DOSは
 * テキスト画面だけを自動有効化しグラフィック画面までは面倒を見ないため、
 * プログラム側で明示的に呼ぶ必要がある。gvram_clear()より先に一度呼べばよい。 */
void gvram_enable(void);

void gvram_clear(void);

/* color: bit0=B, bit1=G, bit2=R (PC-98標準デジタル8色のビット割当) */
void gvram_setpixel(int x, int y, int color);
void gvram_fillrect(int x0, int y0, int x1, int y1, int color);

/* (x,y)を左上として w x h の画像を描画する。x,wは8の倍数であること。
 * planeB/R/G は 1bpp パックデータ(各行 w/8 バイト、h行ぶん)。NULLならそのプレーンは書かない。 */
void gvram_blit(int x, int y, int w, int h,
                 const unsigned char *planeB,
                 const unsigned char *planeR,
                 const unsigned char *planeG);

#endif
