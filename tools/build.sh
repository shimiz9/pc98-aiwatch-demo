#!/usr/bin/env bash
# AI Watch Intro (PC-98/QuuBee向け) ビルドスクリプト
# 使い方: ./tools/build.sh <ソース.c> [出力名.EXE]
set -e
source /home/shimiz/workspace/openwatcom/owsetenv.sh
SRC="${1:?使い方: ./tools/build.sh src/aiwatch.c AIWATCH.EXE}"
OUT="${2:-$(basename "${SRC%.*}" | tr 'a-z' 'A-Z').EXE}"
mkdir -p dist build

# ソースはUTF-8で編集するが、PC-98 DOS(QuuBee)はShift-JIS(CP932)前提のため
# コンパイル直前にSJISへ変換したコピーを作ってそちらをビルドする。
SJIS_SRC="build/$(basename "$SRC")"
iconv -f UTF-8 -t CP932 "$SRC" > "$SJIS_SRC"

wcl -bt=dos -ms -fe="dist/$OUT" "$SJIS_SRC"
( cd dist && zip -j "${OUT%.EXE}.zip" "$OUT" >/dev/null )
echo "=> dist/$OUT および dist/${OUT%.EXE}.zip を生成しました"
