#!/usr/bin/env python3
"""
png2mag.py - PNG画像をPC-98 MAG形式(MAKI02, 16色)へ変換する自作コンバータ。

第三者の非公開MAG変換ツールには依存しない。MAGのバイナリ仕様は
QuuBee(https://github.com/msonrm/quubee)側のデコーダ実装
web/player/magimage.js を仕様リファレンスとして自前で実装した
(コードの移植ではなく、フォーマットの事実だけを参照)。

圧縮方式について: MAGは行/列をまたいだ差分コピー(flagA/flagB)で圧縮できるが、
本スクリプトは実装を単純にするため「flagAを全て0にする」= 全ピクセルを
毎回ピクセルストリームから読む(無圧縮)方式で出力する。ファイルサイズは
最適ではないが、QuuBeeのデコード仕様上100%正しくデコードされる。

依存: Pillow (PNGの読み込みと16色量子化のみに使用。MAGエンコード自体は自作)
使い方: python3 tools/png2mag.py input.png output.mag ["コメント"]
"""
import struct
import sys

from PIL import Image


def encode_mag(img: Image.Image, comment: str = "") -> bytes:
    img = img.convert("RGB")
    w, h = img.size

    # 幅は8の倍数にパディング(1ユニット=8px、shift=3のため)
    pad_w = (w + 7) // 8 * 8
    if pad_w != w:
        canvas = Image.new("RGB", (pad_w, h), (0, 0, 0))
        canvas.paste(img, (0, 0))
        img = canvas
        w = pad_w

    # 16色に量子化してパレット画像へ
    quant = img.quantize(colors=16, method=Image.MEDIANCUT)
    palette_rgb = quant.getpalette()[: 16 * 3]  # [R,G,B, R,G,B, ...] x16
    pixels = list(quant.getdata())  # 各ピクセルのパレットindex(0-15)

    # MAGのパレットは G,R,B の並び
    pal_bytes = bytearray()
    for i in range(16):
        r, g, b = palette_rgb[i * 3: i * 3 + 3]
        pal_bytes += bytes([g, r, b])

    # 4bpp packing: 1バイト = (左ピクセル<<4)|右ピクセル、行ごとに w/2 バイト
    byte_width = w // 2
    pixel_stream = bytearray(byte_width * h)
    for y in range(h):
        row_off = y * w
        out_off = y * byte_width
        for x in range(0, w, 2):
            left = pixels[row_off + x] & 0x0F
            right = pixels[row_off + x + 1] & 0x0F
            pixel_stream[out_off + x // 2] = (left << 4) | right

    units = w // 8
    lines = h
    flaga_bits = units * lines
    flaga_bytes = bytearray((flaga_bits + 7) // 8)  # 全ビット0 = 常に非圧縮(リテラル)

    HEADER_LEN = 32
    off_a = HEADER_LEN + len(pal_bytes)         # パレット直後
    off_b = off_a + len(flaga_bytes)            # flagAは使うがflagBは空(未使用)
    off_p = off_b                                # flagBサイズ0なのでoffP==offB
    reserved_off = 0

    x0, y0, x1, y1 = 0, 0, w - 1, h - 1
    screen_mode = 0x00  # bit7=0(16色) / bit0=0(400ライン, 縦2倍しない)

    header = struct.pack(
        "<3sBHHHHIIII4x",
        b"\x00\x00\x00",   # 予約(機種コード等、デコーダは未参照)
        screen_mode,
        x0, y0, x1, y1,
        off_a, off_b, reserved_off, off_p,
    )
    assert len(header) == HEADER_LEN, len(header)

    sig = b"MAKI02  "
    comment_bytes = comment.encode("cp932", errors="replace") + b"\x1a"

    return (
        sig
        + comment_bytes
        + header
        + bytes(pal_bytes)
        + bytes(flaga_bytes)
        + bytes(pixel_stream)   # offB==offP なのでflagB(空)の直後がpixelストリーム
    )


def main():
    if len(sys.argv) < 3:
        print("使い方: python3 tools/png2mag.py input.png output.mag [\"コメント\"]", file=sys.stderr)
        sys.exit(1)
    in_path, out_path = sys.argv[1], sys.argv[2]
    comment = sys.argv[3] if len(sys.argv) > 3 else "pc98-aiwatch-demo"

    img = Image.open(in_path)
    data = encode_mag(img, comment)
    with open(out_path, "wb") as f:
        f.write(data)
    print(f"=> {out_path} ({len(data)} bytes, {img.size[0]}x{img.size[1]} -> 16色)")


if __name__ == "__main__":
    main()
