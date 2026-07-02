# 開発ログ — AI Watch Intro (PC-98 FM音源デモ)

生成AI(Claude)との対話を通じてQuuBee向けPC-98デモを作る過程の記録。
プロンプト、判断、うまくいかなかった点を時系列で残す。

## 2026-07-02 フェーズ1: 開発環境の統合

以前のセッション(workspace直下)で確立済みだったOpenWatcom V2環境をそのまま流用。
- `openwatcom/` (16bit DOSクロスコンパイラ一式、`open-watcom-v2`のsnapshotビルド)はworkspaceルート(`/home/shimiz/workspace/openwatcom`)に置いたまま共有し、プロジェクト側の`tools/build.sh`から絶対パスで`owsetenv.sh`をsourceする方式にした。
  - 判断理由: OpenWatcom一式は約270ファイル・相当なサイズがあり、プロジェクトごとにコピーすると各リポジトリが肥大化する。複数プロジェクトで共有する前提なら絶対パス参照で十分。
- `tools/build.sh src/xxx.c OUT.EXE` で `wcl -bt=dos -ms` によるコンパイル+リンク、distへのzip化まで一括実行。
- 動作確認: `src/hello_check.c`(INT 21h AH=09hのみ)をビルドし、`dist/CHECK.EXE`が `4D 5A`(MZ)ヘッダを持つ16bit DOSリアルモードEXEであることを`file`/`xxd`で確認。問題なし。確認後ファイルは削除(スケルトンのみ本採用)。

## 2026-07-02 フェーズ2: QuuBeeのHLE-DOS実装調査

`github.com/msonrm/quubee` (native/配下)と、そのサブモジュール `core/np2kai` (AZO234/NP2kai)をシャローcloneして調査。

### 使用可能なINT 21hサブセット (`native/dos_int21.c`)
文字表示・キー入力・ファイルI/O・メモリ確保・EXEC・終了系まで、実用上必要な範囲は一通り実装されている:
- テキスト表示: AH=00h〜0Eh (直接コンソール出力、カーソル制御、ESC/CSI相当のANSI風エスケープも解釈)
- 文字/文字列I/O: AH=01h,06h,07h,08h,09h,0Ah,0Bh,0Ch (getch/putstr等)
- ファイル操作: AH=3Ch(create)/3Dh(open)/3Eh(close)/3Fh(read)/40h(write)/41h(delete)/42h(seek)/43h(attr)/45h,46h(dup)/4Eh,4Fh(findfirst/next)
- メモリ: AH=48h(alloc)/49h(free)/4Ah(resize)/58h(alloc strategy)
- プロセス: AH=4Bh(exec)/4Ch(exit)/4Dh(retcode)/31h(TSR keep)
- その他: AH=19h(カレントドライブ)/1Ah(DTA設定)/25h,35h(割り込みベクタ)/2Ah,2Ch(日付時刻)/30h(DOSバージョン)/36h(空き容量)/39h〜3Bh(ディレクトリ操作)
→ **本デモが必要とする範囲(ファイル読み込み・テキスト表示・終了)はすべてカバーされている。**

### EXEロード対応 (`native/dos_loader.c`)
`magic == 0x5A4D (MZ) || 0x4D5A (ZM)` でEXE形式と判定するローダーが存在(1762行目, 1974行目付近)。`.COM`形式(拡張子で判定)にも対応。
→ **`.EXE`(MZ形式)での配布方針は問題なし。**

### OPNA (FM音源) のI/Oポート
QuuBeeの`native/qb_soundmng.c`はJS側WebAudioへのPCM受け渡しグルー(pull型)のみで、OPNAレジスタのエミュレーション自体はサブモジュール`core/np2kai`(NP2kai本体、fmgenベース)が担当。HLE対象ではなく、**実機同様のハードウェアI/Oエミュレーション**である点を確認できたのは重要な収穫:
- `core/np2kai/cbus/board86.c`: オンボード音源として`0x188`固定ベースでOPNAを登録(`board86_bind`内、`g_opna[0].s.base = 0x000; // 0x188固定`)
- ポート内訳(`opna_o188`/`opna_o18a`/`opna_o18c`/`opna_o18e`などの関数名から確認):
  - `0x188` = レジスタセットA アドレス書き込み / ステータス読み出し
  - `0x18A` = レジスタセットA データ書き込み / データ読み出し
  - `0x18C` = レジスタセットB(拡張チャンネル)アドレス書き込み
  - `0x18E` = レジスタセットB データ書き込み
→ **`outp(0x188, addr); outp(0x18A, data);`のような直接ポートI/Oでよく、市販ドライバは一切不要。この点は当初の想定通りで、実装方針が裏付けられた。**

### テキストVRAM/グラフィックVRAM
`native/qb_scrnmng.c`はJS Canvas側へのサーフェス受け渡しのみで、PC-98のテキストVRAM(0xA000セグメント)・グラフィックVRAM(0xA800/0xE000)自体のメモリマッピングはHLE対象外、`core/np2kai`のメモリコアが実機同様にエミュレートしている前提。
→ 次フェーズでfar pointer(`MK_FP`)による0xA000:0000への直接書き込みが実機同様に機能するか、実際にビルドしたEXEで確認する必要あり(未検証)。

### 試行錯誤・つまずいた点
- `core/np2kai`はgit submoduleのため、最初の`git clone --depth 1`だけでは空ディレクトリになった。`.gitmodules`を確認し`git submodule update --init --depth 1 core/np2kai`で追加取得が必要だった。
- OPNAのI/Oポート実装自体はQuuBee本体(native/)ではなくNP2kai本体(core/np2kai/cbus/board86.c)にあり、最初`native/`配下だけを探して見つからなかった。QuuBeeが「DOSシステムコールだけHLE、ハードウェアエミュレーションはNP2kai本体そのまま」というアーキテクチャだと理解してから探索先を変えたら見つかった。

## 次のステップ
- フェーズ3: テキスト表示のみの最小EXEをAIWATCH.EXEの雛形として作成、QuuBeeでの動作確認(ユーザーによるブラウザ実行が必要)
- フェーズ4: OPNA直接I/O書き込みでの発音最小サンプル
