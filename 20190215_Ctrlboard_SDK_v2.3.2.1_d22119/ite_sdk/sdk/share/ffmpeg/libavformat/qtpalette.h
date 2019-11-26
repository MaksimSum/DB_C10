﻿/*
 * Default Palettes for Quicktime Files
 *  Automatically generated from a utility derived from XAnim:
 *  http://xanim.va.pubnix.com/home.html
 *
 * This file is part of FFmpeg.
 *
 * FFmpeg is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * FFmpeg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with FFmpeg; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef AVFORMAT_QTPALETTE_H
#define AVFORMAT_QTPALETTE_H

#include <inttypes.h>

static const uint8_t ff_qt_default_palette_4[4 * 3] = {
  0x93, 0x65, 0x5E,
  0xFF, 0xFF, 0xFF,
  0xDF, 0xD0, 0xAB,
  0x00, 0x00, 0x00
};

static const uint8_t ff_qt_default_palette_16[16 * 3] = {
  0xFF, 0xFB, 0xFF,
  0xEF, 0xD9, 0xBB,
  0xE8, 0xC9, 0xB1,
  0x93, 0x65, 0x5E,
  0xFC, 0xDE, 0xE8,
  0x9D, 0x88, 0x91,
  0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF,
  0x47, 0x48, 0x37,
  0x7A, 0x5E, 0x55,
  0xDF, 0xD0, 0xAB,
  0xFF, 0xFB, 0xF9,
  0xE8, 0xCA, 0xC5,
  0x8A, 0x7C, 0x77,
  0x00, 0x00, 0x00
};

static const uint8_t ff_qt_default_palette_256[256 * 3] = {
  /*   0, 0x00 */  0xFF, 0xFF, 0xFF,
  /*   1, 0x01 */  0xFF, 0xFF, 0xCC,
  /*   2, 0x02 */  0xFF, 0xFF, 0x99,
  /*   3, 0x03 */  0xFF, 0xFF, 0x66,
  /*   4, 0x04 */  0xFF, 0xFF, 0x33,
  /*   5, 0x05 */  0xFF, 0xFF, 0x00,
  /*   6, 0x06 */  0xFF, 0xCC, 0xFF,
  /*   7, 0x07 */  0xFF, 0xCC, 0xCC,
  /*   8, 0x08 */  0xFF, 0xCC, 0x99,
  /*   9, 0x09 */  0xFF, 0xCC, 0x66,
  /*  10, 0x0A */  0xFF, 0xCC, 0x33,
  /*  11, 0x0B */  0xFF, 0xCC, 0x00,
  /*  12, 0x0C */  0xFF, 0x99, 0xFF,
  /*  13, 0x0D */  0xFF, 0x99, 0xCC,
  /*  14, 0x0E */  0xFF, 0x99, 0x99,
  /*  15, 0x0F */  0xFF, 0x99, 0x66,
  /*  16, 0x10 */  0xFF, 0x99, 0x33,
  /*  17, 0x11 */  0xFF, 0x99, 0x00,
  /*  18, 0x12 */  0xFF, 0x66, 0xFF,
  /*  19, 0x13 */  0xFF, 0x66, 0xCC,
  /*  20, 0x14 */  0xFF, 0x66, 0x99,
  /*  21, 0x15 */  0xFF, 0x66, 0x66,
  /*  22, 0x16 */  0xFF, 0x66, 0x33,
  /*  23, 0x17 */  0xFF, 0x66, 0x00,
  /*  24, 0x18 */  0xFF, 0x33, 0xFF,
  /*  25, 0x19 */  0xFF, 0x33, 0xCC,
  /*  26, 0x1A */  0xFF, 0x33, 0x99,
  /*  27, 0x1B */  0xFF, 0x33, 0x66,
  /*  28, 0x1C */  0xFF, 0x33, 0x33,
  /*  29, 0x1D */  0xFF, 0x33, 0x00,
  /*  30, 0x1E */  0xFF, 0x00, 0xFF,
  /*  31, 0x1F */  0xFF, 0x00, 0xCC,
  /*  32, 0x20 */  0xFF, 0x00, 0x99,
  /*  33, 0x21 */  0xFF, 0x00, 0x66,
  /*  34, 0x22 */  0xFF, 0x00, 0x33,
  /*  35, 0x23 */  0xFF, 0x00, 0x00,
  /*  36, 0x24 */  0xCC, 0xFF, 0xFF,
  /*  37, 0x25 */  0xCC, 0xFF, 0xCC,
  /*  38, 0x26 */  0xCC, 0xFF, 0x99,
  /*  39, 0x27 */  0xCC, 0xFF, 0x66,
  /*  40, 0x28 */  0xCC, 0xFF, 0x33,
  /*  41, 0x29 */  0xCC, 0xFF, 0x00,
  /*  42, 0x2A */  0xCC, 0xCC, 0xFF,
  /*  43, 0x2B */  0xCC, 0xCC, 0xCC,
  /*  44, 0x2C */  0xCC, 0xCC, 0x99,
  /*  45, 0x2D */  0xCC, 0xCC, 0x66,
  /*  46, 0x2E */  0xCC, 0xCC, 0x33,
  /*  47, 0x2F */  0xCC, 0xCC, 0x00,
  /*  48, 0x30 */  0xCC, 0x99, 0xFF,
  /*  49, 0x31 */  0xCC, 0x99, 0xCC,
  /*  50, 0x32 */  0xCC, 0x99, 0x99,
  /*  51, 0x33 */  0xCC, 0x99, 0x66,
  /*  52, 0x34 */  0xCC, 0x99, 0x33,
  /*  53, 0x35 */  0xCC, 0x99, 0x00,
  /*  54, 0x36 */  0xCC, 0x66, 0xFF,
  /*  55, 0x37 */  0xCC, 0x66, 0xCC,
  /*  56, 0x38 */  0xCC, 0x66, 0x99,
  /*  57, 0x39 */  0xCC, 0x66, 0x66,
  /*  58, 0x3A */  0xCC, 0x66, 0x33,
  /*  59, 0x3B */  0xCC, 0x66, 0x00,
  /*  60, 0x3C */  0xCC, 0x33, 0xFF,
  /*  61, 0x3D */  0xCC, 0x33, 0xCC,
  /*  62, 0x3E */  0xCC, 0x33, 0x99,
  /*  63, 0x3F */  0xCC, 0x33, 0x66,
  /*  64, 0x40 */  0xCC, 0x33, 0x33,
  /*  65, 0x41 */  0xCC, 0x33, 0x00,
  /*  66, 0x42 */  0xCC, 0x00, 0xFF,
  /*  67, 0x43 */  0xCC, 0x00, 0xCC,
  /*  68, 0x44 */  0xCC, 0x00, 0x99,
  /*  69, 0x45 */  0xCC, 0x00, 0x66,
  /*  70, 0x46 */  0xCC, 0x00, 0x33,
  /*  71, 0x47 */  0xCC, 0x00, 0x00,
  /*  72, 0x48 */  0x99, 0xFF, 0xFF,
  /*  73, 0x49 */  0x99, 0xFF, 0xCC,
  /*  74, 0x4A */  0x99, 0xFF, 0x99,
  /*  75, 0x4B */  0x99, 0xFF, 0x66,
  /*  76, 0x4C */  0x99, 0xFF, 0x33,
  /*  77, 0x4D */  0x99, 0xFF, 0x00,
  /*  78, 0x4E */  0x99, 0xCC, 0xFF,
  /*  79, 0x4F */  0x99, 0xCC, 0xCC,
  /*  80, 0x50 */  0x99, 0xCC, 0x99,
  /*  81, 0x51 */  0x99, 0xCC, 0x66,
  /*  82, 0x52 */  0x99, 0xCC, 0x33,
  /*  83, 0x53 */  0x99, 0xCC, 0x00,
  /*  84, 0x54 */  0x99, 0x99, 0xFF,
  /*  85, 0x55 */  0x99, 0x99, 0xCC,
  /*  86, 0x56 */  0x99, 0x99, 0x99,
  /*  87, 0x57 */  0x99, 0x99, 0x66,
  /*  88, 0x58 */  0x99, 0x99, 0x33,
  /*  89, 0x59 */  0x99, 0x99, 0x00,
  /*  90, 0x5A */  0x99, 0x66, 0xFF,
  /*  91, 0x5B */  0x99, 0x66, 0xCC,
  /*  92, 0x5C */  0x99, 0x66, 0x99,
  /*  93, 0x5D */  0x99, 0x66, 0x66,
  /*  94, 0x5E */  0x99, 0x66, 0x33,
  /*  95, 0x5F */  0x99, 0x66, 0x00,
  /*  96, 0x60 */  0x99, 0x33, 0xFF,
  /*  97, 0x61 */  0x99, 0x33, 0xCC,
  /*  98, 0x62 */  0x99, 0x33, 0x99,
  /*  99, 0x63 */  0x99, 0x33, 0x66,
  /* 100, 0x64 */  0x99, 0x33, 0x33,
  /* 101, 0x65 */  0x99, 0x33, 0x00,
  /* 102, 0x66 */  0x99, 0x00, 0xFF,
  /* 103, 0x67 */  0x99, 0x00, 0xCC,
  /* 104, 0x68 */  0x99, 0x00, 0x99,
  /* 105, 0x69 */  0x99, 0x00, 0x66,
  /* 106, 0x6A */  0x99, 0x00, 0x33,
  /* 107, 0x6B */  0x99, 0x00, 0x00,
  /* 108, 0x6C */  0x66, 0xFF, 0xFF,
  /* 109, 0x6D */  0x66, 0xFF, 0xCC,
  /* 110, 0x6E */  0x66, 0xFF, 0x99,
  /* 111, 0x6F */  0x66, 0xFF, 0x66,
  /* 112, 0x70 */  0x66, 0xFF, 0x33,
  /* 113, 0x71 */  0x66, 0xFF, 0x00,
  /* 114, 0x72 */  0x66, 0xCC, 0xFF,
  /* 115, 0x73 */  0x66, 0xCC, 0xCC,
  /* 116, 0x74 */  0x66, 0xCC, 0x99,
  /* 117, 0x75 */  0x66, 0xCC, 0x66,
  /* 118, 0x76 */  0x66, 0xCC, 0x33,
  /* 119, 0x77 */  0x66, 0xCC, 0x00,
  /* 120, 0x78 */  0x66, 0x99, 0xFF,
  /* 121, 0x79 */  0x66, 0x99, 0xCC,
  /* 122, 0x7A */  0x66, 0x99, 0x99,
  /* 123, 0x7B */  0x66, 0x99, 0x66,
  /* 124, 0x7C */  0x66, 0x99, 0x33,
  /* 125, 0x7D */  0x66, 0x99, 0x00,
  /* 126, 0x7E */  0x66, 0x66, 0xFF,
  /* 127, 0x7F */  0x66, 0x66, 0xCC,
  /* 128, 0x80 */  0x66, 0x66, 0x99,
  /* 129, 0x81 */  0x66, 0x66, 0x66,
  /* 130, 0x82 */  0x66, 0x66, 0x33,
  /* 131, 0x83 */  0x66, 0x66, 0x00,
  /* 132, 0x84 */  0x66, 0x33, 0xFF,
  /* 133, 0x85 */  0x66, 0x33, 0xCC,
  /* 134, 0x86 */  0x66, 0x33, 0x99,
  /* 135, 0x87 */  0x66, 0x33, 0x66,
  /* 136, 0x88 */  0x66, 0x33, 0x33,
  /* 137, 0x89 */  0x66, 0x33, 0x00,
  /* 138, 0x8A */  0x66, 0x00, 0xFF,
  /* 139, 0x8B */  0x66, 0x00, 0xCC,
  /* 140, 0x8C */  0x66, 0x00, 0x99,
  /* 141, 0x8D */  0x66, 0x00, 0x66,
  /* 142, 0x8E */  0x66, 0x00, 0x33,
  /* 143, 0x8F */  0x66, 0x00, 0x00,
  /* 144, 0x90 */  0x33, 0xFF, 0xFF,
  /* 145, 0x91 */  0x33, 0xFF, 0xCC,
  /* 146, 0x92 */  0x33, 0xFF, 0x99,
  /* 147, 0x93 */  0x33, 0xFF, 0x66,
  /* 148, 0x94 */  0x33, 0xFF, 0x33,
  /* 149, 0x95 */  0x33, 0xFF, 0x00,
  /* 150, 0x96 */  0x33, 0xCC, 0xFF,
  /* 151, 0x97 */  0x33, 0xCC, 0xCC,
  /* 152, 0x98 */  0x33, 0xCC, 0x99,
  /* 153, 0x99 */  0x33, 0xCC, 0x66,
  /* 154, 0x9A */  0x33, 0xCC, 0x33,
  /* 155, 0x9B */  0x33, 0xCC, 0x00,
  /* 156, 0x9C */  0x33, 0x99, 0xFF,
  /* 157, 0x9D */  0x33, 0x99, 0xCC,
  /* 158, 0x9E */  0x33, 0x99, 0x99,
  /* 159, 0x9F */  0x33, 0x99, 0x66,
  /* 160, 0xA0 */  0x33, 0x99, 0x33,
  /* 161, 0xA1 */  0x33, 0x99, 0x00,
  /* 162, 0xA2 */  0x33, 0x66, 0xFF,
  /* 163, 0xA3 */  0x33, 0x66, 0xCC,
  /* 164, 0xA4 */  0x33, 0x66, 0x99,
  /* 165, 0xA5 */  0x33, 0x66, 0x66,
  /* 166, 0xA6 */  0x33, 0x66, 0x33,
  /* 167, 0xA7 */  0x33, 0x66, 0x00,
  /* 168, 0xA8 */  0x33, 0x33, 0xFF,
  /* 169, 0xA9 */  0x33, 0x33, 0xCC,
  /* 170, 0xAA */  0x33, 0x33, 0x99,
  /* 171, 0xAB */  0x33, 0x33, 0x66,
  /* 172, 0xAC */  0x33, 0x33, 0x33,
  /* 173, 0xAD */  0x33, 0x33, 0x00,
  /* 174, 0xAE */  0x33, 0x00, 0xFF,
  /* 175, 0xAF */  0x33, 0x00, 0xCC,
  /* 176, 0xB0 */  0x33, 0x00, 0x99,
  /* 177, 0xB1 */  0x33, 0x00, 0x66,
  /* 178, 0xB2 */  0x33, 0x00, 0x33,
  /* 179, 0xB3 */  0x33, 0x00, 0x00,
  /* 180, 0xB4 */  0x00, 0xFF, 0xFF,
  /* 181, 0xB5 */  0x00, 0xFF, 0xCC,
  /* 182, 0xB6 */  0x00, 0xFF, 0x99,
  /* 183, 0xB7 */  0x00, 0xFF, 0x66,
  /* 184, 0xB8 */  0x00, 0xFF, 0x33,
  /* 185, 0xB9 */  0x00, 0xFF, 0x00,
  /* 186, 0xBA */  0x00, 0xCC, 0xFF,
  /* 187, 0xBB */  0x00, 0xCC, 0xCC,
  /* 188, 0xBC */  0x00, 0xCC, 0x99,
  /* 189, 0xBD */  0x00, 0xCC, 0x66,
  /* 190, 0xBE */  0x00, 0xCC, 0x33,
  /* 191, 0xBF */  0x00, 0xCC, 0x00,
  /* 192, 0xC0 */  0x00, 0x99, 0xFF,
  /* 193, 0xC1 */  0x00, 0x99, 0xCC,
  /* 194, 0xC2 */  0x00, 0x99, 0x99,
  /* 195, 0xC3 */  0x00, 0x99, 0x66,
  /* 196, 0xC4 */  0x00, 0x99, 0x33,
  /* 197, 0xC5 */  0x00, 0x99, 0x00,
  /* 198, 0xC6 */  0x00, 0x66, 0xFF,
  /* 199, 0xC7 */  0x00, 0x66, 0xCC,
  /* 200, 0xC8 */  0x00, 0x66, 0x99,
  /* 201, 0xC9 */  0x00, 0x66, 0x66,
  /* 202, 0xCA */  0x00, 0x66, 0x33,
  /* 203, 0xCB */  0x00, 0x66, 0x00,
  /* 204, 0xCC */  0x00, 0x33, 0xFF,
  /* 205, 0xCD */  0x00, 0x33, 0xCC,
  /* 206, 0xCE */  0x00, 0x33, 0x99,
  /* 207, 0xCF */  0x00, 0x33, 0x66,
  /* 208, 0xD0 */  0x00, 0x33, 0x33,
  /* 209, 0xD1 */  0x00, 0x33, 0x00,
  /* 210, 0xD2 */  0x00, 0x00, 0xFF,
  /* 211, 0xD3 */  0x00, 0x00, 0xCC,
  /* 212, 0xD4 */  0x00, 0x00, 0x99,
  /* 213, 0xD5 */  0x00, 0x00, 0x66,
  /* 214, 0xD6 */  0x00, 0x00, 0x33,
  /* 215, 0xD7 */  0xEE, 0x00, 0x00,
  /* 216, 0xD8 */  0xDD, 0x00, 0x00,
  /* 217, 0xD9 */  0xBB, 0x00, 0x00,
  /* 218, 0xDA */  0xAA, 0x00, 0x00,
  /* 219, 0xDB */  0x88, 0x00, 0x00,
  /* 220, 0xDC */  0x77, 0x00, 0x00,
  /* 221, 0xDD */  0x55, 0x00, 0x00,
  /* 222, 0xDE */  0x44, 0x00, 0x00,
  /* 223, 0xDF */  0x22, 0x00, 0x00,
  /* 224, 0xE0 */  0x11, 0x00, 0x00,
  /* 225, 0xE1 */  0x00, 0xEE, 0x00,
  /* 226, 0xE2 */  0x00, 0xDD, 0x00,
  /* 227, 0xE3 */  0x00, 0xBB, 0x00,
  /* 228, 0xE4 */  0x00, 0xAA, 0x00,
  /* 229, 0xE5 */  0x00, 0x88, 0x00,
  /* 230, 0xE6 */  0x00, 0x77, 0x00,
  /* 231, 0xE7 */  0x00, 0x55, 0x00,
  /* 232, 0xE8 */  0x00, 0x44, 0x00,
  /* 233, 0xE9 */  0x00, 0x22, 0x00,
  /* 234, 0xEA */  0x00, 0x11, 0x00,
  /* 235, 0xEB */  0x00, 0x00, 0xEE,
  /* 236, 0xEC */  0x00, 0x00, 0xDD,
  /* 237, 0xED */  0x00, 0x00, 0xBB,
  /* 238, 0xEE */  0x00, 0x00, 0xAA,
  /* 239, 0xEF */  0x00, 0x00, 0x88,
  /* 240, 0xF0 */  0x00, 0x00, 0x77,
  /* 241, 0xF1 */  0x00, 0x00, 0x55,
  /* 242, 0xF2 */  0x00, 0x00, 0x44,
  /* 243, 0xF3 */  0x00, 0x00, 0x22,
  /* 244, 0xF4 */  0x00, 0x00, 0x11,
  /* 245, 0xF5 */  0xEE, 0xEE, 0xEE,
  /* 246, 0xF6 */  0xDD, 0xDD, 0xDD,
  /* 247, 0xF7 */  0xBB, 0xBB, 0xBB,
  /* 248, 0xF8 */  0xAA, 0xAA, 0xAA,
  /* 249, 0xF9 */  0x88, 0x88, 0x88,
  /* 250, 0xFA */  0x77, 0x77, 0x77,
  /* 251, 0xFB */  0x55, 0x55, 0x55,
  /* 252, 0xFC */  0x44, 0x44, 0x44,
  /* 253, 0xFD */  0x22, 0x22, 0x22,
  /* 254, 0xFE */  0x11, 0x11, 0x11,
  /* 255, 0xFF */  0x00, 0x00, 0x00
};

#endif /* AVFORMAT_QTPALETTE_H */
