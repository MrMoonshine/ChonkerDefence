#pragma once
#include <stdio.h>
#include <stdint.h>

#define LEVEL_WIDTH 32
#define LEVEL_HEIGHT 18
#define LEVEL_PERIMETER_NODES 8
#define LEVEL_TITLE_LENGTH 64
#define LEVEL_STYLE_LENGTH 32
#define LEVEL_RESERVED_BYTE_COUNT 16
//Map data
#define LEVEL_NODE_CODE_PATH        0b0000
#define LEVEL_NODE_CODE_WATER       0b0001
#define LEVEL_NODE_CODE_GRASS       0b0010
#define LEVEL_NODE_CODE_BRIDGE      0b0011
#define LEVEL_NODE_CODE_PATH_FWD    0b0100
#define LEVEL_NODE_CODE_UNDEF1      0b0101
#define LEVEL_NODE_CODE_CAT1        0b0110
#define LEVEL_NODE_CODE_CAT0        0b0111
//Neighbor adjacencies
//Straight is detected by %5 == 0
#define LEVEL_NODE_AJACENCY_STRAIGHT_NS 0x5
#define LEVEL_NODE_AJACENCY_STRAIGHT_EW 0xA
//Curve is detected by %3 == 0
#define LEVEL_NODE_AJACENCY_CURVE_NE 0x3
#define LEVEL_NODE_AJACENCY_CURVE_ES 0x6
#define LEVEL_NODE_AJACENCY_CURVE_WN 0x9
#define LEVEL_NODE_AJACENCY_CURVE_WS 0xC
//Intersect
#define LEVEL_NODE_AJACENCY_INTERSECTION_NES 0x7
#define LEVEL_NODE_AJACENCY_INTERSECTION_NEW 0xB
#define LEVEL_NODE_AJACENCY_INTERSECTION_NSW 0xD
#define LEVEL_NODE_AJACENCY_INTERSECTION_ESW 0xE

int8_t levelNodeInfoAt(uint8_t *data, size_t size, uint8_t width, uint8_t x, uint8_t y);
void levelSetNodeInfoAt(uint8_t *data, size_t size, uint8_t width, uint8_t x, uint8_t y, uint8_t node);
