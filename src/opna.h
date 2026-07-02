/*
 * opna.h - PC-98オンボードOPNA(YM2608)への直接I/Oポートアクセス
 * 市販/フリーのDOSドライバ(PMD/MXDRV等)には一切依存しない自作の最小プレイヤー。
 *
 * ポート配置(QuuBeeのNP2kai本体 core/np2kai/cbus/board86.c で確認済み):
 *   0x188 = レジスタセットA アドレス書き込み/ステータス読み出し (ch1-3, リズム, etc.)
 *   0x18A = レジスタセットA データ書き込み
 *   0x18C = レジスタセットB アドレス書き込み (ch4-6, 拡張)
 *   0x18E = レジスタセットB データ書き込み
 */
#ifndef OPNA_H
#define OPNA_H

/* レジスタセットA(0=ch1-3側)/B(1=ch4-6側)へ1バイト書き込む */
void opna_write(int set, unsigned char reg, unsigned char data);

/* 初期化: タイマ停止・通常モードに設定するのみ(チップ自体のリセットは無い) */
void opna_init(void);

/* channel(0-5) の音色(オペレータ4基=キャリア/モジュレータ)を設定(デフォルト音色) */
void opna_set_tone(int channel);

/* channel(0-5) にエンベロープ・音量を指定して音色を設定。
 * ar_rs: AR(0-31)下位ビット、rs=0固定。 dr/sr: DR/SR(各0-31)。
 * sl_rr: (SL<<4)|RR にパックした値。 tl: TotalLevel(0=最大音量,127=無音)。 */
void opna_set_patch(int channel, unsigned char ar_rs, unsigned char dr,
                     unsigned char sr, unsigned char sl_rr, unsigned char tl);

/* channel(0-5) を fnum/block で発音開始 (レジスタA0h系/28h) */
void opna_note_on(int channel, unsigned fnum, unsigned block);

/* channel(0-5) を消音 */
void opna_note_off(int channel);

#endif
