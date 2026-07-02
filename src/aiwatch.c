/*
 * AIWATCH.EXE - PC-98 FM音源デモ「AI Watch Intro」
 * QuuBee (https://quubee.pages.dev) 向け。DOS INT 21h の範囲のみで完結する。
 *
 * 構成: タイトル(ロゴ+サブタイトル) -> トピック紹介 -> エンディング。
 * BGM(OPNA直接I/O・オリジナル3パート曲)と帯状スクロールは全画面を通して
 * バックグラウンドで鳴り続ける/流れ続ける。市販/フリーのDOSドライバは
 * 一切使わず、音源・画像変換ツールもすべて自作(tools/以下)。
 *
 * 「Press any key」画面は5秒間入力が無ければ自動的に次へ進む。
 */
#include <dos.h>
#include "music.h"
#include "scroll.h"
#include "gvram.h"
#include "logo.h"
#include "robot.h"

#define AUTO_ADVANCE_CS 500UL   /* 自動遷移までの待ち時間(センチ秒=1/100秒)。5秒 */
#define CS_PER_DAY (86400UL * 100UL)

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

/* AH=2Ch: DOSの現在時刻をセンチ秒(1/100秒)単位に変換して返す */
static unsigned long get_time_cs(void)
{
    union REGS r;
    r.h.ah = 0x2C;
    intdos(&r, &r);
    return ((unsigned long)r.h.ch * 3600UL + (unsigned long)r.h.cl * 60UL
            + (unsigned long)r.h.dh) * 100UL + (unsigned long)r.h.dl;
}

/* startからの経過センチ秒。日付をまたいだ場合も一応考慮する */
static unsigned long elapsed_cs(unsigned long start)
{
    unsigned long now = get_time_cs();
    if (now >= start) {
        return now - start;
    }
    return (CS_PER_DAY - start) + now;
}

/* タイトル画面のロゴ色サイクル(1秒おき)。s_logo_active中のみ動作する */
static const int LOGO_COLORS[] = { 5, 1, 2, 3, 4, 6 }; /* マゼンタ/青/緑/シアン/赤/黄 (白・黒は使わない) */
#define LOGO_COLOR_COUNT (int)(sizeof(LOGO_COLORS) / sizeof(LOGO_COLORS[0]))
static int s_logo_active = 0;
static int s_logo_idx = 0;
static unsigned long s_logo_last_cs;

static void logo_cycle_tick(void)
{
    if (!s_logo_active) {
        return;
    }
    if (elapsed_cs(s_logo_last_cs) >= 100UL) {   /* 100センチ秒=1秒 */
        s_logo_last_cs = get_time_cs();
        s_logo_idx = (s_logo_idx + 1) % LOGO_COLOR_COUNT;
        logo_draw(LOGO_COLORS[s_logo_idx]);
    }
}

/* トピック画面のロボットアニメーション。s_robot_active中のみ動作する */
static int s_robot_active = 0;

/* BGM・スクロール・(該当画面なら)ロゴ色サイクル/ロボットを1ティックぶん進める。
 * 画面上の「待ち」は必ずこれ経由にして、演出が止まらないようにする。 */
static void frame_wait(void)
{
    music_tick();
    scroll_step();
    logo_cycle_tick();
    if (s_robot_active) {
        robot_step();
    }
    wait(MUSIC_TICK_WAIT);
}

static void wait_ticks(int n)
{
    int i;
    for (i = 0; i < n; i++) {
        frame_wait();
    }
}

/* AH=0Bh: 入力バッファにキーがあれば AL=0xFF (非ブロッキング) */
static int key_pressed(void)
{
    union REGS r;
    r.h.ah = 0x0B;
    intdos(&r, &r);
    return r.h.al != 0;
}

/* キーが押されるか、timeout_cs(センチ秒)経過したら戻る。
 * その間もBGM/スクロール等は進み続ける。 */
static void wait_key_or_timeout(unsigned long timeout_cs)
{
    unsigned long start = get_time_cs();
    union REGS r;

    while (!key_pressed()) {
        frame_wait();
        if (elapsed_cs(start) >= timeout_cs) {
            return;   /* タイムアウト: 自動的に次の画面へ */
        }
    }
    r.h.ah = 0x07;   /* バッファ済みのキーを読み捨てる(ノンブロッキングで即返る) */
    intdos(&r, &r);
}

static void cls(void)
{
    dprint("\x1b[2J$");   /* ESC[2J: 画面消去 (QuuBee dos_int21.c の CSI 'J' 相当) */
}

int main(void)
{
    music_init();
    scroll_init("               "
                "AI ﾉ ｻｲｼﾝ ﾄﾞｳｺｳ ｦ ﾄﾄﾞｹﾙ ｿｳｺﾞｳ ｼﾞｮｳﾎｳ ｻｲﾄ｡"
                "  AI Watch  https://ai.watch.impress.co.jp/     ");

    /* ---- タイトル画面 ---- */
    cls();
    gvram_enable();
    gvram_clear();
    logo_draw(LOGO_COLORS[0]);   /* "AI WATCH" ロゴ(白文字)をグラフィック画面に描画 */

    dprint("\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n$");
    dprint("                     生成AI・最新テクノロジー総合情報サイト\r\n$");
    dprint("\r\n\r\n$");
    dprint("                         Presented by pc98-aiwatch-demo\r\n$");
    dprint("\r\n\r\n                              -- Press any key --\r\n$");

    s_logo_idx = 0;
    s_logo_last_cs = get_time_cs();
    s_logo_active = 1;
    wait_key_or_timeout(AUTO_ADVANCE_CS);
    s_logo_active = 0;

    /* ---- トピック紹介画面 ---- */
    cls();
    gvram_clear();
    robot_init();
    s_robot_active = 1;
    dprint("\r\n  AI Watch の主要カテゴリ\r\n$");
    dprint("  ======================\r\n\r\n$");

    dprint("  ・生成AI\r\n$");
    wait_ticks(12);
    dprint("      最新の生成AIサービス・モデルのニュースをいち早く\r\n\r\n$");
    wait_ticks(12);

    dprint("  ・開発・インフラ\r\n$");
    wait_ticks(12);
    dprint("      AI開発を支えるツール・クラウド・インフラの動向\r\n\r\n$");
    wait_ticks(12);

    dprint("  ・AI活用\r\n$");
    wait_ticks(12);
    dprint("      ビジネス・現場でのAI活用事例を幅広く紹介\r\n\r\n$");
    wait_ticks(12);

    dprint("\r\n                              -- Press any key --\r\n$");
    wait_key_or_timeout(AUTO_ADVANCE_CS);
    s_robot_active = 0;

    /* ---- エンディング画面 ---- */
    cls();
    gvram_clear();
    dprint("\r\n\r\n\r\n$");
    dprint("                           AI Watch でチェックしよう\r\n\r\n$");
    dprint("                        https://ai.watch.impress.co.jp/\r\n\r\n\r\n$");
    dprint("                                   Thank you!\r\n$");
    wait_ticks(32);

    return 0;
}
