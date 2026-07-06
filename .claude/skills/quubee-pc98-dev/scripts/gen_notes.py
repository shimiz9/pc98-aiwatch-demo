#!/usr/bin/env python3
"""
src/notes.h と src/notes.c を生成するスクリプト。
OPNA(YM2608)のFnum/BlockをFnum = freq * 144 * 2^(20-Block) / MasterClock の式で算出する。
MasterClock はPC-98オンボードOPNAの標準値 7987200Hz を使用。
"""
MASTER_CLOCK = 7987200.0
NOTE_NAMES = ['C', 'CS', 'D', 'DS', 'E', 'F', 'FS', 'G', 'GS', 'A', 'AS', 'B']
MIDI_LOW = 36   # C2
MIDI_HIGH = 84  # C6


def note_fnum_block(midi):
    freq = 440.0 * (2.0 ** ((midi - 69) / 12.0))
    octave = midi // 12 - 1
    block = max(1, min(7, octave))
    fnum = round(freq * 144.0 * (2 ** (20 - block)) / MASTER_CLOCK)
    while fnum > 2047 and block < 7:
        block += 1
        fnum = round(freq * 144.0 * (2 ** (20 - block)) / MASTER_CLOCK)
    while fnum < 1 and block > 1:
        block -= 1
        fnum = round(freq * 144.0 * (2 ** (20 - block)) / MASTER_CLOCK)
    return fnum, block


def main():
    defines, fnums, blocks = [], [], []
    idx = 0
    for midi in range(MIDI_LOW, MIDI_HIGH + 1):
        name = NOTE_NAMES[midi % 12]
        octave = midi // 12 - 1
        label = f"{name}{octave}"
        fnum, block = note_fnum_block(midi)
        defines.append(f"#define NOTE_{label} {idx}")
        fnums.append(str(fnum))
        blocks.append(str(block))
        idx += 1
    count = idx

    with open("src/notes.h", "w", encoding="utf-8") as f:
        f.write("/*\n")
        f.write(" * notes.h - 音名からOPNAのfnum/blockを引くテーブル(gen_notes.pyで自動生成)\n")
        f.write(" * Fnum = freq * 144 * 2^(20-Block) / MasterClock(7987200Hz) で算出。\n")
        f.write(" */\n")
        f.write("#ifndef NOTES_H\n#define NOTES_H\n\n")
        f.write(f"#define NOTE_COUNT {count}\n")
        f.write("#define NOTE_REST 0xFF\n\n")
        f.write("\n".join(defines))
        f.write("\n\n")
        f.write("extern const unsigned NOTE_FNUM[NOTE_COUNT];\n")
        f.write("extern const unsigned char NOTE_BLOCK[NOTE_COUNT];\n\n")
        f.write("#endif\n")

    with open("src/notes.c", "w", encoding="utf-8") as f:
        f.write('/* notes.c - gen_notes.py で自動生成。手編集しないこと */\n')
        f.write('#include "notes.h"\n\n')
        f.write(f"const unsigned NOTE_FNUM[NOTE_COUNT] = {{\n    " + ", ".join(fnums) + "\n};\n\n")
        f.write(f"const unsigned char NOTE_BLOCK[NOTE_COUNT] = {{\n    " + ", ".join(blocks) + "\n};\n")

    print(f"generated {count} notes -> src/notes.h, src/notes.c")


if __name__ == "__main__":
    main()
