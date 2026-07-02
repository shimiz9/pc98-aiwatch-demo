/*
 * music.c - オリジナルBGM「ショップデモ風ポップ」の再生データとプレイヤー
 *
 * 構成: C-G-Am-F(4小節ループ)のいわゆる王道進行。
 *   ch0 メロディ: 8分音符主体の跳ねた旋律
 *   ch1 ベース  : ルート-ルート-5度-ルートの四分音符パターン
 *   ch2 コード  : 各小節のコードの3度(ハモリ)を全音符で伸ばす。
 *                 ベースのルート+5度と合わせて簡易的に三和音を構成する。
 *
 * ティック単位 = 8分音符。4/4拍子、1小節=8ティック。
 */
#include <stddef.h>
#include "opna.h"
#include "notes.h"
#include "music.h"

typedef struct {
    unsigned char note;   /* notes.h の NOTE_xx、または NOTE_REST */
    unsigned char dur;    /* 長さ(ティック数) */
} MEvent;

#define EN 1   /* 8分音符 */
#define QN 2   /* 4分音符 */
#define WN 8   /* 全音符(1小節分) */

static const MEvent melody[] = {
    /* Bar1: C */
    { NOTE_C5, EN }, { NOTE_E5, EN }, { NOTE_G5, EN }, { NOTE_E5, EN },
    { NOTE_D5, EN }, { NOTE_C5, EN }, { NOTE_D5, EN }, { NOTE_E5, EN },
    /* Bar2: G */
    { NOTE_G4, EN }, { NOTE_B4, EN }, { NOTE_D5, EN }, { NOTE_B4, EN },
    { NOTE_A4, EN }, { NOTE_G4, EN }, { NOTE_A4, EN }, { NOTE_B4, EN },
    /* Bar3: Am */
    { NOTE_A4, EN }, { NOTE_C5, EN }, { NOTE_E5, EN }, { NOTE_C5, EN },
    { NOTE_B4, EN }, { NOTE_A4, EN }, { NOTE_B4, EN }, { NOTE_C5, EN },
    /* Bar4: F (ループ先頭のCへ戻りやすいよう下降) */
    { NOTE_F4, EN }, { NOTE_A4, EN }, { NOTE_C5, EN }, { NOTE_A4, EN },
    { NOTE_G4, EN }, { NOTE_F4, EN }, { NOTE_G4, EN }, { NOTE_A4, EN },
};

static const MEvent bass[] = {
    { NOTE_C3, QN }, { NOTE_C3, QN }, { NOTE_G3, QN }, { NOTE_C3, QN }, /* Bar1: C */
    { NOTE_G3, QN }, { NOTE_G3, QN }, { NOTE_D4, QN }, { NOTE_G3, QN }, /* Bar2: G */
    { NOTE_A3, QN }, { NOTE_A3, QN }, { NOTE_E4, QN }, { NOTE_A3, QN }, /* Bar3: Am */
    { NOTE_F3, QN }, { NOTE_F3, QN }, { NOTE_C4, QN }, { NOTE_F3, QN }, /* Bar4: F */
};

static const MEvent chord[] = {
    { NOTE_E4, WN },  /* Bar1: Cの3度 */
    { NOTE_B4, WN },  /* Bar2: Gの3度 */
    { NOTE_C5, WN },  /* Bar3: Amの短3度 */
    { NOTE_A4, WN },  /* Bar4: Fの3度 */
};

typedef struct {
    const MEvent *events;
    int len;
    int channel;
    int idx;
    int remaining;
    int playing;
} Track;

static Track s_tracks[3];

static void track_init(Track *t, const MEvent *events, int len, int channel)
{
    t->events = events;
    t->len = len;
    t->channel = channel;
    t->idx = -1;
    t->remaining = 0;
    t->playing = 0;
}

static void track_tick(Track *t)
{
    const MEvent *e;

    if (t->remaining > 0) {
        t->remaining--;
        return;
    }
    if (t->playing) {
        opna_note_off(t->channel);
        t->playing = 0;
    }
    t->idx++;
    if (t->idx >= t->len) {
        t->idx = 0;
    }
    e = &t->events[t->idx];
    if (e->note != NOTE_REST) {
        opna_note_on(t->channel, NOTE_FNUM[e->note], NOTE_BLOCK[e->note]);
        t->playing = 1;
    }
    t->remaining = e->dur - 1;
}

void music_init(void)
{
    opna_init();
    opna_set_patch(0, 0x1F, 0x0A, 0x02, 0x28, 2);   /* melody: 明るいプラック */
    opna_set_patch(1, 0x1F, 0x05, 0x03, 0x4A, 0);   /* bass  : 音量最大の弾む低音 */
    opna_set_patch(2, 0x14, 0x08, 0x04, 0x64, 12);  /* chord : 控えめなパッド */

    track_init(&s_tracks[0], melody, (int)(sizeof(melody) / sizeof(melody[0])), 0);
    track_init(&s_tracks[1], bass,   (int)(sizeof(bass)   / sizeof(bass[0])),   1);
    track_init(&s_tracks[2], chord,  (int)(sizeof(chord)  / sizeof(chord[0])),  2);
}

void music_tick(void)
{
    track_tick(&s_tracks[0]);
    track_tick(&s_tracks[1]);
    track_tick(&s_tracks[2]);
}
