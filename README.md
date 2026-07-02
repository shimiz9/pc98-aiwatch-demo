# pc98-aiwatch-demo

PC-98 FM音源デモ「AI Watch Intro」— 当時のパソコンショップ店頭デモ風の、
[AI Watch](https://ai.watch.impress.co.jp/)紹介デモです。

ブラウザで動くPC-98フリーソフトプレイヤー「[QuuBee](https://quubee.pages.dev)」
(NP2kaiベースのWasmエミュレータ、[GitHub](https://github.com/msonrm/quubee))上で
動作します。生成AIとの協働でPC-98(MS-DOS)アプリを作る過程そのものをコンテンツに
した企画で、試行錯誤の過程は[docs/devlog.md](docs/devlog.md)に記録しています。

## 遊び方

1. https://quubee.pages.dev を開く
2. [dist/AIWATCH.zip](dist/AIWATCH.zip)(または中の`AIWATCH.EXE`)をQuuBeeの画面に
   ドラッグ&ドロップ
3. ファイル一覧で`AIWATCH.EXE`を選び `▶ Run` で実行
4. タイトル画面・トピック紹介画面は何かキーを押すと次に進みます
   (自動的にBGMと帯状スクロールがバックグラウンドで流れ続けます)

ロゴ画像単体([gfx/AIWATCH.MAG](gfx/AIWATCH.MAG))も、同じくQuuBeeへドロップする
ことで単体表示できます。

## 構成

- **音源:** OPNA(YM2608)のFM音源部へレジスタを直接I/Oポート書き込みするオリジナル
  最小プレイヤー([src/opna.c](src/opna.c))。PMD/MXDRV等の市販/フリーDOSドライバは
  一切同梱・依存していません。
- **BGM:** メロディ/ベース/コードの3パート構成、C-G-Am-Fの4小節ループによる
  オリジナル曲([src/music.c](src/music.c))。演奏データも全てオリジナル。
- **画面:** DOS INT 21hのテキスト表示のみで構成(QuuBeeのHLE-DOSがカバーする範囲)。
  帯状スクロールは半角カナ+半角英数のみで構成し、1バイト=1桁スクロールでも
  文字化けしないようにしています([src/scroll.c](src/scroll.c))。
- **画像:** ロゴ用のMAG(MAKI02)形式ファイルを、QuuBee側のデコーダ実装を仕様
  リファレンスにした自作PNG→MAG変換スクリプト([tools/png2mag.py](tools/png2mag.py))
  で生成しています。

## ビルド方法

Open Watcom V2(16bit DOSクロスコンパイラ)が必要です。

```sh
./tools/build.sh AIWATCH.EXE src/aiwatch.c src/music.c src/opna.c src/notes.c src/scroll.c
```

`dist/AIWATCH.EXE`と`dist/AIWATCH.zip`が生成されます。ソースはUTF-8で編集していますが、
ビルド時に自動でCP932(Shift-JIS)へ変換されます。

音名テーブル(`src/notes.h`/`src/notes.c`)は`tools/gen_notes.py`で再生成できます。

## ターゲット環境の制約

- 16bitリアルモード限定(DPMI/DOSエクステンダ不使用)
- QuuBeeのHLE-DOS(INT 21h)がカバーする範囲のAPIのみ使用
- 実DOS非依存・実機ドライバ非依存

## 検証済み動作

- テキスト表示(タイトル/トピック/エンディング)
- OPNA単音発音(`OPNATEST.EXE`)
- 帯状スクロール(`SCROLLTS.EXE`)
- BGM 3パート再生(`MUSICTS.EXE`)
- ロゴMAG画像の単体表示
