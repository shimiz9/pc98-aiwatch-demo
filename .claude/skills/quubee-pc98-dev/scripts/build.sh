#!/usr/bin/env bash
# QuuBee(PC-98)向けDOSアプリのビルドスクリプト。
# 使い方: ./build.sh OUT.EXE src/foo.c [src/bar.c ...]
#
# 前提: OpenWatcom V2がインストール済みで、owsetenv.shへのパスが
# 環境変数 OWSETENV (既定: $HOME/openwatcom/owsetenv.sh) で指定されていること。
set -e
OWSETENV="${OWSETENV:-$HOME/openwatcom/owsetenv.sh}"
[ -f "$OWSETENV" ] || { echo "エラー: OpenWatcomのowsetenv.shが見つかりません($OWSETENV)。OWSETENV環境変数で指定してください" >&2; exit 1; }
source "$OWSETENV"

OUT="${1:?使い方: ./build.sh OUT.EXE src/foo.c [src/bar.c ...]}"
shift
[ "$#" -ge 1 ] || { echo "エラー: ソースファイルを1つ以上指定してください" >&2; exit 1; }
mkdir -p dist build

# ソースはUTF-8で編集するが、PC-98 DOS(QuuBee)はShift-JIS(CP932)前提のため、
# src/配下の.c/.hを丸ごとCP932変換してbuild/へミラーしてからそちらをビルドする
# (相対 #include "foo.h" が正しく解決されるよう、.hも同じディレクトリに置く)。
# em dash(—)や≈等のUnicode専用記号はCP932に存在せず変換エラーになるので、
# 日本語コメントであってもASCII代替(- , ->, 約, 等)を使うこと。
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
