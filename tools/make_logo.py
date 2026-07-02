#!/usr/bin/env python3
"""
gfx/logo_src.png - AI Watchロゴのソース画像を生成する。
16色のフラットな配色でデザインし、tools/png2mag.py での量子化ロスを避ける。
"""
from PIL import Image, ImageDraw, ImageFont

W, H = 320, 64

BG = (0, 0, 16)          # ほぼ黒(ネイビー寄り)
ACCENT = (0, 200, 220)   # シアン系アクセント
TEXT = (255, 255, 255)   # 白
SUB = (255, 210, 0)      # 黄色系サブテキスト

img = Image.new("RGB", (W, H), BG)
d = ImageDraw.Draw(img)

# 上下にアクセントの帯
d.rectangle([0, 0, W - 1, 3], fill=ACCENT)
d.rectangle([0, H - 4, W - 1, H - 1], fill=ACCENT)

font_main = ImageFont.load_default(size=28)
font_sub = ImageFont.load_default(size=11)

title = "AI WATCH"
bbox = d.textbbox((0, 0), title, font=font_main)
tw, th = bbox[2] - bbox[0], bbox[3] - bbox[1]
d.text(((W - tw) // 2 - bbox[0], 8 - bbox[1]), title, fill=TEXT, font=font_main)

sub = "ai.watch.impress.co.jp"
bbox2 = d.textbbox((0, 0), sub, font=font_sub)
sw = bbox2[2] - bbox2[0]
d.text(((W - sw) // 2 - bbox2[0], H - 18), sub, fill=SUB, font=font_sub)

img.save("gfx/logo_src.png")
print("=> gfx/logo_src.png", img.size)
