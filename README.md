# pc98-aiwatch-demo

PC-98 FM音源デモ「AI Watch Intro」— 当時のパソコンショップ店頭デモ風の、
[AI Watch](https://ai.watch.impress.co.jp/)紹介デモです。

ブラウザで動くPC-98フリーソフトプレイヤー「[QuuBee](https://quubee.pages.dev)」
(NP2kaiベースのWasmエミュレータ、[GitHub](https://github.com/msonrm/quubee))上で
動作します。生成AIとの協働でPC-98(MS-DOS)アプリを作る過程そのものをコンテンツに
した企画で、試行錯誤の過程は[docs/devlog.md](docs/devlog.md)に記録しています。

## 遊び方

ビルド済みの`AIWATCH.zip`は[Releases](https://github.com/shimiz9/pc98-aiwatch-demo/releases)
からダウンロードできます(自分でビルドする必要はありません)。

1. https://quubee.pages.dev を開く
2. [Releases](https://github.com/shimiz9/pc98-aiwatch-demo/releases)からダウンロードした
   `AIWATCH.zip`(リポジトリ内なら[dist/AIWATCH.zip](dist/AIWATCH.zip)、または中の
   `AIWATCH.EXE`)をQuuBeeの画面にドラッグ&ドロップ
3. ファイル一覧で`AIWATCH.EXE`を選び `▶ Run` で実行
4. タイトル画面・トピック紹介画面は何かキーを押すと次に進みます
   (5秒間入力が無ければ自動的に次の画面へ進みます)
   (自動的にBGMと帯状スクロールがバックグラウンドで流れ続けます)
   (タイトル画面のロゴは1秒おきに色が変わり、トピック画面ではロボットが
   画面内を左上→右上→左下→右下の「Z」の軌跡で横切ります)

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
- **画像:** タイトル画面ではグラフィックVRAM(0xA800/0xB000/0xB800の3プレーン)へ
  直接描画したロゴを表示しています([src/gvram.c](src/gvram.c), [src/logo.c](src/logo.c))。
  ロゴのバッジ色は1秒おきに変化します。トピック紹介画面では、同じグラフィック
  VRAMを使って簡単なロボットが「Z」の軌跡で画面を横切ります([src/robot.c](src/robot.c))。
  別途、単体表示用のMAG(MAKI02)形式ファイルもQuuBee側のデコーダ実装を仕様
  リファレンスにした自作PNG→MAG変換スクリプト([tools/png2mag.py](tools/png2mag.py))
  で生成しています([gfx/AIWATCH.MAG](gfx/AIWATCH.MAG))。
- **自動遷移:** 「Press any key」画面はDOSの実時刻(INT21h AH=2Ch)を使って
  5秒間入力が無ければ自動的に次の画面へ進みます。

## ビルド方法

Open Watcom V2(16bit DOSクロスコンパイラ)が必要です。

```sh
./tools/build.sh AIWATCH.EXE src/aiwatch.c src/music.c src/opna.c src/notes.c src/scroll.c src/gvram.c src/logo.c src/robot.c
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
- グラフィックVRAMへのロゴ描画(`GVRAMTS.EXE`, `AIWATCH.EXE`本体)
- ロゴMAG画像の単体表示
- ロゴの色サイクル・5秒自動遷移・ロボットのZ軌跡アニメーション

## 補足: グラフィック画面の注意点

QuuBeeのHLE-DOSはテキスト画面の表示は自動で有効化しますが、グラフィック画面は
プログラム側でGDCへSTARTコマンドを送らないと表示されません(`gvram_enable()`)。
また、グラフィック画面は縦200ラインを2倍に引き伸ばして表示される挙動があるため、
座標を指定する際は縦方向だけ半分の値で計算する必要があります。詳細は
[docs/devlog.md](docs/devlog.md)のフェーズ10を参照してください。
