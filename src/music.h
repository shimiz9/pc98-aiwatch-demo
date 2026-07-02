/*
 * music.h - オリジナルBGMプレイヤー(3パート: メロディ/ベース/コード)
 * OPNAチャンネル0=メロディ, 1=ベース, 2=コード を使用。
 * データはすべてオリジナル(市販曲・既存ドライバのデータ形式には非依存)。
 */
#ifndef MUSIC_H
#define MUSIC_H

/* 1ティック(8分音符)ぶんのソフトウェアウェイト値。QuuBee実機での聴感で調整済み。
 * このヘッダを使う側は wait(MUSIC_TICK_WAIT) を毎ティック挟むこと。 */
#define MUSIC_TICK_WAIT 250000UL

void music_init(void);

/* 1ティック(8分音符相当)ぶん再生を進める。呼び出し側でwait()を挟むこと */
void music_tick(void);

#endif
