#!/usr/bin/env bash
# AI Watch Intro (PC-98/QuuBee向け) ビルドスクリプト
# 使い方: ./tools/build.sh <ソース.c> [出力名.EXE]
set -e
source /home/shimiz/workspace/openwatcom/owsetenv.sh
SRC="${1:?使い方: ./tools/build.sh src/aiwatch.c AIWATCH.EXE}"
OUT="${2:-$(basename "${SRC%.*}" | tr 'a-z' 'A-Z').EXE}"
mkdir -p dist
wcl -bt=dos -ms -fe="dist/$OUT" "$SRC"
( cd dist && zip -j "${OUT%.EXE}.zip" "$OUT" >/dev/null )
echo "=> dist/$OUT および dist/${OUT%.EXE}.zip を生成しました"
