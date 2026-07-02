/*
 * notes.h - 音名からOPNAのfnum/blockを引くテーブル(tools/gen_notes.pyで自動生成)
 * Fnum = freq * 144 * 2^(20-Block) / MasterClock(7987200Hz) で算出。
 */
#ifndef NOTES_H
#define NOTES_H

#define NOTE_COUNT 49
#define NOTE_REST 0xFF

#define NOTE_C2 0
#define NOTE_CS2 1
#define NOTE_D2 2
#define NOTE_DS2 3
#define NOTE_E2 4
#define NOTE_F2 5
#define NOTE_FS2 6
#define NOTE_G2 7
#define NOTE_GS2 8
#define NOTE_A2 9
#define NOTE_AS2 10
#define NOTE_B2 11
#define NOTE_C3 12
#define NOTE_CS3 13
#define NOTE_D3 14
#define NOTE_DS3 15
#define NOTE_E3 16
#define NOTE_F3 17
#define NOTE_FS3 18
#define NOTE_G3 19
#define NOTE_GS3 20
#define NOTE_A3 21
#define NOTE_AS3 22
#define NOTE_B3 23
#define NOTE_C4 24
#define NOTE_CS4 25
#define NOTE_D4 26
#define NOTE_DS4 27
#define NOTE_E4 28
#define NOTE_F4 29
#define NOTE_FS4 30
#define NOTE_G4 31
#define NOTE_GS4 32
#define NOTE_A4 33
#define NOTE_AS4 34
#define NOTE_B4 35
#define NOTE_C5 36
#define NOTE_CS5 37
#define NOTE_D5 38
#define NOTE_DS5 39
#define NOTE_E5 40
#define NOTE_F5 41
#define NOTE_FS5 42
#define NOTE_G5 43
#define NOTE_GS5 44
#define NOTE_A5 45
#define NOTE_AS5 46
#define NOTE_B5 47
#define NOTE_C6 48

extern const unsigned NOTE_FNUM[NOTE_COUNT];
extern const unsigned char NOTE_BLOCK[NOTE_COUNT];

#endif
