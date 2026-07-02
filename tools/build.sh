#!/usr/bin/env bash
# AI Watch Intro (PC-98/QuuBee向け) ビルドスクリプト
# 使い方: ./tools/build.sh OUT.EXE src/foo.c [src/bar.c ...]
set -e
source /home/shimiz/workspace/openwatcom/owsetenv.sh
OUT="${1:?使い方: ./tools/build.sh OUT.EXE src/foo.c [src/bar.c ...]}"
shift
[ "$#" -ge 1 ] || { echo "エラー: ソースファイルを1つ以上指定してください" >&2; exit 1; }
mkdir -p dist build

# ソースはUTF-8で編集するが、PC-98 DOS(QuuBee)はShift-JIS(CP932)前提のため、
# src/配下の.c/.hを丸ごとCP932変換してbuild/へミラーしてからそちらをビルドする
# (相対 #include "foo.h" が正しく解決されるよう、.hも同じディレクトリに置く)。
for f in src/*.c src/*.h; do
    [ -e "$f" ] || continue
    iconv -f UTF-8 -t CP932 "$f" > "build/$(basename "$f")"
done

BUILD_SRCS=()
for SRC in "$@"; do
    BUILD_SRCS+=("build/$(basename "$SRC")")
done

wcl -bt=dos -ms -fe="dist/$OUT" "${BUILD_SRCS[@]}"
( cd dist && zip -j "${OUT%.EXE}.zip" "$OUT" >/dev/null )
echo "=> dist/$OUT および dist/${OUT%.EXE}.zip を生成しました"
