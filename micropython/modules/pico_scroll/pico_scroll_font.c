#include "pico_scroll_font.h"

/* static font data */
static unsigned char __bitmap[256][5] = {
    {0x0, 0x0, 0x0, 0x0, 0x0},      {0x0, 0x0, 0x0, 0x0, 0x0},
    {0x0, 0x0, 0x0, 0x0, 0x0},      {0x0, 0x0, 0x0, 0x0, 0x0},
    {0x0, 0x0, 0x0, 0x0, 0x0},      {0x0, 0x0, 0x0, 0x0, 0x0},
    {0x0, 0x0, 0x0, 0x0, 0x0},      {0x0, 0x0, 0x0, 0x0, 0x0},
    {0x0, 0x0, 0x0, 0x0, 0x0},      {0x0, 0x0, 0x0, 0x0, 0x0},
    {0x0, 0x0, 0x0, 0x0, 0x0},      {0x0, 0x0, 0x0, 0x0, 0x0},
    {0x0, 0x0, 0x0, 0x0, 0x0},      {0x0, 0x0, 0x0, 0x0, 0x0},
    {0x0, 0x0, 0x0, 0x0, 0x0},      {0x0, 0x0, 0x0, 0x0, 0x0},
    {0x7f, 0x0, 0x0, 0x0, 0x0},     {0x7f, 0x7f, 0x0, 0x0, 0x0},
    {0x7f, 0x7f, 0x7f, 0x0, 0x0},   {0x7f, 0x7f, 0x7f, 0x7f, 0x0},
    {0x7f, 0x7f, 0x7f, 0x7f, 0x7f}, {0x0, 0x7f, 0x7f, 0x7f, 0x7f},
    {0x0, 0x0, 0x7f, 0x7f, 0x7f},   {0x0, 0x0, 0x0, 0x7f, 0x7f},
    {0x0, 0x0, 0x0, 0x0, 0x7f},     {0x60, 0x70, 0x3f, 0x2, 0x3c},
    {0x3, 0x3, 0x30, 0x48, 0x48},   {0x3, 0x7b, 0x28, 0x28, 0x8},
    {0x4, 0xc, 0x1c, 0xc, 0x4},     {0x0, 0x3e, 0x1c, 0x8, 0x0},
    {0x0, 0x8, 0x1c, 0x3e, 0x0},    {0x10, 0x18, 0x1c, 0x18, 0x10},
    {0x0, 0x0, 0x0, 0x0, 0x0},      {0x0, 0x0, 0x5f, 0x0, 0x0},
    {0x0, 0x7, 0x0, 0x7, 0x0},      {0x14, 0x7f, 0x14, 0x7f, 0x14},
    {0x24, 0x2a, 0x7f, 0x2a, 0x12}, {0x23, 0x13, 0x8, 0x64, 0x62},
    {0x36, 0x49, 0x55, 0x22, 0x50}, {0x0, 0x5, 0x3, 0x0, 0x0},
    {0x0, 0x1c, 0x22, 0x41, 0x0},   {0x0, 0x41, 0x22, 0x1c, 0x0},
    {0x8, 0x2a, 0x1c, 0x2a, 0x8},   {0x8, 0x8, 0x3e, 0x8, 0x8},
    {0x0, 0x50, 0x30, 0x0, 0x0},    {0x8, 0x8, 0x8, 0x8, 0x8},
    {0x0, 0x60, 0x60, 0x0, 0x0},    {0x20, 0x10, 0x8, 0x4, 0x2},
    {0x3e, 0x51, 0x49, 0x45, 0x3e}, {0x0, 0x42, 0x7f, 0x40, 0x0},
    {0x42, 0x61, 0x51, 0x49, 0x46}, {0x21, 0x41, 0x45, 0x4b, 0x31},
    {0x18, 0x14, 0x12, 0x7f, 0x10}, {0x27, 0x45, 0x45, 0x45, 0x39},
    {0x3c, 0x4a, 0x49, 0x49, 0x30}, {0x1, 0x71, 0x9, 0x5, 0x3},
    {0x36, 0x49, 0x49, 0x49, 0x36}, {0x6, 0x49, 0x49, 0x29, 0x1e},
    {0x0, 0x36, 0x36, 0x0, 0x0},    {0x0, 0x56, 0x36, 0x0, 0x0},
    {0x0, 0x8, 0x14, 0x22, 0x41},   {0x14, 0x14, 0x14, 0x14, 0x14},
    {0x41, 0x22, 0x14, 0x8, 0x0},   {0x2, 0x1, 0x51, 0x9, 0x6},
    {0x32, 0x49, 0x79, 0x41, 0x3e}, {0x7e, 0x11, 0x11, 0x11, 0x7e},
    {0x7f, 0x49, 0x49, 0x49, 0x36}, {0x3e, 0x41, 0x41, 0x41, 0x22},
    {0x7f, 0x41, 0x41, 0x22, 0x1c}, {0x7f, 0x49, 0x49, 0x49, 0x41},
    {0x7f, 0x9, 0x9, 0x1, 0x1},     {0x3e, 0x41, 0x41, 0x51, 0x32},
    {0x7f, 0x8, 0x8, 0x8, 0x7f},    {0x0, 0x41, 0x7f, 0x41, 0x0},
    {0x20, 0x40, 0x41, 0x3f, 0x1},  {0x7f, 0x8, 0x14, 0x22, 0x41},
    {0x7f, 0x40, 0x40, 0x40, 0x40}, {0x7f, 0x2, 0x4, 0x2, 0x7f},
    {0x7f, 0x4, 0x8, 0x10, 0x7f},   {0x3e, 0x41, 0x41, 0x41, 0x3e},
    {0x7f, 0x9, 0x9, 0x9, 0x6},     {0x3e, 0x41, 0x51, 0x21, 0x5e},
    {0x7f, 0x9, 0x19, 0x29, 0x46},  {0x46, 0x49, 0x49, 0x49, 0x31},
    {0x1, 0x1, 0x7f, 0x1, 0x1},     {0x3f, 0x40, 0x40, 0x40, 0x3f},
    {0x1f, 0x20, 0x40, 0x20, 0x1f}, {0x7f, 0x20, 0x18, 0x20, 0x7f},
    {0x63, 0x14, 0x8, 0x14, 0x63},  {0x3, 0x4, 0x78, 0x4, 0x3},
    {0x61, 0x51, 0x49, 0x45, 0x43}, {0x0, 0x0, 0x7f, 0x41, 0x41},
    {0x2, 0x4, 0x8, 0x10, 0x20},    {0x41, 0x41, 0x7f, 0x0, 0x0},
    {0x4, 0x2, 0x1, 0x2, 0x4},      {0x40, 0x40, 0x40, 0x40, 0x40},
    {0x0, 0x1, 0x2, 0x4, 0x0},      {0x20, 0x54, 0x54, 0x54, 0x78},
    {0x7f, 0x48, 0x44, 0x44, 0x38}, {0x38, 0x44, 0x44, 0x44, 0x20},
    {0x38, 0x44, 0x44, 0x48, 0x7f}, {0x38, 0x54, 0x54, 0x54, 0x18},
    {0x8, 0x7e, 0x9, 0x1, 0x2},     {0x8, 0x14, 0x54, 0x54, 0x3c},
    {0x7f, 0x8, 0x4, 0x4, 0x78},    {0x0, 0x44, 0x7d, 0x40, 0x0},
    {0x20, 0x40, 0x44, 0x3d, 0x0},  {0x0, 0x7f, 0x10, 0x28, 0x44},
    {0x0, 0x41, 0x7f, 0x40, 0x0},   {0x7c, 0x4, 0x18, 0x4, 0x78},
    {0x7c, 0x8, 0x4, 0x4, 0x78},    {0x38, 0x44, 0x44, 0x44, 0x38},
    {0x7c, 0x14, 0x14, 0x14, 0x8},  {0x8, 0x14, 0x14, 0x18, 0x7c},
    {0x7c, 0x8, 0x4, 0x4, 0x8},     {0x48, 0x54, 0x54, 0x54, 0x20},
    {0x4, 0x3f, 0x44, 0x40, 0x20},  {0x3c, 0x40, 0x40, 0x20, 0x7c},
    {0x1c, 0x20, 0x40, 0x20, 0x1c}, {0x3c, 0x40, 0x30, 0x40, 0x3c},
    {0x44, 0x28, 0x10, 0x28, 0x44}, {0xc, 0x50, 0x50, 0x50, 0x3c},
    {0x44, 0x64, 0x54, 0x4c, 0x44}, {0x0, 0x8, 0x36, 0x41, 0x0},
    {0x0, 0x0, 0x7f, 0x0, 0x0},     {0x0, 0x41, 0x36, 0x8, 0x0},
    {0x8, 0x8, 0x2a, 0x1c, 0x8},    {0x8, 0x1c, 0x2a, 0x8, 0x8},
    {0x70, 0x29, 0x24, 0x29, 0x70}, {0x70, 0x28, 0x25, 0x28, 0x70},
    {0x70, 0x2a, 0x25, 0x2a, 0x70}, {0x20, 0x54, 0x56, 0x55, 0x78},
    {0x20, 0x54, 0x55, 0x54, 0x78}, {0x7f, 0x7f, 0x49, 0x49, 0x49},
    {0x3d, 0x42, 0x42, 0x42, 0x3d}, {0x30, 0x4a, 0x48, 0x4a, 0x30},
    {0x5c, 0x32, 0x2a, 0x26, 0x1d}, {0x18, 0x64, 0x3c, 0x26, 0x18},
    {0x3d, 0x40, 0x40, 0x40, 0x3d}, {0x38, 0x42, 0x40, 0x22, 0x78},
    {0x15, 0x16, 0x7c, 0x16, 0x15}, {0x54, 0x34, 0x1c, 0x16, 0x15},
    {0x18, 0x4, 0x18, 0x20, 0x18},  {0x4a, 0x55, 0x55, 0x55, 0x29},
    {0x7e, 0x9, 0x7f, 0x9, 0x9},    {0x34, 0x54, 0x38, 0x54, 0x58},
    {0x48, 0x7e, 0x49, 0x49, 0x42}, {0x7f, 0x5, 0x15, 0x7a, 0x50},
    {0x1c, 0x3e, 0x3e, 0x3e, 0x1c}, {0x1c, 0x22, 0x22, 0x22, 0x1c},
    {0x8, 0x1c, 0x3e, 0x1c, 0x8},   {0x8, 0x14, 0x22, 0x14, 0x8},
    {0x0, 0x0, 0x77, 0x0, 0x0},     {0xe, 0x51, 0x31, 0x11, 0x11},
    {0x60, 0x50, 0x48, 0x44, 0x7e}, {0x40, 0x44, 0x4a, 0x51, 0x40},
    {0x40, 0x51, 0x4a, 0x44, 0x40}, {0x10, 0x38, 0x54, 0x10, 0x1f},
    {0x4, 0x2, 0x7f, 0x2, 0x4},     {0x10, 0x20, 0x7f, 0x20, 0x10},
    {0x0, 0x0, 0x0, 0x0, 0x0},      {0x70, 0x50, 0x70, 0x0, 0x0},
    {0x0, 0x0, 0xf, 0x1, 0x1},      {0x40, 0x40, 0x78, 0x0, 0x0},
    {0x10, 0x20, 0x40, 0x0, 0x0},   {0x0, 0x30, 0x30, 0x0, 0x0},
    {0x5, 0x5, 0x45, 0x25, 0x1f},   {0x0, 0x0, 0x0, 0x0, 0x0},
    {0x4, 0x44, 0x34, 0x14, 0xc},   {0x20, 0x10, 0x78, 0x4, 0x0},
    {0x18, 0x8, 0x4c, 0x38, 0x0},   {0x48, 0x28, 0x18, 0x7c, 0x8},
    {0x8, 0x7c, 0x8, 0x28, 0x18},   {0x40, 0x48, 0x48, 0x78, 0x40},
    {0x54, 0x54, 0x54, 0x7c, 0x0},  {0x18, 0x0, 0x58, 0x40, 0x38},
    {0x8, 0x8, 0x8, 0x8, 0x8},      {0x1, 0x41, 0x3d, 0x9, 0x7},
    {0x20, 0x10, 0x78, 0x4, 0x2},   {0xe, 0x2, 0x43, 0x22, 0x1e},
    {0x42, 0x42, 0x7e, 0x42, 0x42}, {0x22, 0x12, 0xa, 0x7f, 0x2},
    {0x42, 0x3f, 0x2, 0x42, 0x3e},  {0xa, 0xa, 0x7f, 0xa, 0xa},
    {0x4, 0x43, 0x41, 0x21, 0x1f},  {0x4, 0x3, 0x42, 0x3e, 0x2},
    {0x41, 0x41, 0x41, 0x41, 0x7f}, {0x2, 0x4f, 0x22, 0x1f, 0x2},
    {0x4a, 0x4a, 0x40, 0x20, 0x1c}, {0x42, 0x22, 0x12, 0x2a, 0x46},
    {0x2, 0x3f, 0x42, 0x4a, 0x46},  {0x3, 0x44, 0x40, 0x20, 0x1e},
    {0x8, 0x46, 0x4a, 0x32, 0x1e},  {0xa, 0x4a, 0x3e, 0x9, 0x8},
    {0xe, 0x0, 0x4e, 0x20, 0x1e},   {0x4, 0x45, 0x3d, 0x5, 0x4},
    {0x0, 0x7f, 0x8, 0x10, 0x0},    {0x44, 0x24, 0x1f, 0x4, 0x4},
    {0x40, 0x42, 0x42, 0x42, 0x40}, {0x42, 0x2a, 0x12, 0x2a, 0x6},
    {0x22, 0x12, 0x7b, 0x16, 0x22}, {0x0, 0x40, 0x20, 0x1f, 0x0},
    {0x78, 0x0, 0x2, 0x4, 0x78},    {0x3f, 0x44, 0x44, 0x44, 0x44},
    {0x2, 0x42, 0x42, 0x22, 0x1e},  {0x4, 0x2, 0x4, 0x8, 0x30},
    {0x32, 0x2, 0x7f, 0x2, 0x32},   {0x2, 0x12, 0x22, 0x52, 0xe},
    {0x0, 0x2a, 0x2a, 0x2a, 0x40},  {0x38, 0x24, 0x22, 0x20, 0x70},
    {0x40, 0x28, 0x10, 0x28, 0x6},  {0x2, 0x3e, 0x4a, 0x4a, 0x4a},
    {0x4, 0x7f, 0x4, 0x14, 0xc},    {0x40, 0x42, 0x42, 0x7e, 0x40},
    {0x4a, 0x4a, 0x4a, 0x4a, 0x7e}, {0x4, 0x5, 0x45, 0x25, 0x1c},
    {0xf, 0x40, 0x20, 0x1f, 0x0},   {0x7c, 0x0, 0x7e, 0x40, 0x30},
    {0x7e, 0x40, 0x20, 0x10, 0x8},  {0x7e, 0x42, 0x42, 0x42, 0x7e},
    {0xe, 0x2, 0x42, 0x22, 0x1e},   {0x42, 0x42, 0x40, 0x20, 0x18},
    {0x2, 0x4, 0x1, 0x2, 0x0},      {0x7, 0x5, 0x7, 0x0, 0x0},
    {0x38, 0x44, 0x48, 0x30, 0x40}, {0x20, 0x55, 0x54, 0x55, 0x78},
    {0x7c, 0x2a, 0x2a, 0x2a, 0x14}, {0x28, 0x54, 0x54, 0x44, 0x20},
    {0x7e, 0x10, 0x10, 0x8, 0x1e},  {0x38, 0x44, 0x4c, 0x54, 0x24},
    {0x78, 0x14, 0x12, 0x12, 0xc},  {0x8, 0x54, 0x54, 0x54, 0x3c},
    {0x20, 0x40, 0x3c, 0x4, 0x4},   {0x2, 0x2, 0x0, 0x7, 0x0},
    {0x20, 0x40, 0x44, 0x3d, 0x0},  {0x5, 0x2, 0x5, 0x0, 0x0},
    {0x18, 0x24, 0x7e, 0x24, 0x10}, {0x14, 0x7f, 0x54, 0x40, 0x40},
    {0x7c, 0x9, 0x5, 0x5, 0x78},    {0x38, 0x45, 0x44, 0x45, 0x38},
    {0x7e, 0x14, 0x12, 0x12, 0xc},  {0xc, 0x12, 0x12, 0x14, 0x7e},
    {0x3c, 0x4a, 0x4a, 0x4a, 0x3c}, {0x30, 0x28, 0x10, 0x28, 0x18},
    {0x58, 0x64, 0x4, 0x64, 0x58},  {0x3c, 0x41, 0x40, 0x21, 0x7c},
    {0x63, 0x55, 0x49, 0x41, 0x41}, {0x44, 0x3c, 0x4, 0x7c, 0x44},
    {0x45, 0x29, 0x11, 0x29, 0x45}, {0xc, 0x50, 0x50, 0x50, 0x3c},
    {0xa, 0xa, 0x7e, 0xa, 0x9},     {0x44, 0x3c, 0x14, 0x14, 0x74},
    {0x7c, 0x14, 0x1c, 0x14, 0x7c}, {0x8, 0x8, 0x2a, 0x8, 0x8},
    {0x0, 0x0, 0x0, 0x0, 0x0},      {0x7f, 0x7f, 0x7f, 0x7f, 0x7f},
};

/* render a text string to a pre-allocated buffer - strlen(text) * 6 bytes */
int render(unsigned char *text, int nchr, unsigned char *buffer, int nbfr) {
    // TODO check nbfr >= 6 * nchr

    for (int i = 0; i < nchr; i++) {
        unsigned char *symbol = __bitmap[text[i]];
	for (int j = 0; j < 5; j++) {
	    buffer[i * 6 + j] = symbol[j];
	}
	buffer[i * 6 + 6] = 0x0;
    }
}
