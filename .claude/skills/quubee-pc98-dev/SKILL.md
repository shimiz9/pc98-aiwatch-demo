---
name: quubee-pc98-dev
description: Develop PC-98 (MS-DOS, 16-bit real mode) applications that run in QuuBee (https://quubee.pages.dev, a browser-based PC-98/NP2kai emulator, https://github.com/msonrm/quubee). Use when building or debugging a DOS .EXE/.COM for QuuBee's HLE-DOS environment — covers Open Watcom cross-build setup with UTF-8→CP932 conversion, INT 21h HLE API subset, text-screen bugs (missing $ terminator, cursor save/restore), OPNA (YM2608) direct I/O sound programming, tempo/timing calibration, graphics VRAM direct drawing (GDC enable, 200/400-line scaling), and MAG (MAKI02) image format encoding.
---

# QuuBee PC-98 開発

QuuBee(NP2kaiベースのブラウザPC-98エミュレータ)向けにDOSアプリを開発する際の
仕様・落とし穴・対処法。実機PC-98を再現しているが、BIOS/実DOSは無く
INT 21hの一部をHLE(C側で再実装)している独自環境である点が最大の特徴。

不明点があれば、まずQuuBee本体のソースを読むのが最も確実な一次情報源になる:
```bash
git clone --depth 1 https://github.com/msonrm/quubee.git
cd quubee && git submodule update --init --depth 1 core/np2kai   # NP2kai本体(ハードウェアエミュレーション)
```
DOSのHLE実装は`native/dos_int21.c`、OPNA/GVRAM等のハードウェアは`core/np2kai/`以下。

## 開発の進め方

1. **ビルド環境:** Open Watcom V2(16bit DOSクロスコンパイラ)を用意する。
   `scripts/build.sh OUT.EXE src/foo.c [src/bar.c ...]`でビルドできる
   (UTF-8ソース→CP932変換、複数ファイルリンク、zip化まで自動)。
   `OWSETENV`環境変数でOpenWatcomの`owsetenv.sh`パスを指定すること
   (既定 `$HOME/openwatcom/owsetenv.sh`)。
2. **最小構成から始める:** テキスト表示のみ(`AH=09h`)の最小EXEをビルドし、
   QuuBee(https://quubee.pages.dev)にドラッグ&ドロップして動作確認できる
   状態を最初に作る。テキスト・音源・グラフィックの各要素は、まず単体の
   小さなテストプログラムで検証してから本体に統合する(統合してから
   デバッグすると原因の切り分けが難しい)。
3. **段階的に機能追加:** 文字表示 → (必要なら)OPNA音源 → (必要なら)
   帯状スクロール等の演出 → (必要なら)グラフィック描画、の順で進めると
   各段階の問題を切り分けやすい。

## リファレンス(必要に応じて読む)

- **[references/dos-hle.md](references/dos-hle.md)** — INT 21hサポート範囲、
  UTF-8→CP932ビルド変換、`AH=09h`の`$`終端忘れバグ、カーソル位置保存/復元
  (`ESC[s`/`ESC[u`)、全角/半角混在センタリング、半角カナ帯状スクロール。
  **テキスト表示・ビルド関連で問題が起きたら最初に見る。**
- **[references/opna-sound.md](references/opna-sound.md)** — OPNA(YM2608)の
  I/Oポート配置とレジスタ操作、FMキャリアのMULを変えるとピッチがずれる制約、
  Fnum/Block計算式、ソフトウェアウェイトによるテンポ調整の不正確さと
  DOS実時刻(`AH=2Ch`)による正確な時間待ちの使い分け。
  **音源・BGM・正確なタイミング処理で問題が起きたら見る。**
- **[references/graphics-vram.md](references/graphics-vram.md)** — グラフィック
  VRAMのプレーン配置(0xA800/B000/B800)、**GDCへSTARTコマンドを送らないと
  画面に何も表示されない**という非自明な初期化、グラフィック画面が縦200ライン
  を2倍に引き伸ばして表示される座標系の食い違い、MAG(MAKI02)画像形式の
  自作エンコード方法(flagAを全て0にすれば無圧縮で簡単に実装できる)。
  **グラフィック描画・画像変換で問題が起きたら見る。**

## 付属スクリプト

- **[scripts/build.sh](scripts/build.sh)** — 上記のビルドラッパー。
- **[scripts/gen_notes.py](scripts/gen_notes.py)** — 音名(`NOTE_C4`等)から
  OPNAのFnum/Blockを引くC言語テーブルを生成する。OPNA音源プログラミングで
  音階を使う際に実行する。
- **[scripts/png2mag.py](scripts/png2mag.py)** — PNG画像をMAG(MAKI02, 16色,
  無圧縮)形式へ変換する自作コンバータ(Pillow依存)。ロゴ等の静止画をQuuBeeで
  表示したい場合に使う。

## 既知の落とし穴サマリ(詳細は各referenceを参照)

| 症状 | 原因 | 対処 |
|---|---|---|
| 日本語が文字化けする | ソースがUTF-8のままwclに渡っている | ビルド前に`iconv -f UTF-8 -t CP932`で変換([dos-hle.md](references/dos-hle.md)) |
| 特定の行が二重に表示される | `dprint()`等の`AH=09h`呼び出しで`$`終端を付け忘れ | 全呼び出しに`$`があるか`grep`で確認([dos-hle.md](references/dos-hle.md)) |
| 一時描画後に画面が勝手にスクロールしていく | カーソル位置を保存/復元せずに一時描画している | `ESC[s`/`ESC[u`で囲む([dos-hle.md](references/dos-hle.md)) |
| 日本語混じりの行の中央揃えがズレる | 全角/半角を同じ幅として計算している | `east_asian_width()`で表示幅を計算([dos-hle.md](references/dos-hle.md)) |
| 横スクロールで文字が乱れる | 全角文字を1バイト単位でスクロールしている | 半角カナ+半角英数のみで構成する([dos-hle.md](references/dos-hle.md)) |
| 音色を変えたら音程までズレた | キャリアオペレータのMULを変更した | MULは固定、AR/DR/SR/SL/RRとTLだけで音色を変える([opna-sound.md](references/opna-sound.md)) |
| BGMのテンポが実機で聴くと合わない | ソフトウェアウェイトの計算値を信じている | 実機で聴いて調整する。正確な時間が要る処理は`AH=2Ch`を使う([opna-sound.md](references/opna-sound.md)) |
| グラフィックを書き込んでも何も表示されない | GDCのSTARTコマンドを送っていない | `outp(0xA2, 0x6B)`を一度呼ぶ([graphics-vram.md](references/graphics-vram.md)) |
| グラフィックの位置が意図よりズレる/大きい | 縦2倍引き伸ばしを考慮していない | 縦座標だけ半分の値で計算する([graphics-vram.md](references/graphics-vram.md)) |
